//
// MIT License
//
// Copyright (c) 2018 Dmitriy Korobochkin, Vitaliy Garmash.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imageviewer.h"
#include "HistogramWidget.h"
#include "imagetransformer.h"

#include "qcustomplot.h"

#include <QAction>
#include <QMenu>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QIcon>
#include <QMessageBox>
#include <QInputDialog>
#include <QElapsedTimer>
#include <QMouseEvent>

#include <exception>

#include "ImageParametersCalculator.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    mUi(new Ui::MainWindow),
    mRubberBand(nullptr),
    mViewer(new ImageViewer),
    mHist(new HistogramWidget),
    mCurOpenedImg(-1),
    mCurProcessedImg(-1),
    mMouseMode(MouseMode::SELECT_PIXEL)
{
    mUi->setupUi(this);
    setCentralWidget(mViewer);

    setMouseTracking(true);

    CreateActions();
    CreateMainMenus();
    CreateStatusBar();

    connect(this, SIGNAL(CurImgWasUpdated()), this, SLOT(DrawCurImg()));
    connect(this, SIGNAL(CurImgWasUpdated()), this, SLOT(MoveToCenterOfDisplay()));
    connect(this, SIGNAL(CurImgWasUpdated()), this, SLOT(UpdateImgsMenu()));
    connect(this, SIGNAL(OpenedImgsChanged()), this, SLOT(UpdateImgsMenu()));
    connect(this, SIGNAL(ProcessedImgsChanged()), this, SLOT(UpdateImgsMenu()));
    connect(this, SIGNAL(AllImgsAreClosed()), this, SLOT(ClearDisplay()));
    connect(this, SIGNAL(AllImgsAreClosed()), this, SLOT(UpdateImgsMenu()));
    connect(this, SIGNAL(ExecuteHuMomentsForBoundary(int,int,int,int)),
            this, SLOT(CalcHuMomentsExecute(int,int,int,int)));

    move((qApp->desktop()->width() - width()) / 2, (qApp->desktop()->height() - height()) / 2);
}

MainWindow::~MainWindow()
{
    delete mUi;
    delete mViewer;
    delete mHist;
}

void MainWindow::CreateFileActions()
{
    mOpenAction = new QAction(tr("Open image"), this);
    mOpenAction->setShortcut(QKeySequence::New);
    mOpenAction->setStatusTip(tr("Open image to processing"));
    connect(mOpenAction, SIGNAL(triggered()), this, SLOT(OpenImgFile()));

    mCloseAction = new QAction(tr("Close image"), this);
    mCloseAction->setShortcut(QKeySequence::Close);
    mCloseAction->setStatusTip(tr("Close current image"));
    connect(mCloseAction, SIGNAL(triggered()), this, SLOT(CloseImgFile()));

    mSaveAction = new QAction(tr("Save image"), this);
    mSaveAction->setShortcut(QKeySequence::Save);
    mSaveAction->setStatusTip(tr("Save current image"));
    connect(mSaveAction, SIGNAL(triggered()), this, SLOT(SaveImgFile()));

    mExitAction = new QAction(tr("Exit"), this);
    mExitAction->setShortcut(tr("Ctrl+Q"));
    mExitAction->setStatusTip(tr("Exit from model. Not saved data will be lose"));
    connect(mExitAction, SIGNAL(triggered()), this, SLOT(Exit()));
}

void MainWindow::CreateFilteringActions()
{
    mMedianBlurAction = new QAction(tr("Median filter"), this);
    mMedianBlurAction->setShortcut(tr("Ctrl+B"));
    mMedianBlurAction->setStatusTip(tr("Median filtration of current image"));
    connect(mMedianBlurAction, SIGNAL(triggered()), this, SLOT(MedianBlur()));

    mGaussianBlurAction = new QAction(tr("Gaussian filter"), this);
    mGaussianBlurAction->setShortcut(tr("Ctrl+G"));
    mGaussianBlurAction->setStatusTip(tr("Gaussian filtration of current image"));
    connect(mGaussianBlurAction, SIGNAL(triggered()), this, SLOT(GaussianBlur()));

    mSepGaussianBlurAction = new QAction(tr("Separated gaussian filter"), this);
    mSepGaussianBlurAction->setStatusTip(tr("Separated gaussian blur"));
    connect(mSepGaussianBlurAction, SIGNAL(triggered()), this, SLOT(SeparateGaussianBlur()));

    mIIRGaussianBlurAction = new QAction(tr("IIR-gaussian filter"), this);
    mIIRGaussianBlurAction->setStatusTip(tr("IIR-imitated gaussian blur"));
    connect(mIIRGaussianBlurAction, SIGNAL(triggered()), this, SLOT(IIRGaussianBlur()));

    mSharpenAction = new QAction(tr("Sharpen filter"), this);
    mSharpenAction->setStatusTip(tr("Increase the sharpness of the image"));
    connect(mSharpenAction, SIGNAL(triggered()), this, SLOT(Sharpen()));

    mAdaptiveThresholdAction = new QAction(tr("Adaptive threshold"), this);
    mAdaptiveThresholdAction->setStatusTip(tr("Select the pixels by threshold"));
    connect(mAdaptiveThresholdAction, SIGNAL(triggered()), this, SLOT(AdaptiveThreshold()));
}

void MainWindow::CreateCorrectorActions()
{
    mSingleScaleRetinexAction = new QAction(tr("Single Scale Retinex"), this);
    mSingleScaleRetinexAction->setStatusTip(tr("Single Scale Retinex"));
    connect(mSingleScaleRetinexAction, SIGNAL(triggered()), this, SLOT(SingleScaleRetinex()));

    mAutoLevelsAction = new QAction(tr("Autolevels"), this);
    mAutoLevelsAction->setStatusTip(tr("Run the autolevels algorithm"));
    connect(mAutoLevelsAction, SIGNAL(triggered()), this, SLOT(AutoLevels()));

    mNormAutoLevelsAction = new QAction(tr("Norm autolevels"), this);
    mNormAutoLevelsAction->setStatusTip(tr("Run the norm autolevels algorithm"));
    connect(mNormAutoLevelsAction, SIGNAL(triggered()), this, SLOT(NormAutoLevels()));

    mGammaAction = new QAction(tr("Gamma-correction"), this);
    mGammaAction->setStatusTip(tr("Run the gamma-correction"));
    connect(mGammaAction, SIGNAL(triggered()), this, SLOT(Gamma()));
}

void MainWindow::CreateOperatorActions()
{
    mSobelAction = new QAction(tr("Sobel operator"), this);
    mSobelAction->setStatusTip(tr("Convolution of current image with Sobel operator"));
    connect(mSobelAction, SIGNAL(triggered()), this, SLOT(Sobel()));

    mScharrAction = new QAction(tr("Scharr operator"), this);
    mScharrAction->setStatusTip(tr("Convolution of current image with Scharr operator"));
    connect(mScharrAction, SIGNAL(triggered()), this, SLOT(Scharr()));
}

void MainWindow::CreateBodersDetectorsActions()
{
    mCannyAction = new QAction(tr("Canny algorithm"), this);
    mCannyAction->setStatusTip(tr("Detect the borders by using Canny algorithm"));
    connect(mCannyAction, SIGNAL(triggered()), this, SLOT(Canny()));

    mSobelDetectorAction = new QAction(tr("Sobel algorithm"), this);
    mSobelDetectorAction->setStatusTip(tr("Detect the borders by using Sobel algorithm"));
    connect(mSobelDetectorAction, SIGNAL(triggered()), this, SLOT(SobelDetector()));

    mScharrDetectorAction = new QAction(tr("Scharr algorithm"), this);
    mScharrDetectorAction->setStatusTip(tr("Detect the borders by using Scharr algorithm"));
    connect(mScharrDetectorAction, SIGNAL(triggered()), this, SLOT(ScharrDetector()));
}

void MainWindow::CreateParamsActions()
{
    mImgEntropyAction = new QAction(tr("Entropy"), this);
    mImgEntropyAction->setShortcut(tr("Ctrl+E"));
    mImgEntropyAction->setStatusTip(tr("Calculate the entropy of image (measure of informative)"));
    connect(mImgEntropyAction, SIGNAL(triggered()), this, SLOT(CalcEntropy()));

    mImgAverBrightnessAction = new QAction(tr("Average of brightness"), this);
    mImgAverBrightnessAction->setStatusTip(tr("Calculate the average of brightness"));
    connect(mImgAverBrightnessAction, SIGNAL(triggered()), this, SLOT(CalcAverageBrightness()));

    mImgStdDeviationAction = new QAction(tr("Standard deviation"), this);
    mImgStdDeviationAction->setStatusTip(tr("Calculate the image standard deviation"));
    connect(mImgStdDeviationAction, SIGNAL(triggered()), this, SLOT(CalcStandardDeviation()));

    mImgMinMaxBrightnessAction = new QAction(tr("Minimum and maximum of brightness"), this);
    mImgMinMaxBrightnessAction->setStatusTip(tr("Calculate minimum and maximum of brightness"));
    connect(mImgMinMaxBrightnessAction, SIGNAL(triggered()), this, SLOT(CalcMinMaxBrightness()));

    mImgCreateBrightnessHistogramAction = new QAction(tr("Histogram of brightness"), this);
    mImgCreateBrightnessHistogramAction->setStatusTip(tr("Creating histogram of brightness"));
    connect(mImgCreateBrightnessHistogramAction, SIGNAL(triggered()), this, SLOT(CreateBrightnessHistogram()));

    mHuMomentsAction = new QAction(tr("Hu's moments"), this);
    mHuMomentsAction->setShortcut(tr("Ctrl+H"));
    mHuMomentsAction->setStatusTip(tr("Calculate the Hu's moments"));
    connect(mHuMomentsAction, SIGNAL(triggered()), this, SLOT(CalcHuMomentsStart()));

    mIntlQualIndAction = new QAction(tr("Integral quality indicator"), this);
    mIntlQualIndAction->setStatusTip(tr("Calculate the integral quality indicator of image"));
    connect(mIntlQualIndAction, SIGNAL(triggered()), this, SLOT(CalcIntegralQualityIndicator()));
}

void MainWindow::CreateCombiningActions()
{
    mInfPriorCombAction = new QAction(tr("Priority of the most informative image"), this);
    mInfPriorCombAction->setShortcut(tr("Ctrl+I"));
    mInfPriorCombAction->setStatusTip(tr("Combine the all opened images by the method of priority of the most informative image"));
    connect(mInfPriorCombAction, SIGNAL(triggered()), this, SLOT(InformPriorityCombining()));

    mMorphCombAction = new QAction(tr("Morphological"), this);
    mMorphCombAction->setShortcut(tr("Ctrl+M"));
    mMorphCombAction->setStatusTip(tr("Combine the all opened images by the morphological method"));
    connect(mMorphCombAction, SIGNAL(triggered()), this, SLOT(MorphologicalCombining()));

    mLocEntrCombAction = new QAction(tr("Local-entropy"), this);
    mLocEntrCombAction->setShortcut(tr("Ctrl+L"));
    mLocEntrCombAction->setStatusTip(tr("Combine the all opened images by the local-entropy method"));
    connect(mLocEntrCombAction, SIGNAL(triggered()), this, SLOT(LocalEntropyCombining()));

    mDifAddCombAction = new QAction(tr("Differences adding"), this);
    mDifAddCombAction->setShortcut(tr("Ctrl+A"));
    mDifAddCombAction->setStatusTip(tr("Combine the all opened images by the differences adding algorithm"));
    connect(mDifAddCombAction, SIGNAL(triggered()), this, SLOT(DifferencesAddingCombining()));

    mCalcDiffAction = new QAction(tr("Calculate the difference"), this);
    mCalcDiffAction->setStatusTip(tr("Calculation the difference of two open images"));
    connect(mCalcDiffAction, SIGNAL(triggered()), this, SLOT(CalcDiff()));
}

void MainWindow::CreateActions()
{
    CreateFileActions();
    CreateFilteringActions();
    CreateCorrectorActions();
    CreateParamsActions();
    CreateOperatorActions();
    CreateBodersDetectorsActions();
    CreateCombiningActions();
}

void MainWindow::CreateFileMenu()
{
    mFileMenu = menuBar()->addMenu(tr("File"));

    mFileMenu->addAction(mOpenAction);
    mFileMenu->addAction(mCloseAction);
    mFileMenu->addAction(mSaveAction);
    mFileMenu->addSeparator();
    mFileMenu->addAction(mExitAction);
}

void MainWindow::CreateFilteringMenu()
{
    mFilterMenu = mProcessingMenu->addMenu(tr("Filtration"));

    mFilterMenu->addAction(mMedianBlurAction);
    mFilterMenu->addAction(mGaussianBlurAction);
    mFilterMenu->addAction(mSepGaussianBlurAction);
    mFilterMenu->addAction(mIIRGaussianBlurAction);
    mFilterMenu->addAction(mSharpenAction);
    mFilterMenu->addAction(mAdaptiveThresholdAction);
}

void MainWindow::CreateCorrectorMenu()
{
    mCorrectorMenu = mProcessingMenu->addMenu(tr("Correction"));

    mCorrectorMenu->addAction(mSingleScaleRetinexAction);
    mCorrectorMenu->addAction(mAutoLevelsAction);
    mCorrectorMenu->addAction(mNormAutoLevelsAction);
    mCorrectorMenu->addAction(mGammaAction);
}

void MainWindow::CreateOperatorsMenu()
{
    mOperatorsMenu = mProcessingMenu->addMenu(tr("Operators"));

    mOperatorsMenu->addAction(mSobelAction);
    mOperatorsMenu->addAction(mScharrAction);
}

void MainWindow::CreateBordersDetectorsMenu()
{
    mBordersDetectorsMenu = mProcessingMenu->addMenu(tr("Detectors of borders"));

    mBordersDetectorsMenu->addAction(mCannyAction);
    mBordersDetectorsMenu->addAction(mSobelDetectorAction);
    mBordersDetectorsMenu->addAction(mScharrDetectorAction);
}

void MainWindow::CreateParamsMenu()
{
    mImgParams = mProcessingMenu->addMenu(tr("Parameters"));

    mImgParams->addAction(mImgEntropyAction);
    mImgParams->addAction(mImgAverBrightnessAction);
    mImgParams->addAction(mImgStdDeviationAction);
    mImgParams->addAction(mImgMinMaxBrightnessAction);
    mImgParams->addAction(mImgCreateBrightnessHistogramAction);
    mImgParams->addAction(mHuMomentsAction);
    mImgParams->addAction(mIntlQualIndAction);
}

void MainWindow::CreateCombiningMenu()
{
    mCombineMenu = menuBar()->addMenu(tr("Combination"));

    mCombineMenu->addAction(mInfPriorCombAction);
    mCombineMenu->addAction(mMorphCombAction);
    mCombineMenu->addAction(mLocEntrCombAction);
    mCombineMenu->addAction(mDifAddCombAction);
    mCombineMenu->addAction(mCalcDiffAction);
}

void MainWindow::CreateMainMenus()
{
    CreateFileMenu();
    mImgsMenu = menuBar()->addMenu(tr("Images"));
    mProcessingMenu = menuBar()->addMenu(tr("Processing"));
    CreateParamsMenu();
    CreateFilteringMenu();
    CreateCorrectorMenu();
    CreateOperatorsMenu();
    CreateBordersDetectorsMenu();
    CreateCombiningMenu();
}

void MainWindow::CreateStatusBar()
{
    statusBarLabel = new QLabel(this);
    statusBarLabel->setAlignment(Qt::AlignHCenter);
    statusBarLabel->setMinimumSize(statusBarLabel->sizeHint());

    statusBar()->addWidget(statusBarLabel);

    connect(this, SIGNAL(UpdateCoordsAndBrigInStatusBar(int,int)),
            this, SLOT(DisplayCoordsAndBrigInStatusBar(int,int)));
}

bool MainWindow::LoadImgFromFile(const QString& fileName)
{
    QImage img(fileName);
    bool ret = !img.isNull();
    if (ret)
        mOpenedImgs.push_back(ImageTransormer::QImage2AImage(img));
    return ret;
}

acv::Image& MainWindow::GetCurImg()
{
    if (mCurOpenedImg != -1)
        return mOpenedImgs[mCurOpenedImg];
    else if (mCurProcessedImg != -1)
        return mProcessedImgs[mCurProcessedImg];
    else
        // This situation should not to be
        throw std::out_of_range("No images selected");
}

const acv::Image& MainWindow::GetCurImg() const
{
    if (mCurOpenedImg != -1)
        return mOpenedImgs[mCurOpenedImg];
    else if (mCurProcessedImg != -1)
        return mProcessedImgs[mCurProcessedImg];
    else
        // This situation should not to be
        throw std::out_of_range("No images selected");
}

void MainWindow::DrawCurImg()
{
    mViewer->DrawImage(GetCurImg());
}

void MainWindow::MoveToCenterOfDisplay()
{
    const int MENU_BAR_HEIGHT = statusBar()->height() + menuBar()->height();
    const acv::Image& curImg = GetCurImg();

    setGeometry(0, 0, curImg.GetWidth(), curImg.GetHeight() + MENU_BAR_HEIGHT);
    move((qApp->desktop()->width() - width()) / 2, (qApp->desktop()->height() - height()) / 2);
}

void MainWindow::UpdateImgsMenu()
{
    mImgsMenu->actions().clear();

    int numAction = 0;
    for(auto& act : mOpenedImgsActions)
    {
        act->setChecked(numAction == mCurOpenedImg);
        ++numAction;

        mImgsMenu->addAction(act);
    }

    if (!mProcessedImgs.empty())
    {
        if (!mOpenedImgs.empty())
            mImgsMenu->addSeparator();

        numAction = 0;
        for(auto& act : mProcessedImgsActions)
        {
            act->setChecked(numAction == mCurProcessedImg);
            ++numAction;

            mImgsMenu->addAction(act);
        }
    }
}

int MainWindow::GetNumOfActionInVec(const QAction* action, const std::vector<QAction*>& actionsVec)
{
    std::vector<QAction*>::const_iterator foundIt = std::find(actionsVec.begin(), actionsVec.end(), action);
    if (foundIt != actionsVec.end())
        return (foundIt - actionsVec.begin());
    else
        return -1;
}

int MainWindow::GetNumOfOpenedImgByAction(const QAction* action)
{
    return GetNumOfActionInVec(action, mOpenedImgsActions);
}

int MainWindow::GetNumOfProcessedImgByAction(const QAction* action)
{
    return GetNumOfActionInVec(action, mProcessedImgsActions);
}

void MainWindow::SetCurImage()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action)
    {
        mCurOpenedImg = GetNumOfOpenedImgByAction(action);
        mCurProcessedImg = GetNumOfProcessedImgByAction(action);

        if (ImgWasSelected())
            emit CurImgWasUpdated();
    }
}

void MainWindow::MedianBlur()
{
    Filtering(acv::ImageFilter::FilterType::MEDIAN);
}

void MainWindow::GaussianBlur()
{
    Filtering(acv::ImageFilter::FilterType::GAUSSIAN);
}

void MainWindow::Sharpen()
{
    Filtering(acv::ImageFilter::FilterType::SHARPEN);
}

void MainWindow::SeparateGaussianBlur()
{
    Filtering(acv::ImageFilter::FilterType::SEP_GAUSSIAN);
}

void MainWindow::IIRGaussianBlur()
{
    Filtering(acv::ImageFilter::FilterType::IIR_GAUSSIAN);
}

void MainWindow::SingleScaleRetinex()
{
    Correct(acv::ImageCorrector::CorrectorType::SSRETINEX);
}

void MainWindow::AutoLevels()
{
    Correct(acv::ImageCorrector::CorrectorType::AUTO_LEVELS);
}

void MainWindow::NormAutoLevels()
{
    Correct(acv::ImageCorrector::CorrectorType::NORM_AUTO_LEVELS);
}

void MainWindow::Gamma()
{
    Correct(acv::ImageCorrector::CorrectorType::GAMMA);
}

void MainWindow::Operator(acv::BordersDetector::DetectorType operatorType)
{
    if (ImgWasSelected())
    {
        acv::BordersDetector::OperatorTypes type;
        if (QMessageBox::question(this, tr("Operator"), "Need the horizontal operator? Else will be vertical.") == QMessageBox::Yes)
            type = acv::BordersDetector::OperatorTypes::HORIZONTAL;
        else
            type = acv::BordersDetector::OperatorTypes::VERTICAL;

        const acv::Image& curImg = GetCurImg();
        acv::Image processedImg(curImg.GetHeight(), curImg.GetWidth());

        QElapsedTimer timer;
        timer.start();

        bool filtred = acv::BordersDetector::OperatorConvolution(curImg, processedImg, operatorType, type);

        qint64 filterTime = timer.elapsed();

        if (filtred)
        {
            QString actionName = FormOperatorActionName(operatorType, type);
            AddProcessedImg(processedImg, actionName);

            QMessageBox::information(this, tr("Operator"), tr("Time of convolution: %1 msec").arg(filterTime), QMessageBox::Ok);
        }
        else
            QMessageBox::warning(this, tr("Operator"), tr("Could not convolution"), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(this, tr("Operator"), tr("No image selected"), QMessageBox::Ok);
    }
}

void MainWindow::Sobel()
{
    Operator(acv::BordersDetector::DetectorType::SOBEL);
}

void MainWindow::Scharr()
{
    Operator(acv::BordersDetector::DetectorType::SCHARR);
}

void MainWindow::AddProcessedImg(const acv::Image& processedImg, const QString& actionName)
{
    mProcessedImgs.push_back(processedImg);
    mProcImgsSavingFlags.push_back(false);

    mCurOpenedImg = -1;
    mCurProcessedImg = mProcessedImgs.size() - 1;
    emit CurImgWasUpdated();

    AddProcessedImgAction(actionName);
    emit ProcessedImgsChanged();
}

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    switch (mMouseMode)
    {
    case MouseMode::SELECT_PIXEL:
        break;
    case MouseMode::SELECT_BOUNDARY:
        mRBBeginPos = event->pos();
        if (!mRubberBand)
            mRubberBand = new QRubberBand(QRubberBand::Rectangle, this);
        mRubberBand->setGeometry(QRect(mRBBeginPos, QSize()));
        mRubberBand->show();

        break;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    switch (mMouseMode)
    {
    case MouseMode::SELECT_PIXEL:
    {
        QPoint curPos = event->pos();
        RecalcToCentralWidgetCoordinates(curPos);

        emit UpdateCoordsAndBrigInStatusBar(curPos.x(), curPos.y());

        break;
    }
    case MouseMode::SELECT_BOUNDARY:
        mRBEndPos = event->pos();
        mRubberBand->setGeometry(QRect(mRBBeginPos, mRBEndPos).normalized());

        break;
    }
}

void MainWindow::RecalcToCentralWidgetCoordinates(QPoint& pnt)
{
    pnt.setX(pnt.x() - mViewer->x());
    pnt.setY(pnt.y() - mViewer->y());
}

void MainWindow::mouseReleaseEvent(QMouseEvent* /*event*/)
{
    switch (mMouseMode)
    {
    case MouseMode::SELECT_PIXEL:
        break;
    case MouseMode::SELECT_BOUNDARY:
        mRubberBand->hide();

        RecalcToCentralWidgetCoordinates(mRBBeginPos);
        RecalcToCentralWidgetCoordinates(mRBEndPos);

        int xMin = std::min(mRBBeginPos.x(), mRBEndPos.x());
        int xMax = std::max(mRBBeginPos.x(), mRBEndPos.x());
        int yMin = std::min(mRBBeginPos.y(), mRBEndPos.y());
        int yMax = std::max(mRBBeginPos.y(), mRBEndPos.y());

        emit ExecuteHuMomentsForBoundary(xMin, xMax, yMin, yMax);

        break;
    }
}

void MainWindow::Canny()
{
    DetectBorders(acv::BordersDetector::DetectorType::CANNY);
}

void MainWindow::SobelDetector()
{
    DetectBorders(acv::BordersDetector::DetectorType::SOBEL);
}

void MainWindow::ScharrDetector()
{
    DetectBorders(acv::BordersDetector::DetectorType::SCHARR);
}

void MainWindow::DetectBorders(acv::BordersDetector::DetectorType detectorType)
{
    if (ImgWasSelected())
    {
        const acv::Image& curImg = GetCurImg();
        acv::Image processedImg(curImg.GetHeight(), curImg.GetWidth());

        QElapsedTimer timer;
        timer.start();

        bool detected = acv::BordersDetector::DetectBorders(curImg, processedImg, detectorType);

        qint64 filterTime = timer.elapsed();

        if (detected)
        {
            QString actionName = FormBordersDetectorActionName(detectorType);
            AddProcessedImg(processedImg, actionName);

            QMessageBox::information(this, tr("Detecting of the borders"), tr("Time of detecting: %1 мсек").arg(filterTime), QMessageBox::Ok);
        }
        else
            QMessageBox::warning(this, tr("Detecting of the borders"), tr("Could not detect the borders"), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(this, tr("Detecting of the borders"), tr("Image was not selected"), QMessageBox::Ok);
    }
}

QString MainWindow::FormFiltrationResultStr(acv::FiltrationResult filtRes)
{
    switch (filtRes)
    {
    case acv::FiltrationResult::INCORRECT_FILTER_SIZE:
        return tr("Incorrect filter size. Filter size should be odd.");
    case acv::FiltrationResult::INCORRECT_FILTER_TYPE:
        return tr("Unknown filter type");
    case acv::FiltrationResult::INTERNAL_ERROR:
        return tr("Error during filtration");
    case acv::FiltrationResult::SMALL_FILTER_SIZE:
        return tr("Filter size should be >= 6");
    case acv::FiltrationResult::SUCCESS:
        return tr("Success of filtration");
    default:
        return tr("Unknown error");
    }
}

void MainWindow::Filtering(acv::ImageFilter::FilterType filterType)
{
    if (ImgWasSelected())
    {
        const int DEFAULT_FILTER_SIZE = 3, MIN_FILTER_SIZE = 1, MAX_FILTER_SIZE = 15, FILTER_SIZE_STEP = 2;
        int filterSize = -1;
        if (filterType != acv::ImageFilter::FilterType::SHARPEN)
        {
            filterSize = QInputDialog::getInt(this, tr("Enter the filter size (odd positive number)"), tr("Filter size"),
                                              DEFAULT_FILTER_SIZE, MIN_FILTER_SIZE, MAX_FILTER_SIZE, FILTER_SIZE_STEP);
        }

        const acv::Image& curImg = GetCurImg();
        acv::Image processedImg(curImg.GetHeight(), curImg.GetWidth());

        QElapsedTimer timer;
        timer.start();

        acv::FiltrationResult filtRes = acv::ImageFilter::Filter(curImg, processedImg, filterType, filterSize);

        qint64 filterTime = timer.elapsed();

        if (filtRes == acv::FiltrationResult::SUCCESS)
        {
            QString actionName = FormFilterActionName(filterType, filterSize);
            AddProcessedImg(processedImg, actionName);

            QMessageBox::information(this, tr("Image filtration"), tr("Time of filtration: %1 msec").arg(filterTime), QMessageBox::Ok);
        }
        else
            QMessageBox::warning(this, tr("Image filtration"), FormFiltrationResultStr(filtRes), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(this, tr("Image filtration"), tr("No image selected"), QMessageBox::Ok);
    }
}

void MainWindow::Correct(acv::ImageCorrector::CorrectorType corType)
{
    if (ImgWasSelected())
    {
        const acv::Image& curImg = GetCurImg();
        acv::Image processedImg(curImg.GetHeight(), curImg.GetWidth());

        QElapsedTimer timer;
        timer.start();

        bool corrected = acv::ImageCorrector::Correct(curImg, processedImg, corType);

        qint64 correctionTime = timer.elapsed();

        if (corrected)
        {
            QString actionName = FormCorrectorActionName(corType);
            AddProcessedImg(processedImg, actionName);

            QMessageBox::information(this, tr("Image correction"), tr("Time of correction: %1 msec").arg(correctionTime), QMessageBox::Ok);
        }
        else
            QMessageBox::warning(this, tr("Image correction"), tr("Could not correct the image"), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(this, tr("Image correction"), tr("No image selected"), QMessageBox::Ok);
    }
}

void MainWindow::InformPriorityCombining()
{
    Combining(acv::ImageCombiner::CombineType::INFORM_PRIORITY);
}

void MainWindow::MorphologicalCombining()
{
    Combining(acv::ImageCombiner::CombineType::MORPHOLOGICAL);
}

void MainWindow::LocalEntropyCombining()
{
    Combining(acv::ImageCombiner::CombineType::LOCAL_ENTROPY);
}

void MainWindow::DifferencesAddingCombining()
{
    Combining(acv::ImageCombiner::CombineType::DIFFERENCES_ADDING);
}

void MainWindow::CalcDiff()
{
    Combining(acv::ImageCombiner::CombineType::CALC_DIFF);
}

void MainWindow::CalcEntropy()
{
    if (ImgWasSelected())
    {
       double entropy = acv::ImageParametersCalculator::CalcEntropy(GetCurImg());
       QMessageBox::information(this, tr("Entropy calculation"), tr("Entropy = %1").arg(entropy), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(this, tr("Entropy calculation"), tr("No image selected"), QMessageBox::Ok);
    }
}

void MainWindow::AdaptiveThreshold()
{
    if (ImgWasSelected())
    {
        const int DEFAULT_THRESHOLD = 7, MIN_THRESHOLD = 1, MAX_THRESHOLD  = 255, THRESHOLD_STEP = 1;
        const int DEFAULT_FILTER_SIZE = 3;

        int threshold = QInputDialog::getInt(this, tr("Enter the size if threshold"), tr("Filter size"),
                                             DEFAULT_THRESHOLD, MIN_THRESHOLD, MAX_THRESHOLD, THRESHOLD_STEP);

        acv::ImageFilter::ThresholdType type;
        if (QMessageBox::question(this, tr("Adaptive threshold"),
                                  "If yes then selected pixels will be equal to MAX value. To MIN value in other case") == QMessageBox::Yes)
            type = acv::ImageFilter::ThresholdType::MAX_MORE_THRESHOLD;
        else
            type = acv::ImageFilter::ThresholdType::MIN_MORE_THRESHOLD;

        const acv::Image& curImg = GetCurImg();
        acv::Image processedImg(curImg.GetHeight(), curImg.GetWidth());

        QElapsedTimer timer;
        timer.start();

        bool res = acv::ImageFilter::AdaptiveThreshold(curImg, processedImg, DEFAULT_FILTER_SIZE, threshold, type);

        qint64 time = timer.elapsed();

        if (res)
        {
            QString actionName = FormAdaptiveThresholdActionName(type, threshold);
            AddProcessedImg(processedImg, actionName);

            QMessageBox::information(this, tr("Adaptive threshold"), tr("Time of calculation: %1 msec").arg(time), QMessageBox::Ok);
        }
        else
            QMessageBox::warning(this, tr("Adaptive threshold"), tr("Could not calculate an adaptive threshold"), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(this, tr("Adaptive threshold"), tr("No image selected"), QMessageBox::Ok);
    }
}

void MainWindow::CalcAverageBrightness()
{
    if (ImgWasSelected())
    {
        double averBrig = acv::ImageParametersCalculator::CalcAverageBrightness(GetCurImg());
        QMessageBox::information(this, tr("Average brightness calculation"), tr("Average brightness = %1").arg(averBrig), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(this, tr("Average brightness calculation"), tr("No image selected"), QMessageBox::Ok);
    }
}

void MainWindow::CalcStandardDeviation()
{
    if (ImgWasSelected())
    {
        double averBrig = acv::ImageParametersCalculator::CalcAverageBrightness(GetCurImg());
        double sd = acv::ImageParametersCalculator::CalcStandardDeviation(GetCurImg(), averBrig);

        QMessageBox::information(this, tr("Standard deviation"), tr("Standard deviation = %1").arg(sd), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(this, tr("Standard deviation"), tr("No image selected"), QMessageBox::Ok);
    }
}

void MainWindow::CalcIntegralQualityIndicator()
{
    if (ImgWasSelected())
    {
        double iqi = acv::ImageParametersCalculator::CalcIntegralQualityIndicator(GetCurImg());
        QMessageBox::information(this, tr("Integral quality indicator"), tr("Standard deviation = %1").arg(iqi), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(this, tr("Integral quality indicator"), tr("No image selected"), QMessageBox::Ok);
    }
}

void MainWindow::CalcMinMaxBrightness()
{
    if (ImgWasSelected())
    {
        acv::Image::Byte minBr, maxBr;
        acv::ImageParametersCalculator::CalcMinMaxBrightness(GetCurImg(), minBr, maxBr);
        QMessageBox::information(this, tr("Minimum and maximum brightness calculation"), tr("Minimum = %1, maximum = %2").arg(minBr).arg(maxBr), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(this, tr("Minimum and maximum brightness calculation"), tr("No image selected"), QMessageBox::Ok);
    }
}

void MainWindow::CreateBrightnessHistogram()
{
    if (ImgWasSelected())
    {
        std::vector<double> brightnessHistogram(acv::Image::MAX_PIXEL_VALUE + 1), valuesOfBrightness(acv::Image::MAX_PIXEL_VALUE + 1);
        for(int i = acv::Image::MIN_PIXEL_VALUE; i < acv::Image::MAX_PIXEL_VALUE+1; i++)
            valuesOfBrightness[i] = i;
        acv::ImageParametersCalculator::CreateBrightnessHistogram(GetCurImg(), brightnessHistogram);
        setCentralWidget(mHist);
        mHist->DrawHist(brightnessHistogram,valuesOfBrightness);
    }
    else
    {
        QMessageBox::warning(this, tr("Creating brightness histogram"), tr("No image selected"), QMessageBox::Ok);
    }
}

QString MainWindow::FormHuMomentsStr(const acv::HuMoments& moments, int xMin, int xMax, int yMin, int yMax)
{
    QString resStr = tr("X: %1..%2 Y: %3..%4").arg(xMin).arg(xMax).arg(yMin).arg(yMax);

    for (size_t i = 0; i < moments.size(); ++i)
        resStr += tr("\nHu[%1] = %2").arg(i + 1).arg(moments[i]);
    return resStr;
}

void MainWindow::SetMouseMode(MouseMode mouseMode)
{
    switch (mouseMode)
    {
    case MouseMode::SELECT_PIXEL:
        mMouseMode = MouseMode::SELECT_PIXEL;

        setMouseTracking(true);
        mViewer->setMouseTracking(true);

        break;
    case MouseMode::SELECT_BOUNDARY:
        mMouseMode = MouseMode::SELECT_BOUNDARY;

        setMouseTracking(false);
        mViewer->setMouseTracking(false);

        emit UpdateCoordsAndBrigInStatusBar(-1, -1);

        break;
    }
}

void MainWindow::CalcHuMomentsStart()
{
    if (ImgWasSelected())
    {
        QMessageBox::information(this, tr("Hu's moments"),
                                 tr("You should select an area of pixels. "
                                    "Hu's moments will be calculated in this area."),
                                 QMessageBox::Ok);

        SetMouseMode(MouseMode::SELECT_BOUNDARY);
    }
    else
    {
        QMessageBox::warning(this, tr("Hu's moments"), tr("No image selected"), QMessageBox::Ok);
    }
}

void MainWindow::CalcHuMomentsExecute(int xMin, int xMax, int yMin, int yMax)
{
    if (ImgWasSelected())
    {
        const acv::Image& curImg = GetCurImg();
        acv::HuMomentsCalculator calculator(curImg, xMin, yMin, xMax, yMax);
        const acv::HuMoments& moments = calculator.GetHuMoments();

        QMessageBox::information(this, tr("Hu's moments"),
                                 FormHuMomentsStr(moments, xMin, xMax, yMin, yMax),
                                 QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(this, tr("Hu's moments"), tr("No image selected"), QMessageBox::Ok);
    }

    SetMouseMode(MouseMode::SELECT_PIXEL);
}

void MainWindow::ClearDisplay()
{
    mViewer->Clear();
}

void MainWindow::DisplayCoordsAndBrigInStatusBar(int x, int y)
{
    QString statusBarStr;

    if (ImgWasSelected())
    {
        const acv::Image& curImg = GetCurImg();
        if (!curImg.IsInvalidCoordinates(y, x))
            statusBarStr = tr("X=%1 Y=%2 B=%3").arg(x).arg(y).arg(curImg.GetPixel(y, x));
    }

    statusBarLabel->setText(statusBarStr);
}

QString MainWindow::FormCombinationResultStr(acv::CombinationResult combRes)
{
    switch (combRes)
    {
    case acv::CombinationResult::INCORRECT_COMBINER_TYPE:
        return tr("Unknown combiner type");
    case acv::CombinationResult::FEW_IMAGES:
        return tr("The number of images should be >= 2");
    case acv::CombinationResult::NOT_SAME_IMAGES:
        return tr("The combined images are not the same");
    case acv::CombinationResult::MANY_IMAGES:
        return tr("The number of images should be 2");
    case acv::CombinationResult::SUCCESS:
        return tr("Success of combining");
    default:
        return tr("Unknown error");
    }
}

void MainWindow::Combining(acv::ImageCombiner::CombineType combType)
{
    if (ImgsWereOpened())
    {
        int answer = QMessageBox::No;
        // There is not difference which image is base in case of local-entropy combining
        if (combType != acv::ImageCombiner::CombineType::LOCAL_ENTROPY)
        {
            answer = QMessageBox::question(this, "Images combining",
                                           "Do you want select the base image (with maximum of entropy)? "
                                           "If not then the basic will be first image!");

            // This is the case when the dialog was closed
            if (answer != QMessageBox::Yes && answer != QMessageBox::No)
                return;
        }

        acv::ImageCombiner combiner;
        for (const auto& img : mOpenedImgs)
            combiner.AddImage(img);

        acv::Image combImg(mOpenedImgs[0].GetHeight(), mOpenedImgs[0].GetWidth());

        QElapsedTimer timer;
        timer.start();

        acv::CombinationResult combRes = combiner.Combine(combType, combImg, answer == QMessageBox::Yes);

        qint64 combTime = timer.elapsed();

        if (combRes == acv::CombinationResult::SUCCESS && combImg.IsInitialized())
        {
            QString actionName = FormCombineActionName(combType);
            AddProcessedImg(combImg, actionName);

            QMessageBox::information(this, tr("Images combining"), tr("Time of combining: %1 мсек").arg(combTime), QMessageBox::Ok);
        }
        else
            QMessageBox::warning(this, tr("Images combining"), FormCombinationResultStr(combRes), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(this, tr("Images combining"), tr("No images were opened"), QMessageBox::Ok);
    }
}

void MainWindow::AddOpenedImgAction(const QString& fileName)
{
    QString actionName = QFileInfo(fileName).fileName();

    QAction* act = new QAction(actionName, this);
    act->setCheckable(true);
    connect(act, SIGNAL(triggered()), this, SLOT(SetCurImage()));

    mOpenedImgsActions.push_back(act);
}

QString MainWindow::FormCombineActionName(acv::ImageCombiner::CombineType combType)
{
    QString ret;

    switch (combType)
    {
    case acv::ImageCombiner::CombineType::INFORM_PRIORITY:
        ret = tr("C_IP: ");
        break;
    case acv::ImageCombiner::CombineType::MORPHOLOGICAL:
        ret = tr("C_M: ");
        break;
    case acv::ImageCombiner::CombineType::LOCAL_ENTROPY:
        ret = tr("C_LE: ");
        break;
    case acv::ImageCombiner::CombineType::DIFFERENCES_ADDING:
        ret = tr("C_DA: ");
        break;
    case acv::ImageCombiner::CombineType::CALC_DIFF:
        ret = tr("C_CD: ");
        break;
    }

    for (size_t i = 0; i < mOpenedImgsActions.size(); ++i)
    {
        ret += mOpenedImgsActions[i]->text();
        if (i < (mOpenedImgsActions.size() - 1))
            ret += tr("+");
    }

    return ret;
}

QString MainWindow::FormFilterActionName(acv::ImageFilter::FilterType filterType, const int filterSize)
{
    QString ret;

    switch (filterType)
    {
    case acv::ImageFilter::FilterType::MEDIAN:
        ret = tr("F_M_%1: ").arg(filterSize);
        break;
    case acv::ImageFilter::FilterType::GAUSSIAN:
        ret = tr("F_G_%1: ").arg(filterSize);
        break;
    case acv::ImageFilter::FilterType::SEP_GAUSSIAN:
        ret = tr("F_G_SEP_%1: ").arg(filterSize);
        break;
    case acv::ImageFilter::FilterType::IIR_GAUSSIAN:
        ret = tr("F_G_IIR_%1: ").arg(filterSize);
        break;
    case acv::ImageFilter::FilterType::SHARPEN:
        ret = tr("F_SHARP: ");
        break;
    default:
        return QString();
    }

    ret = FormProcessedImgActionName(ret);
    return ret;
}

QString MainWindow::FormAdaptiveThresholdActionName(acv::ImageFilter::ThresholdType thresholdType, const int threshold)
{
    QString ret;

    switch (thresholdType)
    {
    case acv::ImageFilter::ThresholdType::MAX_MORE_THRESHOLD:
        ret = tr("AT_MAX_%1: ").arg(threshold);
        break;
    case acv::ImageFilter::ThresholdType::MIN_MORE_THRESHOLD:
        ret = tr("AT_MIN_%1: ").arg(threshold);
        break;
    default:
        return QString();
    }

    ret = FormProcessedImgActionName(ret);
    return ret;
}

QString MainWindow::FormCorrectorActionName(acv::ImageCorrector::CorrectorType corType)
{
    QString ret;

    switch (corType)
    {
    case acv::ImageCorrector::CorrectorType::SSRETINEX:
        ret = tr("CORR_SSR: ");
        break;
    case acv::ImageCorrector::CorrectorType::AUTO_LEVELS:
        ret = tr("CORR_AL: ");
        break;
    case acv::ImageCorrector::CorrectorType::NORM_AUTO_LEVELS:
        ret = tr("CORR_NAL: ");
        break;
    case acv::ImageCorrector::CorrectorType::GAMMA:
        ret = tr("CORR_GAM: ");
        break;
    default:
        return QString();
    }

    ret = FormProcessedImgActionName(ret);
    return ret;
}

QString MainWindow::FormOperatorActionName(acv::BordersDetector::DetectorType operatorType, acv::BordersDetector::OperatorTypes type)
{
    QString ret;

    QString dirStr = (type == acv::BordersDetector::OperatorTypes::VERTICAL) ? tr("V") : tr("H");

    switch (operatorType)
    {
    case acv::BordersDetector::DetectorType::SOBEL:
        ret = tr("O_SOB_%1: ").arg(dirStr);
        break;
    case acv::BordersDetector::DetectorType::SCHARR:
        ret = tr("O_SCH_%1: ").arg(dirStr);
        break;
    default:
        return QString();
    }

    ret = FormProcessedImgActionName(ret);
    return ret;
}

QString MainWindow::FormBordersDetectorActionName(acv::BordersDetector::DetectorType detectorType)
{
    QString ret;

    switch (detectorType)
    {
    case acv::BordersDetector::DetectorType::SOBEL:
        ret = tr("BD_SOB: ");
        break;
    case acv::BordersDetector::DetectorType::SCHARR:
        ret = tr("BD_SCH: ");
        break;
    case acv::BordersDetector::DetectorType::CANNY:
        ret = tr("BD_C: ");
        break;
    default:
        return QString();
    }

    ret = FormProcessedImgActionName(ret);
    return ret;
}

QString MainWindow::FormProcessedImgActionName(const QString& prefix)
{
    QString ret = prefix;

    if (mCurOpenedImg != -1)
        ret += mOpenedImgsActions[mCurOpenedImg]->text();
    else if (mCurProcessedImg != -1)
        ret += mProcessedImgsActions[mCurProcessedImg]->text();
    else
        ret.clear();

    return ret;
}

void MainWindow::AddProcessedImgAction(const QString& actionName)
{
    QAction* act = new QAction(actionName, this);
    act->setCheckable(true);
    connect(act, SIGNAL(triggered()), this, SLOT(SetCurImage()));

    mProcessedImgsActions.push_back(act);
}

bool MainWindow::ImgWasSelected() const
{
    return (mCurOpenedImg != -1 || mCurProcessedImg != -1);
}

bool MainWindow::ImgsWereOpened() const
{
    return !mOpenedImgs.empty();
}

void MainWindow::OpenImgFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open image"), ".",
                                                    tr("Image files (*.bmp)"));
    if (!fileName.isEmpty())
    {
        if (LoadImgFromFile(fileName))
        {
            mCurOpenedImg = mOpenedImgs.size() - 1;
            mCurProcessedImg = -1;
            emit CurImgWasUpdated();

            AddOpenedImgAction(fileName);
            emit OpenedImgsChanged();
        }
    }
}

void MainWindow::DeleteImg(int& curImg, std::vector<acv::Image>& imgs, std::vector<QAction*>& actions)
{
    // Delete the current image from vector
    imgs.erase(imgs.begin() + curImg);

    // Delete the action for current image
    delete actions[curImg];
    actions.erase(actions.begin() + curImg);

    // Select the new current image if we can
    if (!imgs.empty())
    {
        if (curImg > 0)
            --curImg;
    }
    else
        curImg = -1;
}

void MainWindow::CloseImgFile()
{
    if (ImgWasSelected())
    {
       if (mCurOpenedImg != -1)
       {
           DeleteImg(mCurOpenedImg, mOpenedImgs, mOpenedImgsActions);

           // In case of deleting all opened images the first processed image will be current
           if (mCurOpenedImg == -1 && !mProcessedImgs.empty())
               mCurProcessedImg = 0;
       }
       else if (mCurProcessedImg != -1)
       {
           mProcImgsSavingFlags.erase(mProcImgsSavingFlags.begin() + mCurProcessedImg);
           DeleteImg(mCurProcessedImg, mProcessedImgs, mProcessedImgsActions);

           // In case of deleting all processed images the first opened image will be current
           if (mCurProcessedImg == -1 && !mOpenedImgs.empty())
               mCurOpenedImg = 0;
       }

       if (ImgWasSelected())
       {
           emit CurImgWasUpdated();
           emit OpenedImgsChanged(); // This is equivalent to call a signal ProcessedImgsChanged (no matter which image is current)
       }
       else
       {
           emit AllImgsAreClosed();
       }
    }
    else
    {
        QMessageBox::warning(this, tr("Close image"), tr("No image selected"), QMessageBox::Ok);
    }
}

void MainWindow::SaveImgFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save image"), ".",
                                                    tr("Image files (*.bmp)"));
    if (!fileName.isEmpty())
    {
        QImage img = ImageTransormer::AImage2QImage(GetCurImg());
        if (!img.save(fileName))
            QMessageBox::warning(this, tr("Save image"), tr("Could not save image"), QMessageBox::Ok);
        else
        {
            if (mCurProcessedImg != -1)
                mProcImgsSavingFlags[mCurProcessedImg] = true;
        }
    }
}

bool MainWindow::HaveNotSavedImages() const
{
    for (auto flag : mProcImgsSavingFlags)
        if (flag == false)
            return true;
    return false;
}

void MainWindow::Exit()
{
    if (HaveNotSavedImages())
        if(QMessageBox::question(this, tr("Exit from application"), "Not saved data will be lose. Are you sure?") == QMessageBox::No)
            return;
    close();
}

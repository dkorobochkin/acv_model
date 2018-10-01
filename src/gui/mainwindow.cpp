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
#include "imagetransformer.h"

#include <QAction>
#include <QMenu>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QIcon>
#include <QMessageBox>
#include <QInputDialog>
#include <QElapsedTimer>

#include <exception>

#include "ImageParametersCalculator.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    mUi(new Ui::MainWindow),
    mViewer(new ImageViewer),
    mCurOpenedImg(-1),
    mCurProcessedImg(-1)
{
    mUi->setupUi(this);
    setCentralWidget(mViewer);

    CreateActions();
    CreateMainMenus();

    connect(this, SIGNAL(CurImgWasUpdated()), this, SLOT(DrawCurImg()));
    connect(this, SIGNAL(CurImgWasUpdated()), this, SLOT(MoveToCenterOfDisplay()));
    connect(this, SIGNAL(CurImgWasUpdated()), this, SLOT(UpdateImgsMenu()));
    connect(this, SIGNAL(OpenedImgsChanged()), this, SLOT(UpdateImgsMenu()));
    connect(this, SIGNAL(ProcessedImgsChanged()), this, SLOT(UpdateImgsMenu()));
    connect(this, SIGNAL(AllImgsAreClosed()), this, SLOT(ClearDisplay()));
    connect(this, SIGNAL(AllImgsAreClosed()), this, SLOT(UpdateImgsMenu()));

    move((qApp->desktop()->width() - width()) / 2, (qApp->desktop()->height() - height()) / 2);
}

MainWindow::~MainWindow()
{
    delete mUi;
    delete mViewer;
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
}

void MainWindow::CreateParamsMenu()
{
    mImgParams = mProcessingMenu->addMenu(tr("Parameters"));

    mImgParams->addAction(mImgEntropyAction);
    mImgParams->addAction(mImgAverBrightnessAction);
    mImgParams->addAction(mImgStdDeviationAction);
    mImgParams->addAction(mImgMinMaxBrightnessAction);
}

void MainWindow::CreateCombiningMenu()
{
    mCombineMenu = menuBar()->addMenu(tr("Combination"));

    mCombineMenu->addAction(mInfPriorCombAction);
    mCombineMenu->addAction(mMorphCombAction);
    mCombineMenu->addAction(mLocEntrCombAction);
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

void MainWindow::Operator(acv::BordersDetector::OperatorType operatorType)
{
    if (ImgWasSelected())
    {
        acv::BordersDetector::SobelTypes type;
        if (QMessageBox::question(this, tr("Operator"), "Need the horizontal operator? Else will be vertical.") == QMessageBox::Yes)
            type = acv::BordersDetector::SobelTypes::HORIZONTAL;
        else
            type = acv::BordersDetector::SobelTypes::VERTICAL;

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
    Operator(acv::BordersDetector::OperatorType::SOBEL);
}

void MainWindow::Scharr()
{
    Operator(acv::BordersDetector::OperatorType::SCHARR);
}

void MainWindow::AddProcessedImg(const acv::Image& processedImg, const QString& actionName)
{
    mProcessedImgs.push_back(processedImg);

    mCurOpenedImg = -1;
    mCurProcessedImg = mProcessedImgs.size() - 1;
    emit CurImgWasUpdated();

    AddProcessedImgAction(actionName);
    emit ProcessedImgsChanged();
}

void MainWindow::Canny()
{
    if (ImgWasSelected())
    {
        const int CANNY_THRESHOLD_1 = 20, CANNY_THRESHOLD_2 = 90;

        const acv::Image& curImg = GetCurImg();
        acv::Image processedImg(curImg.GetHeight(), curImg.GetWidth());

        QElapsedTimer timer;
        timer.start();

        bool detected = acv::BordersDetector::Canny(curImg, processedImg, CANNY_THRESHOLD_1, CANNY_THRESHOLD_2);

        qint64 filterTime = timer.elapsed();

        if (detected)
        {
            QString actionName = FormCannyActionName();
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

void MainWindow::ClearDisplay()
{
    mViewer->Clear();
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

QString MainWindow::FormOperatorActionName(acv::BordersDetector::OperatorType operatorType, acv::BordersDetector::SobelTypes type)
{
    QString ret;

    QString dirStr = (type == acv::BordersDetector::SobelTypes::VERTICAL) ? tr("V") : tr("H");

    switch (operatorType)
    {
    case acv::BordersDetector::OperatorType::SOBEL:
        ret = tr("O_SOB_%1: ").arg(dirStr);
        break;
    case acv::BordersDetector::OperatorType::SCHARR:
        ret = tr("O_SCH_%1: ").arg(dirStr);
        break;
    default:
        return QString();
    }

    ret = FormProcessedImgActionName(ret);
    return ret;
}

QString MainWindow::FormCannyActionName()
{
    QString ret = tr("BD_C: ");
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
    }
}

void MainWindow::Exit()
{
    if (!mProcessedImgs.empty())
        if(QMessageBox::question(this, tr("Exit from application"), "Not saved data will be lose. Are you sure?") == QMessageBox::No)
            return;
    close();
}

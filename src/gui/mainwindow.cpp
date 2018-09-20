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
        // Такого не должно быть никогда!!!
        throw std::out_of_range("Не выбрано ни одного изображения");
}

const acv::Image& MainWindow::GetCurImg() const
{
    if (mCurOpenedImg != -1)
        return mOpenedImgs[mCurOpenedImg];
    else if (mCurProcessedImg != -1)
        return mProcessedImgs[mCurProcessedImg];
    else
        // Такого никогда не должно быть!!!
        throw std::out_of_range("Не выбрано ни одного изображения");
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
            mProcessedImgs.push_back(processedImg);

            QString actionName = FormOperatorActionName(operatorType, type);

            mCurOpenedImg = -1;
            mCurProcessedImg = mProcessedImgs.size() - 1;
            emit CurImgWasUpdated();

            AddProcessedImgAction(actionName);
            emit ProcessedImgsChanged();

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

void MainWindow::Canny()
{
    if (ImgWasSelected())
    {
        const int CANNY_THRESHOLD_1 = 20, CANNY_THRESHOLD_2 = 90;

        QElapsedTimer timer;
        timer.start();

        acv::Image& curImg = GetCurImg();
        bool detected = acv::BordersDetector::Canny(curImg, CANNY_THRESHOLD_1, CANNY_THRESHOLD_2);

        qint64 filterTime = timer.elapsed();

        if (detected)
        {
            emit CurImgWasUpdated();

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

void MainWindow::Filtering(acv::ImageFilter::FilterType filterType)
{
    if (ImgWasSelected())
    {
        const int DEFAULT_FILTER_SIZE = 3, MIN_FILTER_SIZE = 1, MAX_FILTER_SIZE = 15, FILTER_SIZE_STEP = 2;
        int filterSize = QInputDialog::getInt(this, tr("Enter the filter size (odd positive number)"), tr("Filter size"),
                                              DEFAULT_FILTER_SIZE, MIN_FILTER_SIZE, MAX_FILTER_SIZE, FILTER_SIZE_STEP);

        const acv::Image& curImg = GetCurImg();
        acv::Image processedImg(curImg.GetHeight(), curImg.GetWidth());

        QElapsedTimer timer;
        timer.start();

        bool filtred = acv::ImageFilter::Filter(curImg, processedImg, filterSize, filterType);

        qint64 filterTime = timer.elapsed();

        if (filtred)
        {
            mProcessedImgs.push_back(processedImg);

            QString actionName = FormFilterActionName(filterType, filterSize);

            mCurOpenedImg = -1;
            mCurProcessedImg = mProcessedImgs.size() - 1;
            emit CurImgWasUpdated();

            AddProcessedImgAction(actionName);
            emit ProcessedImgsChanged();

            QMessageBox::information(this, tr("Image filtration"), tr("Time of filtration: %1 msec").arg(filterTime), QMessageBox::Ok);
        }
        else
            QMessageBox::warning(this, tr("Image filtration"), tr("Could not filtration"), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(this, tr("Image filtration"), tr("No image selected"), QMessageBox::Ok);
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
       QMessageBox::information(this, tr("Расчет энтропии"), tr("Энтропия = %1").arg(entropy), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(this, tr("Расчет энтропии"), tr("Изображение не выбрано"), QMessageBox::Ok);
    }
}

void MainWindow::CalcAverageBrightness()
{
    if (ImgWasSelected())
    {
       double averBrig = acv::ImageParametersCalculator::CalcAverageBrightness(GetCurImg());
       QMessageBox::information(this, tr("Расчет средней яркости"), tr("Средняя яркость = %1").arg(averBrig), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(this, tr("Расчет средней яркости"), tr("Изображение не выбрано"), QMessageBox::Ok);
    }
}

void MainWindow::ClearDisplay()
{
    mViewer->Clear();
}

void MainWindow::Combining(acv::ImageCombiner::CombineType combType)
{
    if (ImgsWereOpened())
    {
        int answer = QMessageBox::No;
        // При локально-энтропийном комплексировании нет разницы какое изображение является базовым
        if (combType != acv::ImageCombiner::CombineType::LOCAL_ENTROPY)
        {
            answer = QMessageBox::question(this, "Комбинирование изображений",
                                           "Необходимо ли выбрать базовым наиболее информативное (оценкой энтропии)? "
                                           "Если нет, то базовым будет считаться первое открытое изображение!");

            // Закрыли окно с вопросом с помощью крестика - не проводим комбинирование
            if (answer != QMessageBox::Yes && answer != QMessageBox::No)
                return;
        }

        acv::ImageCombiner combiner;
        for (const auto& img : mOpenedImgs)
            combiner.AddImage(img);

        QElapsedTimer timer;
        timer.start();

        bool combined;
        acv::Image combImg = combiner.Combine(combType, combined, answer == QMessageBox::Yes);

        qint64 combTime = timer.elapsed();

        if (combined && combImg.IsInitialized())
        {
            mProcessedImgs.push_back(combImg);

            mCurOpenedImg = -1;
            mCurProcessedImg = mProcessedImgs.size() - 1;
            emit CurImgWasUpdated();

            AddProcessedImgAction(FormCombineActionName(combType));
            emit ProcessedImgsChanged();

            QMessageBox::information(this, tr("Совмещение изображений"), tr("Время комбинирования: %1 мсек").arg(combTime), QMessageBox::Ok);
        }
        else
            QMessageBox::warning(this, tr("Совмещение изображений"), tr("Не удалось выполнить совмещение"), QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(this, tr("Совмещение изображений"), tr("Не загружено ни одного изображения"), QMessageBox::Ok);
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
        ret = tr("К_ПНИ: ");
        break;
    case acv::ImageCombiner::CombineType::MORPHOLOGICAL:
        ret = tr("К_М: ");
        break;
    case acv::ImageCombiner::CombineType::LOCAL_ENTROPY:
        ret = tr("К_ЛЭ: ");
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
    QString fileName = QFileDialog::getOpenFileName(this, tr("Открыть изображение"), ".",
                                                    tr("Файлы изображений (*.bmp)"));
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
    // Удаляем из массива текущее изображение
    imgs.erase(imgs.begin() + curImg);

    // Удаляем действие для данного изображения
    delete actions[curImg];
    actions.erase(actions.begin() + curImg);

    // Выбираем новое изображение, если это возможно
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

           // Если удалили последнее открытое изображение, то текущим ставим первое обработанное, если оно есть
           if (mCurOpenedImg == -1 && !mProcessedImgs.empty())
               mCurProcessedImg = 0;
       }
       else if (mCurProcessedImg != -1)
       {
           DeleteImg(mCurProcessedImg, mProcessedImgs, mProcessedImgsActions);

           // Если удалили последнее обработанное изображение, то текущим ставим первое открытое, если оно есть
           if (mCurProcessedImg == -1 && !mOpenedImgs.empty())
               mCurOpenedImg = 0;
       }

       if (ImgWasSelected())
       {
           emit CurImgWasUpdated();
           emit OpenedImgsChanged(); // Равноценно вызову сигнала ProcessedImgsChanged (не важно какое изображение стало текущим)
       }
       else
       {
           emit AllImgsAreClosed();
       }
    }
    else
    {
        QMessageBox::warning(this, tr("Закрытие изображения"), tr("Изображение не выбрано"), QMessageBox::Ok);
    }
}

void MainWindow::SaveImgFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить изображение"), ".",
                                                    tr("Файлы изображений (*.bmp)"));
    if (!fileName.isEmpty())
    {
        QImage img = ImageTransormer::AImage2QImage(GetCurImg());
        if (!img.save(fileName))
            QMessageBox::warning(this, tr("Сохранение изображения"), tr("Не удалось выполнить сохранение"), QMessageBox::Ok);
    }
}

void MainWindow::Exit()
{
    if (!mProcessedImgs.empty())
        if(QMessageBox::question(this, tr("Выход из приложения"), "Все несохраненные данные будут потеряны. Действительно выйти?") == QMessageBox::No)
            return;
    close();
}

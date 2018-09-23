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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <vector>

#include "Image.h"
#include "ImageFilter.h"
#include "ImageCombiner.h"
#include "BordersDetector.h"

namespace Ui {
class MainWindow;
}

class QAction;
class QMenu;
class QImage;
class ImageViewer;

class MainWindow : public QMainWindow
{

    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

private slots: // Private slots

    // Slot to open the image
    void OpenImgFile();

    // Slot to close the image (close the current image)
    void CloseImgFile();

    // Slot to save the image (current selected image)
    void SaveImgFile();

    // Slot to close the main window
    void Exit();

    // Slot to draw the current selected image
    void DrawCurImg();

    // Slot to place the main window on center of screen
    void MoveToCenterOfDisplay();

    // Slot to update the menu of images
    // The slot will be called in cases of add or close images
    void UpdateImgsMenu();

    // Slot to set current image by selected image in menu
    void SetCurImage();

    // Slot to run the median blur
    void MedianBlur();

    // Slot to run the Gaussian blur
    void GaussianBlur();

    // IIR-imitated gaussian blur
    void IIRGaussianBlur();

    // Slot to convolution the image with the Sobel operator
    void Sobel();

    // Slot to convolution the image with the Scharr operator
    void Scharr();

    // Slot to detect the borders by using Canny algorithm
    void Canny();

    // Slot to combine the images by method of priority of the most informative image
    void InformPriorityCombining();

    // Slot to combine the images by morphological method
    void MorphologicalCombining();

    // Slot to combine the images by local-entropy method
    void LocalEntropyCombining();

    // Slot to calculation of image entropy
    void CalcEntropy();

    // Slot to calculation of image average brightness
    void CalcAverageBrightness();

    // Slot to clear the display (the slot will be called in case of all images closing)
    void ClearDisplay();

signals: // Signals

    // Signal about select the other image
    void CurImgWasUpdated();

    // Signal about change the vector of opened images
    void OpenedImgsChanged();

    // Signal about change the vector of processed images
    void ProcessedImgsChanged();

    // Signal about close all images
    void AllImgsAreClosed();

private: // Private methods

    // Creation the actions for section "File"
    void CreateFileActions();

    // Creation the actions for section "Filtration"
    void CreateFilteringActions();

    // Creation the actions for section "Operators"
    void CreateOperatorActions();

    // Creation the actions for section "Detectors of borders"
    void CreateBodersDetectorsActions();

    // Creation the actions for section "Parameters"
    void CreateParamsActions();

    // Creation the actions for section "Combining"
    void CreateCombiningActions();

    // Creation all actions
    void CreateActions();

    // Creation menu for section "File"
    void CreateFileMenu();

    // Creation menu for section "Filtration"
    void CreateFilteringMenu();

    // Creation menu for section "Operators
    void CreateOperatorsMenu();

    // Creation menu for section "Detectors of borders"
    void CreateBordersDetectorsMenu();

    // Creation menu for section "Parameters
    void CreateParamsMenu();

    // Creation menu for section "Combining
    void CreateCombiningMenu();

    // Creation main menu
    void CreateMainMenus();

    // Load image from file
    bool LoadImgFromFile(const QString& fileName);

    // Adding the action for new open file
    void AddOpenedImgAction(const QString& fileName);

    // Forming the name for action of combined image
    QString FormCombineActionName(acv::ImageCombiner::CombineType combType);

    // Forming the name for action of processed image
    QString FormProcessedImgActionName(const QString& prefix);

    // Forming the name for action of filtered image
    QString FormFilterActionName(acv::ImageFilter::FilterType filterType, const int filterSize);

    // Forming the name for action of image that get in result of convolution with operator
    QString FormOperatorActionName(acv::BordersDetector::OperatorType operatorType, acv::BordersDetector::SobelTypes type);

    // Adding the action for processed image
    void AddProcessedImgAction(const QString& actionName);

    // Check if has been opened image
    bool ImgsWereOpened() const;

    // Check if has been selected image
    bool ImgWasSelected() const;

    // Combining with the specified type
    void Combining(acv::ImageCombiner::CombineType combType);

    // Filtering with the specified type
    void Filtering(acv::ImageFilter::FilterType filterType);

    // Convolution the image with the operator of specified type
    void Operator(acv::BordersDetector::OperatorType operatorType);

    // Get the number of selected action from actions vector (if was not selected then will be return -1)
    int GetNumOfActionInVec(const QAction* action, const std::vector<QAction*>& actionsVec);

    // Get the number of selected opened image (if was not selected then will be return -1)
    int GetNumOfOpenedImgByAction(const QAction* actions);

    // Get the number of selected processed image (if was not selected then will be return -1)
    int GetNumOfProcessedImgByAction(const QAction* action);

    // Get the reference to the current selected image
    acv::Image& GetCurImg();
    const acv::Image& GetCurImg() const;

    // Удаляет изображение из массива изображений и удаляет действие для него, так же изменяет номер изображения
    // Delete the current image and him action. Will be change the number of current image
    void DeleteImg(int& curImg, std::vector<acv::Image>& imgs, std::vector<QAction*>& actions);

private: // Private members

    // UI
    Ui::MainWindow *mUi;

    // Actions to work with images
    std::vector<QAction*> mOpenedImgsActions;
    std::vector<QAction*> mProcessedImgsActions;
    QAction* mOpenAction;
    QAction* mCloseAction;
    QAction* mSaveAction;
    QAction* mExitAction;
    QAction* mMedianBlurAction;
    QAction* mGaussianBlurAction;
    QAction* mIIRGaussianBlurAction;
    QAction* mSobelAction;
    QAction* mScharrAction;
    QAction* mCannyAction;
    QAction* mInfPriorCombAction;
    QAction* mMorphCombAction;
    QAction* mLocEntrCombAction;
    QAction* mImgEntropyAction;
    QAction* mImgAverBrightnessAction;

    // Main menu
    QMenu* mFileMenu;
    QMenu* mImgsMenu;
    QMenu* mProcessingMenu;
    QMenu* mFilterMenu;
    QMenu* mOperatorsMenu;
    QMenu* mBordersDetectorsMenu;
    QMenu* mImgParams;
    QMenu* mCombineMenu;
    QMenu* mCombineTestMenu;

    // This member is used to draw the image on the screen
    ImageViewer* mViewer;

    // All opened images
    std::vector<acv::Image> mOpenedImgs;

    // All processed images
    std::vector<acv::Image> mProcessedImgs;

    // Current selected opened image (-1 in case of not select)
    int mCurOpenedImg;

    // Current selected processed image (-1 in case of not select)
    int mCurProcessedImg;

};

#endif // MAINWINDOW_H

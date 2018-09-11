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

private slots: // Закрытые слоты

    // Слот для открытия изображения
    void OpenImgFile();

    // Слот для закрытия изображения (закрывает текущее выбранное изображение)
    void CloseImgFile();

    // Слот для сохранения изображения (текущего выбранного)
    void SaveImgFile();

    // Слот для закрытия главного окна
    void Exit();

    // Слот отображения текущего выбранного изображение
    void DrawCurImg();

    // Слот для расположения главного окна по центру экрана
    void MoveToCenterOfDisplay();

    // Слот для обновления меню открытых изображений
    // Вызывает после добавления или закрытия изображений, в том числе и обработанных
    void UpdateImgsMenu();

    // Установка текущего изображения по выбранному из меню
    void SetCurImage();

    // Выполнение медианной фильтрации
    void MedianBlur();

    // Выполнение Гауссовой фильтрации
    void GaussianBlur();

    // Свертка изображения с оператором Собеля
    void Sobel();

    // Свертка изображения с оператором Щарра
    void Scharr();

    // Совмещение с приоритетом наиболее информативного изображения
    void InformPriorityCombining();

    // Морфологическое совмещение
    void MorphologicalCombining();

    // Локально-энтропийное комбинирование
    void LocalEntropyCombining();

    // Расчет энтропии изображения
    void CalcEntropy();

    // Расчет средней яркости изображения
    void CalcAverageBrightness();

    // Очистка дисплея (вызывает когда закрыли все октрытые и обработанные изображения)
    void ClearDisplay();

signals: // Сигналы

    // Сигнал о том, что выбрано другое изображение
    void CurImgWasUpdated();

    // Сигнал о том, что изменился массив открытых изображений
    void OpenedImgsChanged();

    // Сигнал о том, что изменился массив обработанных изображений
    void ProcessedImgsChanged();

    // Сигнал о том, что все изображения закрыты
    void AllImgsAreClosed();

private: // Закрытые методы

    // Создание действий для раздела "Файл"
    void CreateFileActions();

    // Создание действий для раздела "Фильтрация"
    void CreateFilteringActions();

    // Создание действия для раздела "Операторы"
    void CreateOperatorActions();

    // Создание действий для раздела "Параметры"
    void CreateParamsActions();

    // Создание действий для раздела "Совмещение"
    void CreateCombiningActions();

    // Создание действий для работы с изображениями
    void CreateActions();

    // Создание меню для раздела "Файл"
    void CreateFileMenu();

    // Создание меню для раздела "Фильтрация"
    void CreateFilteringMenu();

    // Создание меню для раздела "Операторы"
    void CreateOperatorsMenu();

    // Создание меню для раздела "Параметры"
    void CreateParamsMenu();

    // Создание меню для раздела "Совмещение"
    void CreateCombiningMenu();

    // Создание главных меню
    void CreateMainMenus();

    // Загрузка изображения из файла
    bool LoadImgFromFile(const QString& fileName);

    // Добавление действия для вновь открытого изображения из файла
    void AddOpenedImgAction(const QString& fileName);

    // Формирование имени для действия комбинированного изображения
    QString FormCombineActionName(acv::ImageCombiner::CombineType combType);

    // Формирование имени для действия по обработке изображений
    QString FormProcessedImgActionName(const QString& prefix);

    // Добавление действия для обработанного изображения
    void AddProcessedImgAction(const QString& actionName);

    // Проверяет, было ли открыто хоть одно изображение
    bool ImgsWereOpened() const;

    // Проверка, выбрано ли изображение
    bool ImgWasSelected() const;

    // Совмещение с заданным типом
    void Combining(acv::ImageCombiner::CombineType combType);

    // Фильтрация заданного типа
    void Filtering(acv::ImageFilter::FilterType filterType);

    // Свертка с оператором заданного типа
    void Operator(acv::ImageFilter::OperatorType operatorType);

    // Получить номер выбранного действия из вектора действий (если не выбрано, то возвращает -1)
    int GetNumOfActionInVec(const QAction* action, const std::vector<QAction*>& actionsVec);

    // Получить номер выбранного открытого изображения (если изображение не выбрано, то возвращает -1)
    int GetNumOfOpenedImgByAction(const QAction* actions);

    // Получить номер выбранного обработанного изображения (если изображение не выбрано, то возвращает -1)
    int GetNumOfProcessedImgByAction(const QAction* action);

    // Возвращает ссылку на текущее выбранное изображение
    acv::Image& GetCurImg();
    const acv::Image& GetCurImg() const;

    // Удаляет изображение из массива изображений и удаляет действие для него, так же изменяет номер изображения
    void DeleteImg(int& curImg, std::vector<acv::Image>& imgs, std::vector<QAction*>& actions);

private: // Закрытые данные

    // Пользовательский интерфейс
    Ui::MainWindow *mUi;

    // Действия для работы с изображениями
    std::vector<QAction*> mOpenedImgsActions;
    std::vector<QAction*> mProcessedImgsActions;
    QAction* mOpenAction;
    QAction* mCloseAction;
    QAction* mSaveAction;
    QAction* mExitAction;
    QAction* mMedianBlurAction;
    QAction* mGaussianBlurAction;
    QAction* mSobelAction;
    QAction* mScharrAction;
    QAction* mInfPriorCombAction;
    QAction* mMorphCombAction;
    QAction* mLocEntrCombAction;
    QAction* mImgEntropyAction;
    QAction* mImgAverBrightnessAction;

    // Главные меню
    QMenu* mFileMenu;
    QMenu* mImgsMenu;
    QMenu* mProcessingMenu;
    QMenu* mFilterMenu;
    QMenu* mOperatorsMenu;
    QMenu* mImgParams;
    QMenu* mCombineMenu;
    QMenu* mCombineTestMenu;

    // Используется для отображения изображения на экране
    ImageViewer* mViewer;

    // Хранит все открытые изображения
    std::vector<acv::Image> mOpenedImgs;

    // Хранит все обработанные изображения
    std::vector<acv::Image> mProcessedImgs;

    // Номер текущего выбранного открытого изображения (-1 если не выбрано)
    int mCurOpenedImg;

    // Номер текущего выбранного обработанного (например, результат совмещения) изображения (-1 если не выбрано)
    int mCurProcessedImg;

};

#endif // MAINWINDOW_H

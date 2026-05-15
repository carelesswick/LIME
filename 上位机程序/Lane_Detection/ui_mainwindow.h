/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QWidget>
#include "qchartview.h"
#include "qvideowidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *Open;
    QAction *Stop;
    QAction *Select;
    QAction *Choose;
    QAction *actionclose;
    QAction *Files;
    QAction *Video;
    QWidget *centralWidget;
    QWidget *widget_2;
    QVideoWidget *videowidget;
    QTextBrowser *info_box;
    QChartView *graphicsView;
    QLabel *cameraView;
    QLabel *label;
    QLabel *label_2;
    QLabel *resultView;
    QTextBrowser *textBrowser;
    QListView *listView;
    QPushButton *result;
    QPushButton *yolop_process;
    QLabel *label_3;
    QTreeView *treeView;
    QMenuBar *menuBar;
    QMenu *menu;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1535, 796);
        MainWindow->setStyleSheet(QString::fromUtf8(""));
        Open = new QAction(MainWindow);
        Open->setObjectName(QString::fromUtf8("Open"));
        Stop = new QAction(MainWindow);
        Stop->setObjectName(QString::fromUtf8("Stop"));
        Select = new QAction(MainWindow);
        Select->setObjectName(QString::fromUtf8("Select"));
        Choose = new QAction(MainWindow);
        Choose->setObjectName(QString::fromUtf8("Choose"));
        actionclose = new QAction(MainWindow);
        actionclose->setObjectName(QString::fromUtf8("actionclose"));
        Files = new QAction(MainWindow);
        Files->setObjectName(QString::fromUtf8("Files"));
        Video = new QAction(MainWindow);
        Video->setObjectName(QString::fromUtf8("Video"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        widget_2 = new QWidget(centralWidget);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        widget_2->setGeometry(QRect(10, 0, 541, 301));
        videowidget = new QVideoWidget(widget_2);
        videowidget->setObjectName(QString::fromUtf8("videowidget"));
        videowidget->setGeometry(QRect(40, 30, 480, 272));
        videowidget->setStyleSheet(QString::fromUtf8("border-image: url(:/new/prefix1/resource/file(1).jpeg);"));
        info_box = new QTextBrowser(centralWidget);
        info_box->setObjectName(QString::fromUtf8("info_box"));
        info_box->setGeometry(QRect(50, 660, 471, 51));
        graphicsView = new QChartView(centralWidget);
        graphicsView->setObjectName(QString::fromUtf8("graphicsView"));
        graphicsView->setGeometry(QRect(1130, 30, 381, 331));
        cameraView = new QLabel(centralWidget);
        cameraView->setObjectName(QString::fromUtf8("cameraView"));
        cameraView->setGeometry(QRect(49, 370, 480, 240));
        cameraView->setStyleSheet(QString::fromUtf8("border-image: url(:/new/prefix1/resource/camera(1).jpeg);"));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(267, 310, 61, 23));
        label->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(257, 620, 61, 23));
        label_2->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));
        resultView = new QLabel(centralWidget);
        resultView->setObjectName(QString::fromUtf8("resultView"));
        resultView->setGeometry(QRect(600, 30, 480, 272));
        resultView->setStyleSheet(QString::fromUtf8("border-image: url(:/new/prefix1/resource/network.jpg);\n"
""));
        resultView->setScaledContents(true);
        textBrowser = new QTextBrowser(centralWidget);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        textBrowser->setGeometry(QRect(600, 370, 471, 321));
        textBrowser->setStyleSheet(QString::fromUtf8("border-image: url(:/new/prefix1/resource/cmd.jpeg);"));
        listView = new QListView(centralWidget);
        listView->setObjectName(QString::fromUtf8("listView"));
        listView->setGeometry(QRect(0, 0, 1541, 761));
        listView->setStyleSheet(QString::fromUtf8("background-image: url(:/new/prefix1/resource/bk.png);"));
        result = new QPushButton(centralWidget);
        result->setObjectName(QString::fromUtf8("result"));
        result->setGeometry(QRect(420, 320, 111, 31));
        result->setStyleSheet(QString::fromUtf8("background-color: rgb(193, 193, 193);"));
        yolop_process = new QPushButton(centralWidget);
        yolop_process->setObjectName(QString::fromUtf8("yolop_process"));
        yolop_process->setGeometry(QRect(600, 320, 111, 31));
        yolop_process->setStyleSheet(QString::fromUtf8("background-color: rgb(193, 193, 193);"));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(817, 310, 61, 23));
        label_3->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));
        treeView = new QTreeView(centralWidget);
        treeView->setObjectName(QString::fromUtf8("treeView"));
        treeView->setGeometry(QRect(1130, 390, 381, 301));
        MainWindow->setCentralWidget(centralWidget);
        listView->raise();
        widget_2->raise();
        info_box->raise();
        graphicsView->raise();
        cameraView->raise();
        label->raise();
        label_2->raise();
        resultView->raise();
        textBrowser->raise();
        result->raise();
        yolop_process->raise();
        label_3->raise();
        treeView->raise();
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1535, 28));
        menu = new QMenu(menuBar);
        menu->setObjectName(QString::fromUtf8("menu"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menu->menuAction());
        menu->addAction(Open);
        menu->addAction(Stop);
        menu->addAction(Video);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        Open->setText(QApplication::translate("MainWindow", "Open", nullptr));
        Stop->setText(QApplication::translate("MainWindow", "Stop", nullptr));
        Select->setText(QApplication::translate("MainWindow", "Select", nullptr));
        Choose->setText(QApplication::translate("MainWindow", "choose", nullptr));
        actionclose->setText(QApplication::translate("MainWindow", "close", nullptr));
        Files->setText(QApplication::translate("MainWindow", "Files", nullptr));
        Video->setText(QApplication::translate("MainWindow", "Video", nullptr));
        cameraView->setText(QString());
        label->setText(QApplication::translate("MainWindow", "\350\247\206\351\242\221\347\224\273\351\235\242", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "\347\233\270\346\234\272\347\224\273\351\235\242", nullptr));
        resultView->setText(QString());
        result->setText(QApplication::translate("MainWindow", "\350\247\206\351\242\221\346\226\207\344\273\266", nullptr));
        yolop_process->setText(QApplication::translate("MainWindow", "\350\275\246\351\201\223\347\272\277\350\257\206\345\210\253", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "\350\257\206\345\210\253\347\224\273\351\235\242", nullptr));
        menu->setTitle(QApplication::translate("MainWindow", "\346\221\204\345\203\217\345\244\264", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

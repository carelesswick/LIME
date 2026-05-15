#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QTimer>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QtMultimediaWidgets/QtMultimediaWidgets>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QtMultimedia/QMediaPlayer>
#include <QVBoxLayout>
#include "sysinfolinuximpl.h"
#include <QProcess>
#include <float.h>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <iostream>
#include <omp.h>
#include <QtCharts>

using namespace cv;
using namespace std;

#define internal 8     //修改这里以改变多久保存一帧
QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

    QImage MatImageToQt(const Mat &src);
    void InitChart();

        friend sysinfolinuximpl;

private slots:
    void readFrame();

    void on_Open_triggered();

    void on_Stop_triggered();

    void on_Select_triggered();

    void yolop_process();

    void readBashStandardOutputInfo();

    void timerTimeOut();

    void on_LegendMarkerClicked();

private:
    Ui::MainWindow *ui;
    /* 摄像头对象 */
    VideoCapture cap;
    /* opencv格式下摄像头采集的图像帧 */
    Mat src_image;
    /* 定时器 */
    QTimer *timer;
    QTimer *timer2;       
    /* 本地视频文件路径 */
    QString vid_dir;
    /* 播放器 */
    QMediaPlayer *player;
    /* 视频播放控件 */
    QVideoWidget *videowidget;
    /* 终端进程控制 */
    QProcess* process2;
    QProcess* process3;
    /* 识别图片存放文件系统 */
    QFileSystemModel *m_model;
    /* 摄像头采集图片数量时间统计 */
    int count;
    double t = 0;

    /* 接收数据接口 */
    void receivedData_cpu(double);
    void receivedDate_mem(double);
    /* 数据最大个数 */
    int maxSize;
    /* x 轴上的最大值 */
     int maxX;
     /* y 轴上的最大值 */
     int maxY;
     /* y 轴 */
     QValueAxis *axisY;
     /* x 轴 */
     QValueAxis *axisX;
     /* QList int 类型容器 */
     QList<double> data_cpu;
     QList<double> data_mem;
     /* QSplineSeries 对象（曲线）*/
     QSplineSeries *series_cpu;
     QSplineSeries *series_mem;
     /* QChart 图表 */
     QChart *chart;
     /* 图表视图 */
     QChartView *chartView;
     /*实例化sysinfolinuximpl类*/
     sysinfolinuximpl sysinfo;
};

#endif // MAINWINDOW_H

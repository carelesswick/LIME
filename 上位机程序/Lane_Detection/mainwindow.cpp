#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <vector>
#include <iostream>
#include <QDateTime>
#include <QMessageBox>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //识别图片所在文件夹目录显示
    m_model = new QFileSystemModel;
    QString path = "/home/kylin/桌面/project_v1.0/LSTR/result/";
    m_model->setRootPath(path);
    ui->treeView->setModel(m_model);
    ui->treeView->setRootIndex(m_model->index(path));

    //初始化定时器1，用于控制摄像头采集帧率
    timer = new QTimer(this);
    //初始化定时器2，用于控制硬件监视器刷新率
    timer2 = new QTimer(this);
    //创建进程2用于控制卷积神经网络程序执行
    process2 = new QProcess;
    //创建进程3用于对本地视频文件按一定帧率提取视频帧
    process3 = new QProcess;
    //开启进程终端并等待指令
    process2->start("bash");
    process3->start("bash");
    process2->waitForStarted();
    process3->waitForStarted();

    timer2->start(1000);

    /* 定义槽函数 对应上位机功能 */
    //摄像头采集
    connect(timer,SIGNAL(timeout()),this,SLOT(readFrame()));
    //开启摄像头
    connect(ui->Open,SIGNAL(clicked()),this,SLOT(on_Open_triggered()));
    //关闭摄像头
    connect(ui->Stop,SIGNAL(clicked()),this,SLOT(on_Stop_triggered()));
    //选择本地视频文件并播放
    connect(ui->result,SIGNAL(clicked()),this,SLOT(on_Select_triggered()));
    //启动神经网络车道线识别功能
    connect(ui->yolop_process,SIGNAL(clicked()),this,SLOT(yolop_process()));
    //打印终端输出信息
    connect(process2, SIGNAL(readyReadStandardOutput()), this, SLOT(readBashStandardOutputInfo()));
    //控制硬件监视器刷新
    connect(timer2, SIGNAL(timeout()), this, SLOT(timerTimeOut()));

    //初始化硬件监视器
    InitChart();
    setWindowTitle("神经网络车道线识别系统");
}

MainWindow::~MainWindow()
{
    delete ui;
}

//开启摄像头
void MainWindow::on_Open_triggered()
{
    cap.open(0);
    //控制摄像头采集帧率在25帧以上
    timer->start(3);
    t = getTickCount();
    count = 0;
}

//关闭摄像头
void MainWindow::on_Stop_triggered()
{
    // 停止读取数据。
    timer->stop();
    cap.release();
    ui->cameraView->clear();
    //统计摄像头开启期间采集图片数量并显示
    t = ((double)getTickCount() - t) / getTickFrequency() - 1.500;      //点击按钮到显示画面有一个延时，统计时需要减去
    ui->info_box->append(tr("摄像头运行了 %1 s, 采集了 %2 张图像").arg(t).arg(count));

}

//选择本地视频文件并播放
void MainWindow::on_Select_triggered()
{
    if (cap.isOpened())
    {
        QMessageBox::warning(this, "Warning!", "请关闭摄像头再操作!");
        return;
    }
    //本地视频文件路径
    vid_dir = "/home/kylin/桌面/project_v1.0/videos/";
    //打开文件夹
    QString filename2 = QFileDialog::getOpenFileName(this, tr("文件夹"), vid_dir, tr("video files(*.avi *.mp4 *.wmv);;images(*.png *jpg *bmp);;All files(*.*)"));
    if(filename2.isEmpty())
        QMessageBox::warning(this, "Warning!", "文件夹路径错误!");
    else
    {
        //使用ffmpeg对视频采集画面帧
        process3->write("cd /home/kylin/桌面/project_v1.0/LSTR/videos/\n");
        process3->write("ffmpeg -i test.mp4 -vf fps=10 frames/%d.jpg\n");
        waitKey(2000);
        //启动视频播放器
        player=new QMediaPlayer;
        videowidget = ui->videowidget;
        videowidget->show();
        player->setVideoOutput(videowidget);
        //设置需要打开的媒体文件
        player->setMedia(QUrl::fromLocalFile(filename2));
        player->play();
        if(player->state() == QMediaPlayer::StoppedState)
        {
           videowidget->close();
           return;
        }
    }
}

//启动神经网络车道线识别功能
void MainWindow::yolop_process()
{
    //通过终端命令控制神经网络程序执行
    process2->write("cd /home/kylin/桌面/project_v1.0/LSTR/build\n");
    process2->write("./LSTR ../videos/frames/\n");

    waitKey(10000);//为了缓解识别期间CPU负荷大程序拥塞，设置延时
    for(int i = 1; i < INT_MAX; i++)
    {
        //读取识别的图片并显示在上位机
        Mat r = imread("/home/kylin/桌面/project_v1.0/LSTR/result/" + to_string(i) + ".jpg");
        if(r.empty()) break;
        QImage rq = MatImageToQt(r);
        ui->resultView->setPixmap(QPixmap::fromImage(rq));
        waitKey(100);
    }

}

//打印终端输出信息
void MainWindow::readBashStandardOutputInfo()
{
    QByteArray _out = process2->readAllStandardOutput();
    if(!_out.isEmpty())
        ui->textBrowser->append("<font color=\"#FFFFFF\">" +QString::fromLocal8Bit(_out)+ "</font> ");
}

//摄像头采集
void MainWindow::readFrame()
{

    cap.read(src_image);
    if(!src_image.empty())
    {
        QImage qsrc = MatImageToQt(src_image);
        ui->cameraView->setPixmap(QPixmap::fromImage(qsrc));
        Mat re;
        cv::resize(src_image, re, cv::Size(320,240), cv::INTER_AREA);
        imwrite("/home/kylin/桌面/project_v1.0/frames/"+ to_string(count) + ".jpg", re);
        //统计图片数量
        count ++;
    }
}

//Mat转成QImage以在上位机显示
QImage MainWindow::MatImageToQt(const Mat &src)
{
    //CV_8UC1 8位无符号的单通道---灰度图片
    if(src.type() == CV_8UC1)
    {
        //使用给定的大小和格式构造图像
        //QImage(int width, int height, Format format)
        QImage qImage(src.cols,src.rows,QImage::Format_Indexed8);
        //扩展颜色表的颜色数目
        qImage.setColorCount(256);

        //在给定的索引设置颜色
        for(int i = 0; i < 256; i ++)
        {
            //得到一个黑白图
            qImage.setColor(i,qRgb(i,i,i));
        }
        //复制输入图像,data数据段的首地址
        uchar *pSrc = src.data;
        //
        for(int row = 0; row < src.rows; row ++)
        {
            //遍历像素指针
            uchar *pDest = qImage.scanLine(row);
            //从源src所指的内存地址的起始位置开始拷贝n个
            //字节到目标dest所指的内存地址的起始位置中
            memcmp(pDest,pSrc,src.cols);
            //图像层像素地址
            pSrc += src.step;
        }
        return qImage;
    }
    //为3通道的彩色图片
    else if(src.type() == CV_8UC3)
    {
        //得到图像的的首地址
        const uchar *pSrc = (const uchar*)src.data;
        //以src构造图片
        QImage qImage(pSrc,src.cols,src.rows,src.step,QImage::Format_RGB888);
        //在不改变实际图像数据的条件下，交换红蓝通道
        return qImage.rgbSwapped();
    }
    //四通道图片，带Alpha通道的RGB彩色图像
    else if(src.type() == CV_8UC4)
    {
        const uchar *pSrc = (const uchar*)src.data;
        QImage qImage(pSrc, src.cols, src.rows, src.step, QImage::Format_ARGB32);
        //返回图像的子区域作为一个新图像
        return qImage.copy();
    }
    else
    {
        return QImage();
    }
}

//初始化图表
void MainWindow::InitChart()
{
    // 创建图表的各个部件
    chart = new QChart();
    chart->setTitle("硬件监视器");

    maxSize = 51;
     /* x 轴上的最大值 */
    maxX = 5000;
     /* y 轴最大值 */
     maxY = 100;
    // 创建曲线序列
    series_cpu = new QSplineSeries();
    series_mem = new QSplineSeries();
    series_cpu->setName("CPU");
    series_mem->setName("内存");

    // 序列添加到图表
    chart->addSeries(series_cpu);
    chart->addSeries(series_mem);
    chart->createDefaultAxes();
    // 其他附加参数
    series_cpu->setPointsVisible(true);       // 设置数据点可见


    // 创建坐标轴
    axisX = new QValueAxis;    // X轴
    axisX->setRange(0, maxX);               // 设置坐标轴范围
    axisX->setTitleText("刷新率1秒/次");         // 标题
    axisX->setLabelFormat("%i");         // 设置x轴格式
    axisX->setTickCount(3);               // 设置刻度
    axisX->setMinorTickCount(3);

    axisY = new QValueAxis;    // Y轴
    axisY->setRange(0, maxY);               // Y轴范围(-1 - 20)
    axisY->setTitleText("占用率");         // 标题

    // 设置X于Y轴数据集
    chart->setAxisX(axisX, series_cpu);   // 为序列设置坐标轴
    chart->setAxisY(axisY, series_cpu);
    chart->setAxisX(axisX, series_mem);   // 为序列设置坐标轴
    chart->setAxisY(axisY, series_mem);
    //series_cpu->attachAxis(axisX);
    //chart->setAxisX(axisX, series1);   // 为序列设置坐标轴
    //chart->setAxisY(axisY, series1);
    ui->graphicsView->setChart(chart);
    //设置抗锯齿
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setAttribute(Qt::WA_TranslucentBackground);
    // 设置图表主题色
    ui->graphicsView->chart()->setTheme(QChart::ChartTheme(0));
    //chart->setBackgroundVisible(false);  //去背景
    //chart->setTheme(QChart::ChartThemeDark);
    chart->setTheme(QChart::ChartThemeLight);
    //qsrand(time(NULL));

    // 图例被点击后触发
    foreach (QLegendMarker* marker, chart->legend()->markers())
    {
       QObject::disconnect(marker, SIGNAL(clicked()), this, SLOT(on_LegendMarkerClicked()));
       QObject::connect(marker, SIGNAL(clicked()), this, SLOT(on_LegendMarkerClicked()));
    }
}

/*每隔1s执行一次此函数,其功能为接受sysinfolinuximpl计算的数据,并传输给receivedData_cpu*/
void MainWindow::timerTimeOut()
 {
    double cpuLoadAverage = sysinfo.cpuLoadAverage();
    double mem_used = sysinfo.get_mem_usage__();
    /* 产生随机 0~maxY 之间的数据 */
    receivedData_cpu(cpuLoadAverage);
    receivedDate_mem(mem_used);

 }
/*此函数刷新QChart图表CPU数据*/
void MainWindow::receivedData_cpu(double value)
 {
     /* 将数据添加到 data 中 */
     data_cpu.append(value);

     /* 当储存数据的个数大于最大值时，把第一个数据删除 */
     while (data_cpu.size() > maxSize) {
     /* 移除 data 中第一个数据 */
     data_cpu.removeFirst();
     }

     /* 先清空 */
     series_cpu->clear();

     /* 计算 x 轴上的点与点之间显示的间距 */
     int xSpace = maxX / (maxSize - 1);

     /* 添加点，xSpace * i 表示第 i 个点的 x 轴的位置 */
     for (int i = 0; i < data_cpu.size(); ++i) {
     series_cpu->append(xSpace * i, data_cpu.at(i));
 }
}
/*此函数刷新QChart图表的内存数据*/
void MainWindow::receivedDate_mem(double value)
 {
     /* 将数据添加到 data 中 */
     data_mem.append(value);

     /* 当储存数据的个数大于最大值时，把第一个数据删除 */
     while (data_mem.size() > maxSize) {
     /* 移除 data 中第一个数据 */
     data_mem.removeFirst();
     }

     /* 先清空 */
     series_mem->clear();

     /* 计算 x 轴上的点与点之间显示的间距 */
     int xSpace = maxX / (maxSize - 1);

     /* 添加点，xSpace * i 表示第 i 个点的 x 轴的位置 */
     for (int i = 0; i < data_mem.size(); ++i) {
     series_mem->append(xSpace * i, data_mem.at(i));
 }
}

// 图例点击后显示与隐藏线条,
void MainWindow::on_LegendMarkerClicked()
{
    QLegendMarker* marker = qobject_cast<QLegendMarker*> (sender());

    switch (marker->type())
    {
        case QLegendMarker::LegendMarkerTypeXY:
        {
            marker->series()->setVisible(!marker->series()->isVisible());
            marker->setVisible(true);
            qreal alpha = 1.0;
            if (!marker->series()->isVisible())
                alpha = 0.5;

            QColor color;
            QBrush brush = marker->labelBrush();
            color = brush.color();
            color.setAlphaF(alpha);
            brush.setColor(color);
            marker->setLabelBrush(brush);

            brush = marker->brush();
            color = brush.color();
            color.setAlphaF(alpha);
            brush.setColor(color);
            marker->setBrush(brush);

            QPen pen = marker->pen();
            color = pen.color();
            color.setAlphaF(alpha);
            pen.setColor(color);
            marker->setPen(pen);
            break;
        }
        default:
            break;
    }
}

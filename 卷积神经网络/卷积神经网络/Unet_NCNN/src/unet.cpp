#include "net.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <time.h>
#include <algorithm>
#include <map>
#include <iostream>

using namespace std;
using namespace cv;

#define INPUT_WIDTH     720
#define INPUT_HEIGHT    720

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("illegal parameters!");
        exit(0);
    }

    ncnn::Net Unet;//声明并定义了一个名为 Unet 的 ncnn::Net 类型对象
    // 加载神经网络模型
    Unet.load_param("../models/model.ncnn.param");//模型结构（层定义、连接关系）
    Unet.load_model("../models/model.ncnn.bin"); // 模型权重（训练好的参数）

    int64 tic, toc;

    tic = cv::getTickCount();// 获取当前时间

    cv::Scalar value = Scalar(0,0,0);//给图像补黑边
    cv::Mat src;
    cv::Mat tmp;
    src = cv::imread(argv[1]);

    // 根据需要的尺寸，调整图像大小并填充边界
    float width = src.size().width;
    float height = src.size().height;
    int top = 0, bottom = 0;
    int left = 0, right = 0;

    //copyMakeBorder函数用于在图像周围添加边界（补黑边），将输入的图像调整为正方形的尺寸为tmp
    if (width > height) {
        top = (width - height) / 2;
        bottom = (width - height) - top;
        cv::copyMakeBorder(src, tmp, top, bottom, 0, 0, BORDER_CONSTANT, value);
    } else {
        left = (height - width) / 2;
        right = (height - width) - left;
        cv::copyMakeBorder(src, tmp, 0, 0, left, right, BORDER_CONSTANT, value);
    }

    // 根据输入尺寸和原始图像尺寸的比例，计算边界的大小
    top = (INPUT_HEIGHT*top)/width;
    bottom = (INPUT_HEIGHT*bottom)/width;
    left = (INPUT_WIDTH*left)/height;
    right = (INPUT_WIDTH*right)/height;

    // 调整图像大小为720*720即tmp1
    cv::Mat tmp1;
    cv::resize(tmp, tmp1, cv::Size(INPUT_WIDTH, INPUT_HEIGHT), 0,0,INTER_CUBIC);

    // 将图像转换为浮点数类型，并归一化到范围 [0, 1]
    cv::Mat image;
    tmp1.convertTo(image, CV_32FC3, 1/255.0);

    // cv32fc3 的布局是 hwc ncnn的Mat布局是 chw 需要调整排布
    float *srcdata = (float*)image.data;//是OpenCV的HWC格式的内存指针
    float *data = new float[INPUT_WIDTH*INPUT_HEIGHT*3];//申请新的内存，用来存CHW格式的数据
    for (int i = 0; i < INPUT_HEIGHT; i++)
       for (int j = 0; j < INPUT_WIDTH; j++)
           for (int k = 0; k < 3; k++) {
              data[k*INPUT_HEIGHT*INPUT_WIDTH + i*INPUT_WIDTH + j] = srcdata[i*INPUT_WIDTH*3 + j*3 + k];
           }

    // 创建 ncnn::Mat 对象作为输入
    ncnn::Mat in(image.rows*image.cols*3, data);//将数据转换为ncnn的Mat格式
    in = in.reshape(720, 720, 3);//三维
    

    Unet.opt.num_threads = 4;
    // 创建 ncnn::Extractor 对象并设置参数
    ncnn::Extractor ex = Unet.create_extractor();
    // 设置推理的模式和线程数
    ex.set_light_mode(true);//轻量推理模式

    // 输入图像并进行推理
    ex.input("in0", in);//输入图像
    ncnn::Mat mask;
    ex.extract("out0", mask);//输出掩码

#if 1
    cv::Mat cv_img = cv::Mat::zeros(INPUT_WIDTH,INPUT_HEIGHT,CV_8UC1);//这一行在创建并初始化一个灰度图（掩码）
    {
    float *srcdata = (float*)mask.data;
    unsigned char *data = cv_img.data;

    // 将输出的掩码转换为灰度图像
    for (int i = 0; i < mask.h; i++)
       for (int j = 0; j < mask.w; j++) {
#if 1
         float tmp = srcdata[0*mask.w*mask.h+i*mask.w+j];
         int maxk = 0;
         for (int k = 0; k < mask.c; k++) {
           if (tmp < srcdata[k*mask.w*mask.h+i*mask.w+j]) {
             tmp = srcdata[k*mask.w*mask.h+i*mask.w+j];
             maxk = k;
           }
         }

         data[i*INPUT_WIDTH + j] = maxk;

         // 去除填充边界
         if ((left > 0) && (right > 0) && ((j < left) || (j >= INPUT_WIDTH - right)))
           data[i*INPUT_WIDTH + j] = 0;

         if ((top > 0) && (bottom > 0) && ((i < top) || (i >= INPUT_HEIGHT - bottom)))
           data[i*INPUT_WIDTH + j] = 0;
#else
         if (srcdata[1*mask.w*mask.h+i*mask.w+j] > 0.999)
           data[i*INPUT_WIDTH + j] = 1;
         else
           data[i*INPUT_WIDTH + j] = 0;
#endif
       }
    }

    {
        toc = cv::getTickCount() - tic;

        double time = toc / double(cv::getTickFrequency());
        std::cout << "time:" << time << "s" <<std::endl;
    }

    // 将灰度图像转换为彩色图像，并将掩码区域标记为绿色
    cv_img *= 255;
    cv::Mat result;
    image.copyTo(result);
    result.setTo(cv::Scalar(0,255,0),cv_img);
    cv::imwrite("result.jpg", result);
    //cv::imshow("test", result);
    //cv::waitKey();
#endif
    return 0;
}

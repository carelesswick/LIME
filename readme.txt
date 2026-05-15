Readme
1.OpenCV的安装
　　从官网下载OPenCV源码包，本项目使用的是OpenCV4.7.0, 进入源码包目录，在终端界面输入：
　　./configure
　　完成配置，然后进行编译：
　　make -j8
　　等待编译完成，执行安装：
　　Sudo make install
　　opencv会自动安装到系统根目录/usr/local下面，代码中包含头文件能自动识别
2.NCNN框架的安装
　　首先需要安装protobuf
　　$ sudo apt-get install autoconf automake libtool curl make g++ unzip
　　$ git clone https://github.com/google/protobuf.git
　　$ cd protobuf
　　$ git submodule update --init --recursive
　　$ ./autogen.sh
　　$ ./configure
　　$ make
　　$ make check
　　$ sudo make install
　　$ sudo ldconfig  #refresh shared library cache
　　
　　然后安装ncnn
　　$ git clone https://github.com/Tencent/ncnn
　　$ cd <ncnn-root-dir>
　　$ mkdir -p build
　　$ cd build
　　$ cmake ..
　　$ make -j4

3.ONNX框架的安装
　　下载onnxruntime（建议按照官方文档直接clone，不要自己到github上下载，不然运行过程中会报找不到某个文件的错误）
　　git clone --recursive https://github.com/Microsoft/onnxruntime
　　cd onnxruntime
　　安装onnxruntime
　　./build.sh --config RelWithDebInfo --build_wheel --update --build

4.安装QT
　　本项目安装的是qt5.12.8,从官网下载对应的源码包进行编译
　　本项目中包含的重要qt组件有qtchart/qtmultimedia 均采用从官网下载5.12.8对应的子模块源码包编译。
　　**上位机中视频文件采集帧画面需要在终端调用ffmpeg工具，本项目开发板已经自带，如果没有该组件需要自行安装：
　　Sudo apt-get install ffmpeg
　　qt中第三方库的链接,需要在.pro文件中链接第三方包含文件和链接库，本项目需要的第三方库是opencv
5.文件路径的说明
　　由于项目开发板环境尚存在一些问题，为了保证程序的正常运行，开发板在测试过程中使用的都是绝对路径，现将代码中需要用到的文件路径作说明如下：
　　1）摄像头采集图片保存路径：
　　上位机软件根目录/frames
　　2）本地视频文件路径：
　　上位机软件根目录/LSTR/videos
　　3）识别车道线后保存的图片路径：
　　上位机软件根目录/LSTR/result
    4）上位机界面以及各个窗口的背景图片路径：
    上位机软件根目录/resources
    5）神经网络车道线识别功能终端命令
    在函数 yolop_process 当中：
		process2->write("cd /上位机软件根目录/LSTR/build\n");
		process2->write("./LSTR ../videos/frames/\n");
6.编译方式说明
　　1）LIME算法：
　　进入到LIME算法cmake工程根目录下，增强前和增强后操作相同
　　mkdir build
　　cd build
　　cmake ..
　　make      
　　./lime
测试用的图片素材存放在工程目录中的data子目录下，可以在代码中更换图片进行测试
　　2）Unet NCNN编译
　　进入到Unet_NCNN目录下
　　mkdir build
　　cd build
　　cmake ..
　　make
　　./unet_ncnn ../images/0.jpg  //识别子目录images下0.jpg 参数2可以自行更改以测试不同的图片
　　3）LSTR_ONNX编译
　　进入到LSTR_ONNX目录下，
　　mkdir build
　　cd build
　　cmake ..
　　make
　　./LSTR
　　测试用的图片素材存放在工程目录中的images子目录下，可以在代码中更换图片进行测试

**源代码中所有工程编译需要的第三方包含头文件和链接库已经一并打包在了工程目录中，按照上述说明进行编译即可。
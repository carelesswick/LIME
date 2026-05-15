#include <opencv2/opencv.hpp>
#include <iostream>
#include <omp.h>
#include <arm_neon.h>

namespace LIME {

    class lime {
    public:
        cv::Mat img_norm; // 输入图像归一化
        cv::Mat R;
        cv::Mat out_lime; // 增强后的图像
        cv::Mat dv;
        cv::Mat dh;
        cv::Mat T_hat;    // 初始化的光照图
        cv::Mat W;        // 权重矩阵
        int channel;      // 存储图像通道数
        int row;          // 图像的行数
        int col;          // 图像的列数
        float alpha = 1;
        float rho = 2;
        float gamma = 0.7;
        float epsilon;    // 迭代系数
        float thd;        // 迭代收敛时间阈值

    public:
        lime(cv::Mat src);
        cv::Mat enhance(cv::Mat &src); // 图像增强

        void _init_IllumMap(cv::Mat src);
        void weightStrategy();
        void Illum_filter(cv::Mat& img_in, cv::Mat& img_out);
        void Illumination(cv::Mat& src, cv::Mat& out);
        cv::Mat getMax(const cv::Mat &bgr);
        cv::Mat Dev(int n, int k);
        cv::Mat getReal(cv::Mat mat);
        cv::Mat Frobenius(cv::Mat mat);
        cv::Mat derivative(cv::Mat matrix);
        cv::Mat solveT(cv::Mat G, cv::Mat Z, float u);
        cv::Mat solveG(cv::Mat T, cv::Mat Z, float u, cv::Mat W);
        cv::Mat solveZ(cv::Mat T, cv::Mat G, cv::Mat Z, float u);
        float solveU(float u);
        void fft2(const cv::Mat& input, cv::Mat& output, int opt);
        int ReverseBin(int a, int n);
        cv::Mat optIllumMap();
        
        static inline float comp(float& a, float& b, float& c) {
            return fmax(a, fmax(b, c));
        }
    };

    lime::lime(cv::Mat src) {
        channel = src.channels();
    }

    // 初始化光照图
    void lime::_init_IllumMap(cv::Mat src) {
        src.convertTo(img_norm, CV_32F, 1 / 255.0, 0);
        row = img_norm.rows;
        col = img_norm.cols;
        T_hat = lime::getMax(img_norm);  
        epsilon = Frobenius(T_hat)*0.001;
        dv = Dev(row, 1);
        dh = Dev(col, -1);
    }

    // 获取色彩通道最大值
    cv::Mat lime::getMax(const cv::Mat& bgr) {
        cv::Mat temp_mat(row, col, CV_32F, cv::Scalar::all(0.0));
        std::vector<cv::Mat> img_norm_rgb;
        cv::split(bgr, img_norm_rgb);
        cv::Mat img_norm_b = img_norm_rgb.at(0);
        cv::Mat img_norm_g = img_norm_rgb.at(1);
        cv::Mat img_norm_r = img_norm_rgb.at(2);

        #pragma omp parallel for
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                temp_mat.at<float>(i, j) = fmax(img_norm_g.at<float>(i, j), 
                                                fmax(img_norm_b.at<float>(i, j), img_norm_r.at<float>(i, j)));
            }
        }
        return temp_mat;
    }

    // 图像增强函数
    cv::Mat lime::enhance(cv::Mat &src) {
        _init_IllumMap(src);
        cv::Size sz(img_norm.size());
        R = cv::Mat(sz, CV_32F, cv::Scalar::all(0.0));

        // 将图像分成128x128的块进行处理
        int blockSize = 128;
        cv::Mat out_img = cv::Mat::zeros(row, col, CV_32F);

        #pragma omp parallel for
        for (int i = 0; i < row; i += blockSize) {
            for (int j = 0; j < col; j += blockSize) {
                int blockRows = std::min(blockSize, row - i);
                int blockCols = std::min(blockSize, col - j);

                // 获取当前块的子矩阵
                cv::Mat block = img_norm(cv::Rect(j, i, blockCols, blockRows));

                // 计算该块的亮度，并使用阈值决定是否进行增强
                float blockMax = cv::norm(block, cv::NORM_INF); // 计算当前块的最大亮度值

                // 设置一个阈值，判断该块是否需要增强
                float threshold = 0.5;
                if (blockMax < threshold) {
                    // 需要增强
                    block = block / T_hat;  // 对亮度较低的区域进行增强
                }

                // 将增强后的块放回到对应的位置
                block.copyTo(out_img(cv::Rect(j, i, blockCols, blockRows)));
            }
        }

        // 返回增强后的图像
        out_img.convertTo(out_lime, CV_8U, 255);  // 转换为8位图像
        return out_lime;
    }

    // 求解矩阵范数
    float lime::Frobenius(cv::Mat mat) {
        float norm = 0.0f;
        for (int i = 0; i < mat.rows; i++) {
            for (int j = 0; j < mat.cols; j++) {
                norm += mat.at<float>(i, j) * mat.at<float>(i, j);
            }
        }
        return sqrt(norm);
    }

    // FFT2变换
    void lime::fft2(const cv::Mat& input, cv::Mat& output, int opt) {
        // 使用FFT进行图像处理
        // 这里省略FFT的详细实现
    }

    // 求解u
    float lime::solveU(float u) {
        return u * rho;
    }

    // 计算子问题T
    cv::Mat lime::solveT(cv::Mat G, cv::Mat Z, float u) {
        // 这里省略T的计算部分
    }

    // 最终获取优化后的光照图
    cv::Mat lime::optIllumMap() {
        // 省略光照图的优化过程
    }

}

int main() {
    // 加载图片
    cv::Mat img_in = cv::imread("../data/38.jpg");
    if (img_in.empty()) {
        std::cerr << "Error Input!" << std::endl;
        return -1;
    }

    LIME::lime *l = new LIME::lime(img_in);  // 创建 LIME 算法对象
    cv::Mat img_out = l->enhance(img_in);  // 进行图像增强

    // 显示结果
    cv::imshow("Enhanced Image", img_out);
    cv::imwrite("output.jpg", img_out);

    cv::waitKey(0);  // 等待按键
    return 0;
}
/*这段代码实现了一个基于局部亮度增强的图像增强算法，具体来说，它使用了LIME（Low-Light Image Enhancement）方法来增强低光条件下的图像。以下是对代码的详细解释：

### 1. 函数定义
```cpp
cv::Mat lime::enhance(cv::Mat &src)
```
• `cv::Mat lime::enhance(cv::Mat &src)` 是一个成员函数，接受一个 `cv::Mat` 类型的输入图像 `src`，并返回一个增强后的图像。

### 2. 初始化光照图
```cpp
_init_IllumMap(src);
```
• `_init_IllumMap(src)` 是一个内部函数，用于初始化光照图（Illumination Map）。光照图通常用于表示图像中每个像素的亮度信息。

### 3. 创建输出矩阵
```cpp
cv::Size sz(img_norm.size());
R = cv::Mat(sz, CV_32F, cv::Scalar::all(0.0));
```
• `sz` 是输入图像 `img_norm` 的尺寸。
• `R` 是一个与输入图像大小相同的浮点矩阵，初始化为全0。

### 4. 分块处理图像
```cpp
int blockSize = 128;
cv::Mat out_img = cv::Mat::zeros(row, col, CV_32F);
```
• `blockSize` 设置为128，表示将图像分成128x128的块进行处理。
• `out_img` 是一个与输入图像大小相同的浮点矩阵，初始化为全0，用于存储增强后的图像。

### 5. 并行处理每个块
```cpp
#pragma omp parallel for
for (int i = 0; i < row; i += blockSize) {
    for (int j = 0; j < col; j += blockSize) {
        int blockRows = std::min(blockSize, row - i);
        int blockCols = std::min(blockSize, col - j);

        cv::Mat block = img_norm(cv::Rect(j, i, blockCols, blockRows));

        float blockMax = cv::norm(block, cv::NORM_INF);

        float threshold = 0.5;
        if (blockMax < threshold) {
            block = block / T_hat;
        }

        block.copyTo(out_img(cv::Rect(j, i, blockCols, blockRows)));
    }
}
```
• `#pragma omp parallel for` 使用OpenMP并行化处理每个块，以加速处理速度。
• `i` 和 `j` 是块的起始行和列索引。
• `blockRows` 和 `blockCols` 是当前块的实际行数和列数，确保不会超出图像边界。
• `block` 是从 `img_norm` 中提取的当前块的子矩阵。
• `blockMax` 是当前块的最大亮度值，通过 `cv::norm(block, cv::NORM_INF)` 计算得到。
• `threshold` 是一个阈值，用于判断当前块是否需要增强。如果 `blockMax` 小于阈值，则对该块进行增强。
• `block = block / T_hat` 是对亮度较低的区域进行增强，`T_hat` 是一个预先计算好的增强因子。
• 最后，将增强后的块复制回 `out_img` 的对应位置。

### 6. 返回增强后的图像
```cpp
out_img.convertTo(out_lime, CV_8U, 255);
return out_lime;
```
• `out_img.convertTo(out_lime, CV_8U, 255)` 将浮点矩阵 `out_img` 转换为8位图像，范围为0到255。
• `out_lime` 是最终增强后的图像，函数返回该图像。

### 总结
这段代码的主要功能是通过分块处理的方式，对低光图像进行局部亮度增强。它首先将图像分成128x128的块，然后对每个块进行亮度检测，如果块的亮度低于设定的阈值，则对该块进行增强。最后，将增强后的块重新组合成完整的图像并返回。
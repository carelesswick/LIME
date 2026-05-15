#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <onnxruntime_cxx_api.h>
#include <arm_neon.h>
#include <lime.h>

using namespace cv;
using namespace std;
using namespace Ort;

class LSTR
{
public:
	LSTR();
	Mat detect(Mat& cv_image);
	~LSTR();  // 析构函数, 释放内存
	
private:
	void normalize_(Mat img);   // 图像归一化函数
	int inpWidth;   // 输入图像宽度
	int inpHeight;  // 输出图像宽度
	vector<float> input_image_; // 存储归一化后的图像数据
	vector<float> mask_tensor;  // 存储预测的掩码数据
	float mean[3] = { 0.485, 0.456, 0.406 };   // 图像归一化均值
	float std[3] = { 0.229, 0.224, 0.225 };    // 图像归一化标准差
	const int len_log_space = 50;
	float* log_space;
	// 车道线颜色数组
	const Scalar lane_colors[8] = { Scalar(68,65,249), Scalar(44,114,243),Scalar(30,150,248),Scalar(74,132,249),Scalar(79,199,249),Scalar(109,190,144),Scalar(142, 144, 77),Scalar(161, 125, 39) };

	Env env = Env(ORT_LOGGING_LEVEL_ERROR, "LSTR");   // ONNX Runtime 环境
	Ort::Session *ort_session = nullptr;              // ONNX Runtime 会话指针
	const ORTCHAR_T* model_path;                      // 模型路径
	SessionOptions sessionOptions = SessionOptions(); // 会话选项
	vector<const char*> input_names;   				  // 输入节点名称
	vector<const char*> output_names;				  // 输出节点名称
	vector<AllocatedStringPtr> inputNodeNameAllocatedStrings;   // 输入节点名称的内存分配指针
	vector<AllocatedStringPtr> outputNodeNameAllocatedStrings;
	vector<vector<int64_t>> input_node_dims; // >=1 outputs  // 输入节点维度
	vector<vector<int64_t>> output_node_dims; // >=1 outputs // 输出节点维度
};

LSTR::LSTR()
{
	const ORTCHAR_T* model_path = "../lstr_360x640.onnx";       // 模型文件路径
	sessionOptions.SetGraphOptimizationLevel(ORT_ENABLE_BASIC); // 设置会话的图优化级别为基本优化级别
	ort_session = new Session(env, model_path, sessionOptions); // 创建 ONNX Runtime 会话对象，并加载模型
	size_t numInputNodes = ort_session->GetInputCount(); 		// 获取输入节点数量
	size_t numOutputNodes = ort_session->GetOutputCount();		// 获取输出节点数量
	AllocatorWithDefaultOptions allocator;						// 创建内存分配器对象
	// 处理输入节点
	for (int i = 0; i < numInputNodes; i++)
	{
		Ort::AllocatedStringPtr input_name_Ptr = ort_session->GetInputNameAllocated(i, allocator); //获取输入节点名称
		inputNodeNameAllocatedStrings.push_back(std::move(input_name_Ptr));						   // 将输入节点名称的内存分配指针添加到容器中
		input_names.push_back(inputNodeNameAllocatedStrings.back().get());						   // 将输入节点名称添加到容器中
		Ort::TypeInfo input_type_info = ort_session->GetInputTypeInfo(i);						   // 获取输入节点的类型信息
		auto input_tensor_info = input_type_info.GetTensorTypeAndShapeInfo();					   // 获取输入节点的张量类型和形状信息
		auto input_dims = input_tensor_info.GetShape();											   // 获取输入节点的维度信息
		input_node_dims.push_back(input_dims);													   // 将输入节点的维度信息添加到容器中
	}
	// 处理输出节点,与输入类似
	for (int i = 0; i < numOutputNodes; i++)
	{
		Ort::AllocatedStringPtr output_name_Ptr= ort_session->GetOutputNameAllocated(i, allocator);
		outputNodeNameAllocatedStrings.push_back(std::move(output_name_Ptr));
		output_names.push_back(outputNodeNameAllocatedStrings.back().get());
		Ort::TypeInfo output_type_info = ort_session->GetOutputTypeInfo(i);
		auto output_tensor_info = output_type_info.GetTensorTypeAndShapeInfo();
		auto output_dims = output_tensor_info.GetShape();
		output_node_dims.push_back(output_dims);
	}
	this->inpHeight = input_node_dims[0][2];   // 设置输入图像的高度
	this->inpWidth = input_node_dims[0][3];    // 设置输入图像的宽度
	this->mask_tensor.resize(this->inpHeight * this->inpWidth, 0.0);   // 调整掩码数据的大小为图像高度乘以图像宽度，并初始化为0
	log_space = new float[len_log_space];
	FILE* fp = fopen("../log_space.bin", "rb");
	fread(log_space, sizeof(float), len_log_space, fp);//导入数据
	fclose(fp);//关闭文件。
}

LSTR::~LSTR()
{
	delete[] log_space;
	log_space = NULL;
}

void LSTR::normalize_(Mat img)
{
	int row = img.rows;
	int col = img.cols;
	this->input_image_.resize(row * col * img.channels());
	for (int c = 0; c < 3; c++)   // 遍历通道（3通道图像）
	{
		for (int i = 0; i < row; i++)  // 遍历图像的每一行
		{
			for (int j = 0; j < col; j++)  // 遍历图像的每一列
			{
				float pix = img.ptr<uchar>(i)[j * 3 + c];    // 获取图像指定位置的像素值（根据通道数和图像结构进行计算）
				this->input_image_[c * row * col + i * col + j] = (pix / 255.0 - mean[c]) / std[c]; // 归一化并存储到 input_image_ 中
			}
		}
	}
}

Mat LSTR::detect(Mat& srcimg)
{
	const int img_height = srcimg.rows;
	const int img_width = srcimg.cols;
	Mat dstimg;
	// 调整输入图像的大小为网络模型的输入尺寸
	resize(srcimg, dstimg, Size(this->inpWidth, this->inpHeight), INTER_LINEAR);
	// 归一化处理调整后的图像
	this->normalize_(dstimg);
	array<int64_t, 4> input_shape_{ 1, 3, this->inpHeight, this->inpWidth }; // 输入张量的形状
	array<int64_t, 4> mask_shape_{ 1, 1, this->inpHeight, this->inpWidth };  // 掩码张量的形状

	auto allocator_info = MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);
	// 存储输入张量的向量
	vector<Value> ort_inputs;
	
	ort_inputs.push_back(Value::CreateTensor<float>(allocator_info, input_image_.data(), input_image_.size(), input_shape_.data(), input_shape_.size()));

	ort_inputs.push_back(Value::CreateTensor<float>(allocator_info, mask_tensor.data(), mask_tensor.size(), mask_shape_.data(), mask_shape_.size()));
	// 运行推理过程，获取输出张量
	vector<Value> ort_outputs = ort_session->Run(RunOptions{ nullptr }, input_names.data(), ort_inputs.data(), 2, output_names.data(), output_names.size());
	// 获取预测的逻辑张量数据
	const float* pred_logits = ort_outputs[0].GetTensorMutableData<float>();
	// 获取预测的曲线张量数据
	const float* pred_curves = ort_outputs[1].GetTensorMutableData<float>();
	const int logits_h = output_node_dims[0][1]; // 逻辑张量的高度
	const int logits_w = output_node_dims[0][2]; // 逻辑张量的宽度
	const int curves_w = output_node_dims[1][2]; // 曲线张量的宽度
	vector<int> good_detections;     // 存储有效的检测结果索引
	vector< vector<Point>> lanes;    // 存储检测到的车道线点的集合
	for (int i = 0; i < logits_h; i++)
	{
		float max_logits = -10000;
		int max_id = -1;
		for (int j = 0; j < logits_w; j++)
		{
			const float data = pred_logits[i*logits_w + j];   // 获取逻辑张量中指定位置的数据
			if (data > max_logits)
			{
				max_logits = data;
				max_id = j;
			}
		}
		if (max_id == 1)
		{
			good_detections.push_back(i);    // 将有效的检测结果索引添加到 good_detections 向量中
			const float *p_lane_data = pred_curves + i * curves_w;
			vector<Point> lane_points(len_log_space);
			for (int k = 0; k < len_log_space; k++)
			{
				// 计算车道线点的 y 坐标
				const float y = p_lane_data[0] + log_space[k] * (p_lane_data[1] - p_lane_data[0]);  
				// 计算车道线点的 x 坐标
				const float x = p_lane_data[2] / powf(y - p_lane_data[3], 2.0) + p_lane_data[4] / (y - p_lane_data[3]) + p_lane_data[5] + p_lane_data[6] * y - p_lane_data[7];
				// 构建车道线点坐标并添加到 lane_points 向量中
				lane_points[k] = Point(int(x*img_width), int(y*img_height));
			}
			lanes.push_back(lane_points);
		}
	}

	/// draw lines
	vector<int> right_lane;  // 存储右侧车道线索引
	vector<int> left_lane;   // 存储左侧车道线索引
	for (int i = 0; i < good_detections.size(); i++)
	{
		if (good_detections[i] == 0)  // 将索引为 0 的检测结果视为右侧车道线
		{
			right_lane.push_back(i);
		}
		if (good_detections[i] == 5) // 将索引为 5 的检测结果视为左侧车道线
		{
			left_lane.push_back(i);
		}
	}
	Mat visualization_img = srcimg.clone();  // 创建用于可视化的图像副本
	if (right_lane.size() == left_lane.size())  // 如果右侧和左侧车道线数量相等
	{
		Mat lane_segment_img = visualization_img.clone();  // 创建车道线分割图像的副本
		vector<Point> points = lanes[right_lane[0]];       // 获取右侧车道线的点集
		reverse(points.begin(), points.end());			   // 反转点集，以便绘制封闭区域
		// 将左侧车道线的点集插入到右侧车道线点集之前
		points.insert(points.begin(), lanes[left_lane[0]].begin(), lanes[left_lane[0]].end()); 
		// 绘制封闭区域（车道线分割区域）
		fillConvexPoly(lane_segment_img, points, Scalar(0, 255, 0));
		// 将车道线分割区域与原始图像进行叠加
		addWeighted(visualization_img, 0.4, lane_segment_img, 0.6, 0, visualization_img);
	}
	for (int i = 0; i < lanes.size(); i++)
	{
		for (int j = 0; j < lanes[i].size(); j++)
		{
			circle(visualization_img, lanes[i][j], 3, lane_colors[good_detections[i]], -1);
		}
	}
	return visualization_img;   // 返回可视化结果图像
}

int main(int argc, char** argv)
{
	 if (argc != 2)
    {
        fprintf(stderr, "Usage: %s [imagepath]\n", argv[0]);
        return -1;
    }
	//实例化神经网络模型
	LSTR mynet;
	Mat frame;
	//实例化低照度增强预处理LIME算法对象
	LIME::lime *l;
	const char* filefolderpath = argv[1]; //第二个参数是读取的图片文件夹路径
	//string imgpath = "../images/0.jpg";
	double time = cv::getTickCount();
	   cout << "开始计时" << endl;
    for(int i = 1; i < INT_MAX; i++)
    {
        cv::Mat m = cv::imread(filefolderpath + to_string(i) + ".jpg", 1);
        if (m.empty())
        {
            break;
        }
        cout << "正在处理第" << i <<"张图片" << endl;
		Mat d;
		cv::resize(m, d, cv::Size(360,204));
		l = new LIME::lime(d);
		//对图片进行LIME低照度增强预处理
		d = l->enhance(d);
		//使用神经网络进行车道线识别
		Mat dstimg = mynet.detect(d);
		cv::imwrite("../result/" + to_string(i) + ".jpg", dstimg);
		delete l;
	}
	time = ((double)cv::getTickCount() - time) / cv::getTickFrequency();
    cout << "处理完成,共用时" << time << "秒" << endl;
    return 0;
}

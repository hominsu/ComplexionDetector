#pragma once

#ifndef DETECTED_H
#define DETECTED_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <imgproc/imgproc.hpp>

#undef slots
#include <torch/script.h>
#include <torch/torch.h>
#define slots Q_SLOTS

#include <algorithm>
#include <iostream>
#include <memory>
#include <chrono>

class Detected
{
public:
	void initalDetected();
	void loadingModule();
	cv::Mat forward(const cv::Mat &src);
	std::vector<torch::Tensor> non_max_suppression(torch::Tensor preds, float score_thresh, float iou_thresh, bool isGpu);
	bool getisGpu();
	double getusagetime();

private:
	bool isGpu;
	int classId;
	double usagetime;
	torch::DeviceType device_type;
	torch::jit::script::Module module;
	std::vector<std::string> classnames;

};

#endif
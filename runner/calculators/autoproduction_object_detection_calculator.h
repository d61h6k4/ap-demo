#pragma once

#include <cuda_runtime_api.h>

#include <deque>
#include <future>
#include <memory>

#include <opencv2/core/cuda.hpp>

#include "autoproduction/inference/object_detection.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/formats/detection.pb.h"
#include "mediapipe/framework/formats/image_frame_opencv.h"
#include "mediapipe/framework/port/status.h"

namespace mediapipe {
struct Logger : public nvinfer1::ILogger {
  void log(nvinfer1::ILogger::Severity severity, const char *msg) override {
    if (severity != nvinfer1::ILogger::Severity::kINFO &&
        severity != nvinfer1::ILogger::Severity::kVERBOSE) {
      LOG(ERROR) << msg;
    }
  }
};

class AutoproductionObjectDetectionCalculator : public CalculatorBase {
public:
  static ::mediapipe::Status GetContract(CalculatorContract *cc);
  ::mediapipe::Status Open(CalculatorContext *cc) override;
  ::mediapipe::Status Process(CalculatorContext *cc) override;
  ::mediapipe::Status Close(CalculatorContext *cc) override;

private:
  std::shared_ptr<Logger> logger_;
  cudaStream_t stream_;
  std::unique_ptr<Autoproduction::Inference::ObjectDetectionModel<1, 3>>
      odmodel_;

  Npp8u *img_ptr_;
  cv::cuda::GpuMat cuda_img_;
};
} // namespace mediapipe



#include "runner/calculators/autoproduction_object_detection_calculator.h"

#include <vector>

#include "NvInfer.h"

#include "autoproduction/utils/detection.h"
#include "mediapipe/framework/formats/image_frame.h"
#include "mediapipe/framework/formats/rect.pb.h"
#include "mediapipe/framework/port/logging.h"
#include "mediapipe/framework/port/ret_check.h"

namespace mediapipe {
namespace {
constexpr char kImageTag[] = "IMAGE_FRAME";
constexpr char kPersonsDetection[] = "PERSONS_DETECTION";
constexpr char kSportBallsDetection[] = "SPORTBALLS_DETECTION";

::mediapipe::Detection
TensorToDetection(float box_ymin, float box_xmin, float box_ymax,
                  float box_xmax, Autoproduction::Util::Score score,
                  Autoproduction::Util::ClassName class_label) {

  ::mediapipe::Detection detection;
  detection.add_score(score);

  if (class_label == Autoproduction::Util::ClassName::person_) {
    detection.add_label("person");
  } else if (class_label == Autoproduction::Util::ClassName::sport_ball_) {
    detection.add_label("sport ball");
  } else {
    detection.add_label("unk");
  }

  LocationData *location_data = detection.mutable_location_data();
  location_data->set_format(LocationData::RELATIVE_BOUNDING_BOX);

  LocationData::RelativeBoundingBox *relative_bbox =
      location_data->mutable_relative_bounding_box();

  relative_bbox->set_xmin(box_xmin);
  relative_bbox->set_ymin(box_ymin);
  relative_bbox->set_width(box_xmax - box_xmin);
  relative_bbox->set_height(box_ymax - box_ymin);
  return detection;
}

} // namespace
REGISTER_CALCULATOR(AutoproductionObjectDetectionCalculator);

::mediapipe::Status
AutoproductionObjectDetectionCalculator::GetContract(CalculatorContract *cc) {
  cc->Inputs().Tag(kImageTag).Set<ImageFrame>();
  cc->Outputs().Tag(kPersonsDetection).Set<std::vector<Detection>>();
  cc->Outputs().Tag(kSportBallsDetection).Set<std::vector<Detection>>();

  return ::mediapipe::OkStatus();
}

::mediapipe::Status
AutoproductionObjectDetectionCalculator::Open(CalculatorContext *cc) {
  cudaError_t st = cudaSetDevice(0);
  if (st != cudaSuccess) {
    throw std::runtime_error("coud not set CUDA device");
  }

  st = cudaStreamCreate(&stream_);
  if (st != cudaSuccess) {
    throw std::runtime_error("coud not create CUDA stream");
  }

  logger_.reset(new Logger());
  size_t input_size = 800 * 3840 * 3 * sizeof(Npp8u);
  st = cudaMalloc(reinterpret_cast<void **>(&img_ptr_), input_size);
  if (st != cudaSuccess) {
    throw std::runtime_error("Could not allocate input layer");
  }
  cuda_img_ = cv::cuda::GpuMat(800, 3840, CV_8UC3, img_ptr_);

  // TODO(d61h6k4) Can we do it better?
  odmodel_.reset(new Autoproduction::Inference::ObjectDetectionModel<1, 5>(
      "//tmp/models/object_detection_football.onnx", 800, 3840, 640, 640,
      stream_, 0, logger_));

  return ::mediapipe::OkStatus();
}

::mediapipe::Status
AutoproductionObjectDetectionCalculator::Close(CalculatorContext *cc) {
  cudaError_t st = cudaStreamSynchronize(stream_);
  if (st != cudaSuccess) {
    LOG(ERROR) << "CUDA stream synchornization failed";
  }
  st = cudaStreamDestroy(stream_);
  if (st != cudaSuccess) {
    LOG(ERROR) << "CUDA stream destroy failed";
  }
  return ::mediapipe::OkStatus();
}

::mediapipe::Status
AutoproductionObjectDetectionCalculator::Process(CalculatorContext *cc) {
  if (!cc->Inputs().Tag(kImageTag).IsEmpty()) {
    const auto &input_img = cc->Inputs().Tag(kImageTag).Get<ImageFrame>();

    cuda_img_.upload(formats::MatView(&input_img));
    auto dets = odmodel_->operator()(img_ptr_);

    std::vector<Detection> persons_detection;
    std::vector<Detection> sportballs_detection;
    for (const auto &detection : dets) {
      if (detection.score_ > 0.5f) {
        if (detection.class_name_ == Autoproduction::Util::ClassName::person_) {
          persons_detection.push_back(
              TensorToDetection(detection.bbox_.ymin_, detection.bbox_.xmin_,
                                detection.bbox_.ymax_, detection.bbox_.xmax_,
                                detection.score_, detection.class_name_));
        } else if (detection.class_name_ ==
                   Autoproduction::Util::ClassName::sport_ball_) {
          sportballs_detection.push_back(
              TensorToDetection(detection.bbox_.ymin_, detection.bbox_.xmin_,
                                detection.bbox_.ymax_, detection.bbox_.xmax_,
                                detection.score_, detection.class_name_));
        }
      }
    }

    cc->Outputs()
        .Tag(kSportBallsDetection)
        .AddPacket(MakePacket<std::vector<::mediapipe::Detection>>(
                       sportballs_detection)
                       .At(cc->InputTimestamp()));

    cc->Outputs()
        .Tag(kPersonsDetection)
        .AddPacket(
            MakePacket<std::vector<::mediapipe::Detection>>(persons_detection)
                .At(cc->InputTimestamp()));
  }
  return ::mediapipe::OkStatus();
}
} // namespace mediapipe

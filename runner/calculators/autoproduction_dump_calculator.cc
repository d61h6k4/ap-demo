
#include "runner/calculators/autoproduction_dump_calculator.h"

#include <string>
#include <vector>

#include "mediapipe/framework/formats/detection.pb.h"
#include "mediapipe/framework/formats/rect.pb.h"
#include "mediapipe/framework/port/ret_check.h"

namespace mediapipe {
namespace {
constexpr char kRectTag[] = "RECT";
constexpr char kDetectionsTag[] = "DETECTIONS";
constexpr char kOutputFileTag[] = "OUTPUT_FILE_PATH";
constexpr char kOutputDetectionsFileTag[] = "OUTPUT_DETECTIONS_FILE_PATH";

int TimestampToFrameNum(::mediapipe::Timestamp t) {
  return static_cast<int>(t.Microseconds() / 40000.);
}
}  // namespace
REGISTER_CALCULATOR(AutoproductionDumpCalculator);

::mediapipe::Status AutoproductionDumpCalculator::GetContract(
    CalculatorContract* cc) {
  cc->Inputs().Tag(kRectTag).Set<Rect>();
  cc->InputSidePackets().Tag(kOutputFileTag).Set<std::string>();

  RET_CHECK(!(cc->Inputs().HasTag(kDetectionsTag) ^
              cc->InputSidePackets().HasTag(kOutputDetectionsFileTag)))
      << "DETECTIONS and OUTPUT_DETECTIONS_FILE_PATH shoud be setted together";

  if (cc->InputSidePackets().HasTag(kOutputDetectionsFileTag)) {
    cc->InputSidePackets().Tag(kOutputDetectionsFileTag).Set<std::string>();
    cc->Inputs().Tag(kDetectionsTag).Set<std::vector<Detection>>();
  }

  return ::mediapipe::OkStatus();
}

::mediapipe::Status AutoproductionDumpCalculator::Open(CalculatorContext* cc) {
  const auto& filename =
      cc->InputSidePackets().Tag(kOutputFileTag).Get<std::string>();
  dumper_.open(filename, std::ios::out);
  RET_CHECK(dumper_.is_open()) << "Failed to open file " << filename;
  // Write header
  dumper_ << "frame_num,x,y,w,h\n";

  if (cc->InputSidePackets().HasTag(kOutputDetectionsFileTag)) {
    const auto& dfilename =
        cc->InputSidePackets().Tag(kOutputDetectionsFileTag).Get<std::string>();
    detections_.open(dfilename, std::ios::out);
    RET_CHECK(detections_.is_open()) << "Failed to open file " << dfilename;
    // Write header
    detections_ << "frame_num,ymin,xmin,width,height,label,score\n";
  }

  return ::mediapipe::OkStatus();
}

::mediapipe::Status AutoproductionDumpCalculator::Process(
    CalculatorContext* cc) {
  if (!cc->Inputs().Tag(kRectTag).IsEmpty()) {
    const auto& rect = cc->Inputs().Tag(kRectTag).Get<Rect>();
    dumper_ << TimestampToFrameNum(cc->InputTimestamp()) << ','
            << rect.x_center() << ',' << rect.y_center() << ',' << rect.width()
            << ',' << rect.height() << '\n';
  }

  if (cc->Inputs().HasTag(kDetectionsTag) &&
      !cc->Inputs().Tag(kDetectionsTag).IsEmpty()) {
    const auto& detections =
        cc->Inputs().Tag(kDetectionsTag).Get<std::vector<Detection>>();
    for (const auto& detection : detections) {
      detections_ << TimestampToFrameNum(cc->InputTimestamp()) << ','
                  << detection.location_data().relative_bounding_box().ymin()
                  << ','
                  << detection.location_data().relative_bounding_box().xmin()
                  << ','
                  << detection.location_data().relative_bounding_box().width()
                  << ','
                  << detection.location_data().relative_bounding_box().height()
                  << ',' << detection.label().Get(0) << ','
                  << detection.score().Get(0) << '\n';
    }
  }
  return ::mediapipe::OkStatus();
}

::mediapipe::Status AutoproductionDumpCalculator::Close(CalculatorContext* cc) {
  dumper_.close();
  detections_.close();
  return ::mediapipe::OkStatus();
}
}  // namespace mediapipe

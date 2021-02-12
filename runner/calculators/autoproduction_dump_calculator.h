#pragma once

#include <fstream>

#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/port/status.h"

namespace mediapipe {
class AutoproductionDumpCalculator : public CalculatorBase {
 public:
  static ::mediapipe::Status GetContract(CalculatorContract* cc);
  ::mediapipe::Status Open(CalculatorContext* cc) override;
  ::mediapipe::Status Process(CalculatorContext* cc) override;
  ::mediapipe::Status Close(CalculatorContext* cc) override;

 private:
  std::fstream dumper_;
  std::fstream detections_;
};
}  // namespace mediapipe

load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "autoproduction_object_detection_calculator",
    srcs = ["autoproduction_object_detection_calculator.cc"],
    hdrs = ["autoproduction_object_detection_calculator.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@autoproduction//autoproduction/inference:object_detection",
        "@autoproduction//autoproduction/utils:detection",
        "@autoproduction_config_cuda//:cuda_runtime",
        "@mediapipe//mediapipe/calculators/util:detections_to_render_data_calculator",
        "@mediapipe//mediapipe/framework:calculator_framework",
        "@mediapipe//mediapipe/framework/formats:detection_cc_proto",
        "@mediapipe//mediapipe/framework/formats:image_frame",
        "@mediapipe//mediapipe/framework/formats:image_frame_opencv",
        "@mediapipe//mediapipe/framework/formats:rect_cc_proto",
        "@mediapipe//mediapipe/framework/port:ret_check",
    ],
    alwayslink = 1,
)

cc_library(
    name = "autoproduction_dump_calculator",
    srcs = ["autoproduction_dump_calculator.cc"],
    hdrs = ["autoproduction_dump_calculator.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@mediapipe//mediapipe/framework:calculator_framework",
        "@mediapipe//mediapipe/framework/formats:detection_cc_proto",
        "@mediapipe//mediapipe/framework/formats:rect_cc_proto",
        "@mediapipe//mediapipe/framework/port:ret_check",
        "@mediapipe//mediapipe/framework/port:status",
    ],
    alwayslink = 1,
)

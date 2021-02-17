load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "ap_demo",
    deps = [
        "//runner/calculators:autoproduction_dump_calculator",
        "//runner/calculators:autoproduction_object_detection_calculator",
        "@mediapipe//mediapipe/calculators/util:annotation_overlay_calculator",
        "@mediapipe//mediapipe/calculators/util:rect_to_render_data_calculator",
        "@mediapipe//mediapipe/calculators/video:opencv_video_decoder_calculator",
        "@mediapipe//mediapipe/calculators/video:opencv_video_encoder_calculator",
        "@mediapipe//mediapipe/examples/desktop:simple_run_graph_main",
    ],
)

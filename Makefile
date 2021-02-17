VIDEO_NAME = vfb_luebeck_012500_3min

build:
	bazel build --define MEDIAPIPE_DISABLE_GPU=1 //runner:ap_demo

debug: build
	GLOG_logtostderr=1 bazel-bin/runner/ap_demo \
       --calculator_graph_config_file runner/ap_demo_debug_graph.pbtxt \
       --input_side_packets=input_video_path=test_videos/$(VIDEO_NAME).mp4,output_detections_file_path=$(VIDEO_NAME)_detections.csv

render: build
	GLOG_logtostderr=1 bazel-bin/runner/ap_demo \
       --calculator_graph_config_file runner/ap_demo_graph.pbtxt \
       --input_side_packets=input_video_path=test_videos/$(VIDEO_NAME).mp4,output_video_path=processed_videos/$(VIDEO_NAME).mp4

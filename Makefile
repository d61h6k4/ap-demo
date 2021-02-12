build:
	bazel build --define MEDIAPIPE_DISABLE_GPU=1 //runner:ap_demo

debug: build
	GLOG_logtostderr=1 bazel-bin/runner/ap_ice_hockey \
       --calculator_graph_config_file runner/ap_ice_hockey_debug_graph.pbtxt \
       --input_side_packets=input_video_path=test_videos/primary_vod_pano.mp4,output_file_path=rois.csv,output_detections_file_path=detections.csv

render: build
	GLOG_logtostderr=1 bazel-bin/runner/ap_demo \
       --calculator_graph_config_file runner/ap_demo_graph.pbtxt \
       --input_side_packets=input_video_path=test_videos/mab8a08339_012500_10min.mp4,output_video_path=rois.mp4


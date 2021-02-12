To build the application you need opencv and numpy:

```
      python -m pip install --upgrade pip numpy
      sudo apt install libopencv-core-dev libopencv-highgui-dev \
                       libopencv-calib3d-dev libopencv-features2d-dev \
                       libopencv-imgproc-dev libopencv-video-dev
```
After you can build it:

```
bazel build --config=macos --define MEDIAPIPE_DISABLE_GPU=1 \
                           //runner:ap_ice_hockey
```

Run it in a way that it will output all ROIs to csv file:

```
GLOG_logtostderr=1 bazel-bin/runner/ap_ice_hockey \
       --calculator_graph_config_file runner/ap_ice_hockey_graph.pbtxt \
       --input_side_packets=input_video_path=video.mp4,output_file_path=rois.csv
```
Render ROIs on the video:

```
GLOG_logtostderr=1 bazel-bin/runner/ap_ice_hockey \
       --calculator_graph_config_file runner/ap_ice_hockey_render_graph.pbtxt \
       --input_side_packets=input_video_path=video.mp4,output_file_path=rois.mp4
```

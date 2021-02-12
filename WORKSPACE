workspace(name = "ap_demo")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("//third_party:cuda_configure.bzl", "cuda_configure")

cuda_configure(name = "autoproduction_config_cuda")

load("//third_party:tensorrt_configure.bzl", "tensorrt_configure")

tensorrt_configure(name = "autoproduction_config_tensorrt")

_MEDIAPIPE_VERSION = "0.8.0"

http_archive(
    name = "mediapipe",
    #sha256 = "0b129a28864fd2cd2822948621645efb9d62b6d043d1c4da3cf2d01886bfd901",
    strip_prefix = "mediapipe-{}".format(_MEDIAPIPE_VERSION),
    url = "https://github.com/google/mediapipe/archive/{}.tar.gz".format(_MEDIAPIPE_VERSION),
)

http_archive(
    name = "build_bazel_rules_apple",
    patch_args = [
        "-p1",
    ],
    patches = [
        # Bypass checking ios unit test runner when building MP ios applications.
        "@mediapipe//third_party:build_bazel_rules_apple_bypass_test_runner_check.diff",
    ],
    sha256 = "7a7afdd4869bb201c9352eed2daf37294d42b093579b70423490c1b4d4f6ce42",
    url = "https://github.com/bazelbuild/rules_apple/releases/download/0.19.0/rules_apple.0.19.0.tar.gz",
)

load(
    "@build_bazel_rules_apple//apple:repositories.bzl",
    "apple_rules_dependencies",
)

apple_rules_dependencies()

http_archive(
    name = "rules_foreign_cc",
    strip_prefix = "rules_foreign_cc-master",
    url = "https://github.com/bazelbuild/rules_foreign_cc/archive/master.zip",
)

load("@rules_foreign_cc//:workspace_definitions.bzl", "rules_foreign_cc_dependencies")

rules_foreign_cc_dependencies()

# This is used to select all contents of the archives for CMake-based packages to give CMake access to them.
all_content = """filegroup(name = "all", srcs = glob(["**"]), visibility = ["//visibility:public"])"""

new_local_repository(
    name = "linux_opencv",
    build_file = "@//third_party:opencv_linux.BUILD",
    path = "/usr/local",
)

new_local_repository(
    name = "linux_ffmpeg",
    build_file = "@mediapipe//third_party:ffmpeg_linux.BUILD",
    path = "/usr",
)

# gflags needed by glog
http_archive(
    name = "com_github_gflags_gflags",
    sha256 = "19713a36c9f32b33df59d1c79b4958434cb005b5b47dc5400a7a4b078111d9b5",
    strip_prefix = "gflags-2.2.2",
    url = "https://github.com/gflags/gflags/archive/v2.2.2.zip",
)

# 2020-08-21
http_archive(
    name = "com_github_glog_glog",
    sha256 = "58c9b3b6aaa4dd8b836c0fd8f65d0f941441fb95e27212c5eeb9979cfd3592ab",
    strip_prefix = "glog-0a2e5931bd5ff22fd3bf8999eb8ce776f159cda6",
    urls = [
        "https://github.com/google/glog/archive/0a2e5931bd5ff22fd3bf8999eb8ce776f159cda6.zip",
    ],
)

http_archive(
    name = "com_github_glog_glog_no_gflags",
    build_file = "@mediapipe//third_party:glog_no_gflags.BUILD",
    patch_args = [
        "-p1",
    ],
    patches = [
        "@mediapipe//third_party:com_github_glog_glog_9779e5ea6ef59562b030248947f787d1256132ae.diff",
    ],
    sha256 = "58c9b3b6aaa4dd8b836c0fd8f65d0f941441fb95e27212c5eeb9979cfd3592ab",
    strip_prefix = "glog-0a2e5931bd5ff22fd3bf8999eb8ce776f159cda6",
    urls = [
        "https://github.com/google/glog/archive/0a2e5931bd5ff22fd3bf8999eb8ce776f159cda6.zip",
    ],
)

git_repository(
    name = "autoproduction",
    branch = "main",
    remote = "git@github.com:d61h6k4/autoproduction.git",
)

_BAZEL_TOOLCHAINS_VERSION = "4.0.0"

http_archive(
    name = "bazel_toolchains",
    #    sha256 = "4fb3ceea08101ec41208e3df9e56ec72b69f3d11c56629d6477c0ff88d711cf7",
    strip_prefix = "bazel-toolchains-{}".format(_BAZEL_TOOLCHAINS_VERSION),
    urls = [
        "https://github.com/bazelbuild/bazel-toolchains/releases/download/{}/bazel-toolchains-{}.tar.gz".format(_BAZEL_TOOLCHAINS_VERSION, _BAZEL_TOOLCHAINS_VERSION),
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-toolchains/releases/download/{}/bazel-toolchains-{}.tar.gz".format(_BAZEL_TOOLCHAINS_VERSION, _BAZEL_TOOLCHAINS_VERSION),
    ],
)

# Build against Kythe master.  Run `bazel sync` to update to the latest commit.
http_archive(
    name = "io_kythe",
    strip_prefix = "kythe-master",
    urls = ["https://github.com/google/kythe/archive/master.zip"],
)

load("@io_kythe//:setup.bzl", "kythe_rule_repositories", "maybe")

kythe_rule_repositories()

# TODO(d61h6k4): remove this, when kythe will resolve it.
# This needs to be loaded before loading the
# go_* rules.  Normally, this is done by go_rules_dependencies in external.bzl,
# but because we want to overload some of those dependencies, we need the go_*
# rules before go_rules_dependencies.  Likewise, we can't precisely control
# when loads occur within a Starlark file so we now need to load this
# manually...
load("@io_bazel_rules_go//go/private:repositories.bzl", "go_name_hack")

maybe(
    go_name_hack,
    name = "io_bazel_rules_go_name_hack",
    is_rules_go = False,
)

load("@io_kythe//:external.bzl", "kythe_dependencies")

kythe_dependencies()

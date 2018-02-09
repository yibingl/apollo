load("//tools:cpplint.bzl", "cpplint")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "smartereye_gflags",
    srcs = [
        "smartereye_gflags.cc",
    ],
    hdrs = [
        "smartereye_gflags.h",
    ],
    deps = [
        "//external:gflags",
    ],
)

cc_library(
    name = "smartereye_camera_lib",
    srcs = [
        "smartereye_camera.cc",
        "camera_device.cc",
    ],
    hdrs = [
        "smartereye_camera.h",
        "camera_device.h",
    ],
    deps = [
        "smartereye_gflags",
        "//modules/drivers/smartereye/proto:smartereye_proto",
        "//modules/common",
        "//modules/common:apollo_app",
        "//modules/common/adapters:adapter_manager",
        "//modules/common/monitor_log",
        "//modules/common/time",
        "//modules/common/util:json_util",
        "//third_party/json",
        "//third_party/smartereye:smartereye_sdk_lib",
        "@ros//:ros_common",
    ],
)

cc_binary(
    name = "smartereye",
    srcs = ["main.cc"],
    deps = [
        ":smartereye_camera_lib",
        "//external:gflags",
        "//modules/common:log",
        "//modules/common/monitor_log",
        "//third_party/smartereye:smartereye_sdk_lib",
        "@ros//:ros_common",
    ],
)

cpplint()

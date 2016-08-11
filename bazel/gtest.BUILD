package(default_visibility = ["//visibility:public"])

licenses(["notice"])

cc_library(
    name = "main",
    srcs = glob(
        ["googletest/src/*.cc"],
        exclude = ["googletest/src/gtest-all.cc"]
    ),
    hdrs = glob([
        "googletest/include/**/*.h",
        "googletest/src/*.h"
    ]),
    includes = ["googletest/include", "googletest"],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
)


cc_library(
    name = "laminate",
    srcs = glob(["src/*.cc"]),
    hdrs = glob(["include/*.h"]),
    includes = ['include'],
    visibility = ["//visibility:public"],
    deps = ["@protobuf//:protobuf",
            "@blosc//:blosc"]
)

cc_test(
    name = "laminate_test",
    srcs = glob(["test/*.cc"]),
    deps = [":laminate", "@gtest//:main"],
    size = "small",
)

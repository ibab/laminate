
cc_library(
    name = "laminate",
    srcs = glob(["src/*.cc"]),
    hdrs = glob(["include/*.h"]),
    copt = ["-std=c++11"],
    includes = ["include"],
    visibility = ["//visibility:public"],
    deps = ["@protobuf//:protobuf",
            "@blosc//:blosc"]
)


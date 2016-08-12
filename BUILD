
cc_library(
    name = "laminate",
    srcs = glob(["src/*.cc"]),
    hdrs = glob(["src/*.h"]),
    copts = ["-std=c++11"],
    includes = ["src"],
    visibility = ["//visibility:public"],
    deps = ["@protobuf//:protobuf",
            "@blosc//:blosc",
            "@hdf5//:hdf5_cpp"]
)


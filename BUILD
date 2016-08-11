
cc_library(
    name = "laminate",
    srcs = glob(["src/*.cc"]),
    hdrs = glob(["include/*.h"]),
    copts = ["-std=c++11"],
    includes = ["include"],
    visibility = ["//visibility:public"],
    deps = ["@protobuf//:protobuf",
            "@blosc//:blosc",
            "@hdf5//:hdf5_cpp"]
)


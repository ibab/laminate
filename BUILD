load('//:bazel/compilation_database.bzl', 'compilation_database')

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

compilation_database(
    name='compilation_database',
    targets=['//:laminate',
             '//examples:column_store',
             '//examples:row_store',
             '//test:test_hdf_stream'],
)

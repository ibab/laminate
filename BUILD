
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
    name = "test_column_store",
    srcs = glob(["test/test_column_store.cc"]),
    deps = [":laminate", "@gtest//:main"],
    size = "small",
)

cc_test(
    name = "test_row_store",
    srcs = glob(["test/test_row_store.cc"]),
    deps = [":laminate", "@gtest//:main"],
    size = "small",
)

load('@protobuf//:protobuf.bzl', 'cc_proto_library')

cc_proto_library(
    name = "example_protobuf",
    srcs =["examples/main.proto"],
    cc_libs = ["@protobuf//:protobuf"],
    protoc = "@protobuf//:protoc",
    default_runtime = "@protobuf//:protobuf",
)

cc_binary(
    name = "column_store",
    srcs = ["examples/column_store.cc"],
    deps = [":laminate",
            "example_protobuf"],
)

cc_binary(
    name = "row_store",
    srcs = ["examples/row_store.cc"],
    deps = [":laminate",
            "example_protobuf"],
)

cc_binary(
    name = "blosc_example",
    srcs = ["examples/blosc.cc"],
    deps = [":laminate"],
)



load('@protobuf//:protobuf.bzl', 'cc_proto_library')

cc_proto_library(
    name = "example_protobuf",
    srcs =["example.proto"],
    cc_libs = ["@protobuf//:protobuf"],
    protoc = "@protobuf//:protoc",
    default_runtime = "@protobuf//:protobuf",
)

cc_binary(
    name = "column_store",
    srcs = ["column_store.cc"],
    copts = ["-std=c++11"],
    deps = ["//:laminate",
            "example_protobuf"],
    visibility = ['//visibility:public'],
)

cc_binary(
    name = "row_store",
    srcs = ["row_store.cc"],
    copts = ["-std=c++11"],
    deps = ["//:laminate",
            "example_protobuf"],
    visibility = ['//visibility:public'],
)

cc_binary(
    name = "blosc_example",
    srcs = ["blosc.cc"],
    copts = ["-std=c++11"],
    deps = ["//:laminate"],
    visibility = ['//visibility:public'],
)


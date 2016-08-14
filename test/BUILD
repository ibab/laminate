cc_test(
    name = "test_hdf_stream",
    srcs = ["test_hdf_stream.cc"],
    copts = ["-std=c++11"],
    deps = ["//:laminate", "@gtest//:main", "@boost//:filesystem"],
    size = "small",
    visibility = ['//visibility:public'],
)

cc_test(
    name = "test_column_store",
    srcs = ["test_column_store.cc"],
    copts = ["-std=c++11"],
    deps = ["//:laminate", "@gtest//:main"],
    size = "small",
)

cc_test(
    name = "test_row_store",
    srcs = ["test_row_store.cc"],
    copts = ["-std=c++11"],
    deps = ["//:laminate", "@gtest//:main"],
    size = "small",
)



cc_test(
    name = "test_column_store",
    srcs = glob(["test/test_column_store.cc"]),
    copts = ["-std=c++11"],
    deps = ["//:laminate", "@gtest//:main"],
    size = "small",
)

cc_test(
    name = "test_row_store",
    srcs = glob(["test/test_row_store.cc"]),
    copts = ["-std=c++11"],
    deps = ["//:laminate", "@gtest//:main"],
    size = "small",
)


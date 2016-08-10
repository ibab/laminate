package(default_visibility = ["//visibility:public"])

licenses(["notice"])

cc_library(
  name="blosc",
  srcs=glob(["blosc/*.c"], exclude=["blosc/*-avx2.c"]),
  hdrs=glob(["blosc/*.h"], exclude=["blosc/*-avx2.h"]),
  includes=["blosc/"],
  copts=["-msse2"],
  defines=["HAVE_ZLIB"],
  deps=["@zlib//:zlib"],
)


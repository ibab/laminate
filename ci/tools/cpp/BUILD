filegroup(
    name = "empty",
    srcs = [],
)


filegroup(
    name = "toolchain",
    srcs = [":cc-compiler-local",
            ":empty"],
)

cc_toolchain(
    name = "cc-compiler-local",
    all_files = ":empty",
    compiler_files = ":empty",
    cpu = "local",
    dwp_files = ":empty",
    dynamic_runtime_libs = [":empty"],
    linker_files = ":empty",
    objcopy_files = ":empty",
    static_runtime_libs = [":empty"],
    strip_files = ":empty",
    supports_param_files = 1,
    visibility=["//visibility:public"],
)

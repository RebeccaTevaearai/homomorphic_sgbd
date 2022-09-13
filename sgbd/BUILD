load("//transpiler:fhe.bzl", "fhe_cc_library")
load("@bazel_skylib//:bzl_library.bzl", "bzl_library")

fhe_cc_library(
    name = "api_tfhe_and",
    src = "api/api_and.cc",
    hdrs = ["api/api_and.h"],
    encryption = "tfhe",
    num_opt_passes = 2,
    optimizer = "xls",
)

fhe_cc_library(
    name = "api_tfhe_equality_int",
    src = "api/api_equality_int.cc",
    hdrs = ["api/api_equality_int.h"],
    encryption = "tfhe",
    num_opt_passes = 2,
    optimizer = "xls",
)

fhe_cc_library(
    name = "api_tfhe_equality_char",
    src = "api/api_equality_char.cc",
    hdrs = ["api/api_equality_char.h"],
    encryption = "tfhe",
    num_opt_passes = 2,
    optimizer = "xls",
)

cc_library(
    name = "server",
    srcs = [],
    hdrs = ["server.h"],
    deps = [
        ":api_tfhe_and",
        ":api_tfhe_equality_int",
        ":api_tfhe_equality_char",
        ":utils",
        "//transpiler/data:tfhe_data",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/types:span",
        "@tfhe//:libtfhe",
    ],
)

cc_library(
    name = "utils",
    hdrs = ["utils.h"],
    deps = [
        "//transpiler/data:tfhe_data",
    ],
)

cc_binary(
    name = "sgbd",
    srcs = [
        "api/api_and.h",
        "api/api_equality_int.h",
        "api/api_equality_char.h",
        "main.cc",
        "server.h",
        "utils.h",
    ],
    deps = [
        ":api_tfhe_and",
        ":api_tfhe_equality_int",
        ":api_tfhe_equality_char",
        ":server",
        ":utils",
        "//transpiler/data:tfhe_data",
        "@com_google_absl//absl/base",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@tfhe//:libtfhe",
    ],
)
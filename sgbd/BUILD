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
    name = "api_tfhe_sum",
    src = "api/api_sum.cc",
    hdrs = ["api/api_sum.h"],
    encryption = "tfhe",
    num_opt_passes = 2,
    optimizer = "xls",
)

fhe_cc_library(
    name = "api_tfhe_div",
    src = "api/api_div.cc",
    hdrs = ["api/api_div.h"],
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

fhe_cc_library(
    name = "api_tfhe_inequality_int",
    src = "api/api_inequality_int.cc",
    hdrs = ["api/api_inequality_int.h"],
    encryption = "tfhe",
    num_opt_passes = 2,
    optimizer = "xls",
)

fhe_cc_library(
    name = "api_tfhe_superior_int",
    src = "api/api_superior_int.cc",
    hdrs = ["api/api_superior_int.h"],
    encryption = "tfhe",
    num_opt_passes = 2,
    optimizer = "xls",
)

fhe_cc_library(
    name = "api_tfhe_inferior_int",
    src = "api/api_inferior_int.cc",
    hdrs = ["api/api_inferior_int.h"],
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
        ":api_tfhe_sum",
        ":api_tfhe_div",
        ":api_tfhe_equality_int",
        ":api_tfhe_equality_char",
        ":api_tfhe_inequality_int",
        ":api_tfhe_superior_int",
        ":api_tfhe_inferior_int",
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
        "api/api_sum.h",
        "api/api_div.h",
        "api/api_equality_int.h",
        "api/api_equality_char.h",
        "api/api_inequality_int.h",
        "api/api_superior_int.h",
        "api/api_inferior_int.h",
        "main.cc",
        "server.h",
        "utils.h",
    ],
    deps = [
        ":api_tfhe_and",
        ":api_tfhe_sum",
        ":api_tfhe_div",
        ":api_tfhe_equality_int",
        ":api_tfhe_equality_char",
        ":api_tfhe_inequality_int",
        ":api_tfhe_superior_int",
        ":api_tfhe_inferior_int",
        ":server",
        ":utils",
        "//transpiler/data:tfhe_data",
        "@com_google_absl//absl/base",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@tfhe//:libtfhe",
    ],
)

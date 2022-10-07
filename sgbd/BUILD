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
    name = "api_tfhe_and_short",
    src = "api/api_and_short.cc",
    hdrs = ["api/api_and_short.h"],
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
    name = "api_tfhe_sum_short",
    src = "api/api_sum_short.cc",
    hdrs = ["api/api_sum_short.h"],
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
    name = "api_tfhe_div_short",
    src = "api/api_div_short.cc",
    hdrs = ["api/api_div_short.h"],
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
    name = "api_tfhe_equality_short",
    src = "api/api_equality_short.cc",
    hdrs = ["api/api_equality_short.h"],
    encryption = "tfhe",
    num_opt_passes = 2,
    optimizer = "xls",
)

fhe_cc_library(
    name = "api_tfhe_product",
    src = "api/api_product.cc",
    hdrs = ["api/api_product.h"],
    encryption = "tfhe",
    num_opt_passes = 2,
    optimizer = "xls",
)

fhe_cc_library(
    name = "api_tfhe_product_short",
    src = "api/api_product_short.cc",
    hdrs = ["api/api_product_short.h"],
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
    name = "api_tfhe_inequality_short",
    src = "api/api_inequality_short.cc",
    hdrs = ["api/api_inequality_short.h"],
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
    name = "api_tfhe_superior_short",
    src = "api/api_superior_short.cc",
    hdrs = ["api/api_superior_short.h"],
    encryption = "tfhe",
    num_opt_passes = 2,
    optimizer = "xls",
)

fhe_cc_library(
    name = "api_tfhe_superior_or_equal_int",
    src = "api/api_superior_or_equal_int.cc",
    hdrs = ["api/api_superior_or_equal_int.h"],
    encryption = "tfhe",
    num_opt_passes = 2,
    optimizer = "xls",
)

fhe_cc_library(
    name = "api_tfhe_superior_or_equal_short",
    src = "api/api_superior_or_equal_short.cc",
    hdrs = ["api/api_superior_or_equal_short.h"],
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

fhe_cc_library(
    name = "api_tfhe_inferior_short",
    src = "api/api_inferior_short.cc",
    hdrs = ["api/api_inferior_short.h"],
    encryption = "tfhe",
    num_opt_passes = 2,
    optimizer = "xls",
)

fhe_cc_library(
    name = "api_tfhe_inferior_or_equal_int",
    src = "api/api_inferior_or_equal_int.cc",
    hdrs = ["api/api_inferior_or_equal_int.h"],
    encryption = "tfhe",
    num_opt_passes = 2,
    optimizer = "xls",
)

fhe_cc_library(
    name = "api_tfhe_inferior_or_equal_short",
    src = "api/api_inferior_or_equal_short.cc",
    hdrs = ["api/api_inferior_or_equal_short.h"],
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
        ":api_tfhe_product",
        ":api_tfhe_inequality_int",
        ":api_tfhe_superior_int",
        ":api_tfhe_superior_or_equal_int",
        ":api_tfhe_inferior_int",
        ":api_tfhe_inferior_or_equal_int",
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

cc_library(
    name = "queries",
    hdrs = ["queries.h"],
    deps = [
        ":utils",
        ":server",
        "//transpiler/data:tfhe_data",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/types:span",
        "@tfhe//:libtfhe",
    ],
)

cc_binary(
    name = "sgbd",
    srcs = [
        "main.cc",
        "server.h",
        "utils.h",
        "queries.h",
    ],
    deps = [
        ":server",
        ":utils",
        ":queries",
        "//transpiler/data:tfhe_data",
        "@com_google_absl//absl/base",
        "@com_google_xls//xls/common/logging",
        "@com_google_xls//xls/common/status:status_macros",
        "@tfhe//:libtfhe",
    ],
)

cc_test(
    name = "performance_test",
    srcs = ["benchtest/performance_test.cc"],
    deps = [
        ":server",
        ":queries",
        ":utils",
        "//transpiler/data:tfhe_data",
        "@com_google_googletest//:gtest_main",
        "@com_google_xls//xls/common/status:matchers",
    ],
)

cc_test(
    name = "performance_test_big",
    srcs = ["benchtest/performance_test_big.cc"],
    deps = [
        ":server",
        ":queries",
        ":utils",
        "//transpiler/data:tfhe_data",
        "@com_google_googletest//:gtest_main",
        "@com_google_xls//xls/common/status:matchers",
    ],
)

cc_test(
    name = "performance_test_verybig",
    srcs = ["benchtest/performance_test_verybig.cc"],
    deps = [
        ":server",
        ":queries",
        ":utils",
        "//transpiler/data:tfhe_data",
        "@com_google_googletest//:gtest_main",
        "@com_google_xls//xls/common/status:matchers",
    ],
)

cc_test(
    name = "api_test",
    srcs = ["benchtest/api_test.cc"],
    deps = [
        ":api_tfhe_and",
        ":api_tfhe_and_short",
        ":api_tfhe_sum",
        ":api_tfhe_sum_short",
        ":api_tfhe_div",
        ":api_tfhe_div_short",
        ":api_tfhe_equality_int",
        ":api_tfhe_equality_short",
        ":api_tfhe_product",
        ":api_tfhe_product_short",
        ":api_tfhe_inequality_int",
        ":api_tfhe_inequality_short",
        ":api_tfhe_superior_int",
        ":api_tfhe_superior_short",
        ":api_tfhe_superior_or_equal_int",
        ":api_tfhe_superior_or_equal_short",
        ":api_tfhe_inferior_int",
        ":api_tfhe_inferior_short",
        ":api_tfhe_inferior_or_equal_int",
        ":api_tfhe_inferior_or_equal_short",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/types:span",
        "@tfhe//:libtfhe",
        "//transpiler/data:tfhe_data",
        "@com_google_googletest//:gtest_main",
        "@com_google_xls//xls/common/status:matchers",
    ],
)

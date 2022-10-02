/*
#include "transpiler/examples/sgbd/api/api_and.h"
#include "transpiler/examples/sgbd/api/api_and_short.h"
#include "transpiler/examples/sgbd/api/api_sum.h"
#include "transpiler/examples/sgbd/api/api_sum_short.h"
#include "transpiler/examples/sgbd/api/api_div.h"
#include "transpiler/examples/sgbd/api/api_div_short.h"
#include "transpiler/examples/sgbd/api/api_equality_int.h"
#include "transpiler/examples/sgbd/api/api_equality_short.h"
#include "transpiler/examples/sgbd/api/api_product.h"
#include "transpiler/examples/sgbd/api/api_product_short.h"
#include "transpiler/examples/sgbd/api/api_inequality_int.h"
#include "transpiler/examples/sgbd/api/api_inequality_short.h"
#include "transpiler/examples/sgbd/api/api_superior_int.h"
#include "transpiler/examples/sgbd/api/api_superior_short.h"
#include "transpiler/examples/sgbd/api/api_superior_or_equal_int.h"
#include "transpiler/examples/sgbd/api/api_superior_or_equal_short.h"
#include "transpiler/examples/sgbd/api/api_inferior_int.h"
#include "transpiler/examples/sgbd/api/api_inferior_short.h"
#include "transpiler/examples/sgbd/api/api_inferior_or_equal_int.h"
#include "transpiler/examples/sgbd/api/api_inferior_or_equal_short.h"
*/
#include "transpiler/examples/sgbd_cleartext/api_tfhe_and.h"
#include "transpiler/examples/sgbd_cleartext/api_tfhe_and_short.h"
#include "transpiler/examples/sgbd_cleartext/api_tfhe_sum.h"
#include "transpiler/examples/sgbd_cleartext/api_tfhe_sum_short.h"
#include "transpiler/examples/sgbd_cleartext/api_tfhe_div.h"
#include "transpiler/examples/sgbd_cleartext/api_tfhe_div_short.h"
#include "transpiler/examples/sgbd_cleartext/api_tfhe_equality_int.h"
#include "transpiler/examples/sgbd_cleartext/api_tfhe_equality_short.h"
#include "transpiler/examples/sgbd_cleartext/api_tfhe_product.h"
#include "transpiler/examples/sgbd_cleartext/api_tfhe_product_short.h"
#include "transpiler/examples/sgbd_cleartext/api_tfhe_inequality_int.h"
#include "transpiler/examples/sgbd_cleartext/api_tfhe_inequality_short.h"
#include "transpiler/examples/sgbd_cleartext/api_tfhe_superior_int.h"
#include "transpiler/examples/sgbd_cleartext/api_tfhe_superior_short.h"
#include "transpiler/examples/sgbd_cleartext/api_tfhe_superior_or_equal_int.h"
#include "transpiler/examples/sgbd_cleartext/api_tfhe_superior_or_equal_short.h"
#include "transpiler/examples/sgbd_cleartext/api_tfhe_inferior_int.h"
#include "transpiler/examples/sgbd_cleartext/api_tfhe_inferior_short.h"
#include "transpiler/examples/sgbd_cleartext/api_tfhe_inferior_or_equal_int.h"
#include "transpiler/examples/sgbd_cleartext/api_tfhe_inferior_or_equal_short.h"

#include "gtest/gtest.h"
#include "transpiler/data/cleartext_data.h"


TEST(ApiTest, AND) {

    int arg1 = 1;
    int arg2 = 0;

    Encoded<int> cipher_arg1(arg1);
    Encoded<int> cipher_arg2(arg2);

    Encoded<int> cipher_result;

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    XLS_CHECK_OK(tfhe_and(cipher_result, cipher_arg1, cipher_arg2));

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();

    EXPECT_EQ(result, 0);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;

}

TEST(ApiTest, AND_SHORT) {

    short arg1 = 1;
    short arg2 = 0;

    Encoded<short> cipher_arg1(arg1);
    Encoded<short> cipher_arg2(arg2);

    Encoded<short> cipher_result;

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    XLS_CHECK_OK(tfhe_and_short(cipher_result, cipher_arg1, cipher_arg2));

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();

    EXPECT_EQ(result, 0);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;

}


TEST(ApiTest, DIV) {

    int arg1 = 10;
    int arg2 = 5;

    Encoded<int> cipher_arg1(arg1);
    Encoded<int> cipher_arg2(arg2);

    Encoded<int> cipher_result;

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    XLS_CHECK_OK(tfhe_div(cipher_result, cipher_arg1, cipher_arg2));

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();

    EXPECT_EQ(result, 2);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;

}

TEST(ApiTest, DIV_SHORT) {

    short arg1 = 10;
    short arg2 = 5;

    Encoded<short> cipher_arg1(arg1);
    Encoded<short> cipher_arg2(arg2);

    Encoded<short> cipher_result;;

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    XLS_CHECK_OK(tfhe_div_short(cipher_result, cipher_arg1, cipher_arg2));

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();

    EXPECT_EQ(result, 2);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;

}

TEST(ApiTest, EQUALITY) {

    int arg1 = 10;
    int arg2 = 10;

    Encoded<int> cipher_arg1(arg1);
    Encoded<int> cipher_arg2(arg2);

    Encoded<int> cipher_result;

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    XLS_CHECK_OK(equality_int(cipher_result, cipher_arg1, cipher_arg2));

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();

    EXPECT_EQ(result, 1);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;

}


TEST(ApiTest, EQUALITY_SHORT) {

    short arg1 = 10;
    short arg2 = 10;

    Encoded<short> cipher_arg1(arg1);
    Encoded<short> cipher_arg2(arg2);

    Encoded<short> cipher_result;

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    XLS_CHECK_OK(equality_short(cipher_result, cipher_arg1, cipher_arg2));

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();

    EXPECT_EQ(result, 1);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;

}

TEST(ApiTest, INEQUALITY) {

    int arg1 = 10;
    int arg2 = 8;

    Encoded<int> cipher_arg1(arg1);
    Encoded<int> cipher_arg2(arg2);

    Encoded<int> cipher_result;

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    XLS_CHECK_OK(inequality_int(cipher_result, cipher_arg1, cipher_arg2));

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();

    EXPECT_EQ(result, 1);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;

}

TEST(ApiTest, INEQUALITY_SHORT) {

    short arg1 = 10;
    short arg2 = 8;

    Encoded<short> cipher_arg1(arg1);
    Encoded<short> cipher_arg2(arg2);

    Encoded<short> cipher_result;

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    XLS_CHECK_OK(inequality_short(cipher_result, cipher_arg1, cipher_arg2));

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();

    EXPECT_EQ(result, 1);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;

}

TEST(ApiTest, INFERIOR) {

    int arg1 = 8;
    int arg2 = 10;

    Encoded<int> cipher_arg1(arg1);
    Encoded<int> cipher_arg2(arg2);

    Encoded<int> cipher_result;

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    XLS_CHECK_OK(inferior_int(cipher_result, cipher_arg1, cipher_arg2));

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();

    EXPECT_EQ(result, 1);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;

}

TEST(ApiTest, INFERIOR_SHORT) {

    short arg1 = 8;
    short arg2 = 10;

    Encoded<short> cipher_arg1(arg1);
    Encoded<short> cipher_arg2(arg2);

    Encoded<short> cipher_result;

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    XLS_CHECK_OK(inferior_short(cipher_result, cipher_arg1, cipher_arg2));

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();

    EXPECT_EQ(result, 1);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;

}


TEST(ApiTest, INFERIOR_OR_EQUAL) {

    int arg1 = 8;
    int arg2 = 8;

    Encoded<int> cipher_arg1(arg1);
    Encoded<int> cipher_arg2(arg2);

    Encoded<int> cipher_result;

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    XLS_CHECK_OK(inferior_or_equal_int(cipher_result, cipher_arg1, cipher_arg2));

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();

    EXPECT_EQ(result, 1);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;

}

TEST(ApiTest, INFERIOR_OR_EQUAL_SHORT) {

    short arg1 = 8;
    short arg2 = 8;

    Encoded<short> cipher_arg1(arg1);
    Encoded<short> cipher_arg2(arg2);

    Encoded<short> cipher_result;

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    XLS_CHECK_OK(inferior_or_equal_short(cipher_result, cipher_arg1, cipher_arg2));

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();

    EXPECT_EQ(result, 1);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;

}

TEST(ApiTest, PRODUCT_SHORT) {

    short arg1 = 10;
    short arg2 = 5;

    Encoded<short> cipher_arg1(arg1);
    Encoded<short> cipher_arg2(arg2);

    Encoded<short> cipher_result;

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    XLS_CHECK_OK(tfhe_product_short(cipher_result, cipher_arg1, cipher_arg2));

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();

    EXPECT_EQ(result, 50);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;

}


TEST(ApiTest, PRODUCT) {

    int arg1 = 10;
    int arg2 = 5;

    Encoded<int> cipher_arg1(arg1);
    Encoded<int> cipher_arg2(arg2);

    Encoded<int> cipher_result;

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    XLS_CHECK_OK(tfhe_product(cipher_result, cipher_arg1, cipher_arg2));

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();

    EXPECT_EQ(result, 50);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;

}

TEST(ApiTest, SUM) {

    int arg1 = 10;
    int arg2 = 8;

    Encoded<int> cipher_arg1(arg1);
    Encoded<int> cipher_arg2(arg2);

    Encoded<int> cipher_result;

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    XLS_CHECK_OK(tfhe_sum(cipher_result, cipher_arg1, cipher_arg2));

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();

    EXPECT_EQ(result, 18);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;

}

TEST(ApiTest, SUM_SHORT) {

    short arg1 = 10;
    short arg2 = 8;

    Encoded<short> cipher_arg1(arg1);
    Encoded<short> cipher_arg2(arg2);

    Encoded<short> cipher_result;

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    XLS_CHECK_OK(tfhe_sum_short(cipher_result, cipher_arg1, cipher_arg2));

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();

    EXPECT_EQ(result, 18);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;

}

TEST(ApiTest, SUPERIOR) {

    int arg1 = 10;
    int arg2 = 8;

    Encoded<int> cipher_arg1(arg1);
    Encoded<int> cipher_arg2(arg2);

    Encoded<int> cipher_result;

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    XLS_CHECK_OK(superior_int(cipher_result, cipher_arg1, cipher_arg2));

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();

    EXPECT_EQ(result, 1);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;

}

TEST(ApiTest, SUPERIOR_SHORT) {

    short arg1 = 10;
    short arg2 = 8;

    Encoded<short> cipher_arg1(arg1);
    Encoded<short> cipher_arg2(arg2);

    Encoded<short> cipher_result;

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    XLS_CHECK_OK(superior_short(cipher_result, cipher_arg1, cipher_arg2));

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();

    EXPECT_EQ(result, 1);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;

}

TEST(ApiTest, SUPERIOR_OR_EQUAL) {

    int arg1 = 8;
    int arg2 = 8;

    Encoded<int> cipher_arg1(arg1);
    Encoded<int> cipher_arg2(arg2);

    Encoded<int> cipher_result;

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    XLS_CHECK_OK(superior_or_equal_int(cipher_result, cipher_arg1, cipher_arg2));

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();

    EXPECT_EQ(result, 1);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;

}

TEST(ApiTest, SUPERIOR_OR_EQUAL_SHORT) {

    short arg1 = 8;
    short arg2 = 8;

    Encoded<short> cipher_arg1(arg1);
    Encoded<short> cipher_arg2(arg2);

    Encoded<short> cipher_result;

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    XLS_CHECK_OK(superior_or_equal_short(cipher_result, cipher_arg1, cipher_arg2));

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();

    EXPECT_EQ(result, 1);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;

}


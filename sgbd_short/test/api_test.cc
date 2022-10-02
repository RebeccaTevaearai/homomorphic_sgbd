#include "transpiler/examples/sgbd/api/api_and.h"
#include "transpiler/examples/sgbd/api/api_sum.h"
#include "transpiler/examples/sgbd/api/api_div.h"
#include "transpiler/examples/sgbd/api/api_equality_int.h"
#include "transpiler/examples/sgbd/api/api_product.h"
#include "transpiler/examples/sgbd/api/api_inequality_int.h"
#include "transpiler/examples/sgbd/api/api_superior_int.h"
#include "transpiler/examples/sgbd/api/api_inferior_int.h"

#include "transpiler/examples/sgbd/api_tfhe_and.h"
#include "transpiler/examples/sgbd/api_tfhe_sum.h"
#include "transpiler/examples/sgbd/api_tfhe_div.h"
#include "transpiler/examples/sgbd/api_tfhe_equality_int.h"
#include "transpiler/examples/sgbd/api_tfhe_product.h"
#include "transpiler/examples/sgbd/api_tfhe_inequality_int.h"
#include "transpiler/examples/sgbd/api_tfhe_superior_int.h"
#include "transpiler/examples/sgbd/api_tfhe_inferior_int.h"

#include "gtest/gtest.h"
#include "transpiler/data/tfhe_data.h"

constexpr int kMainMinimumLambda = 120;
constexpr std::array<uint32_t, 3> kSeed = {314, 1592, 657};

TFHEParameters params(kMainMinimumLambda);
TFHESecretKeySet key(params, kSeed);

TEST(ApiTest, AND) {

    int arg1 = 1;
    int arg2 = 0;
    int arg3 = 22;

    auto cipher_arg1 = Tfhe<int>::Encrypt(arg1, key);
    auto cipher_arg2 = Tfhe<int>::Encrypt(arg2, key);
    auto cipher_arg3 = Tfhe<int>::Encrypt(arg3, key);

    Tfhe<int> cipher_result(params);

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    XLS_CHECK_OK(tfhe_and(cipher_result, cipher_arg1, cipher_arg2, key.cloud()));

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decrypt(key);

    EXPECT_EQ(result, 0);

    XLS_CHECK_OK(tfhe_and(cipher_result, cipher_arg1, cipher_arg1, key.cloud()));

    result = cipher_result.Decrypt(key);
    EXPECT_EQ(result, 1);

    XLS_CHECK_OK(tfhe_and(cipher_result, cipher_arg2, cipher_arg3, key.cloud()));
    result = cipher_result.Decrypt(key);
    EXPECT_EQ(result, 0);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;
}

TEST(ApiTest, AND) {

    int arg1 = 1;
    int arg2 = 0;
    int arg3 = 22;

    auto cipher_arg1 = Tfhe<int>::Encrypt(arg1, key);
    auto cipher_arg2 = Tfhe<int>::Encrypt(arg2, key);
    auto cipher_arg3 = Tfhe<int>::Encrypt(arg3, key);

    Tfhe<int> cipher_result(params);

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    XLS_CHECK_OK(tfhe_and(cipher_result, cipher_arg1, cipher_arg2, key.cloud()));

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decrypt(key);

    EXPECT_EQ(result, 0);

    XLS_CHECK_OK(tfhe_and(cipher_result, cipher_arg1, cipher_arg1, key.cloud()));

    result = cipher_result.Decrypt(key);
    EXPECT_EQ(result, 1);

    XLS_CHECK_OK(tfhe_and(cipher_result, cipher_arg2, cipher_arg3, key.cloud()));
    result = cipher_result.Decrypt(key);
    EXPECT_EQ(result, 0);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;
}
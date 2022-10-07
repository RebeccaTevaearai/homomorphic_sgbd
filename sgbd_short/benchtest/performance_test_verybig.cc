/*
*   File name: performance_test_verybig.cc
*   
*   Description: 
*   Test of performance for all the type of queries for n = 20 (n: number of entries in the table). 
*
*
*   Author: Rébecca Tevaearai
*   Date: October 2022
*/

#include "transpiler/examples/sgbd_short/utils.h"
#include "transpiler/examples/sgbd_short/queries.h"
#include "transpiler/examples/sgbd_short/server.h"

#include "transpiler/data/tfhe_data.h"
#include "gtest/gtest.h"
#include <string>
#include <stdlib.h>

constexpr int kMainMinimumLambda = 120;

// Random seed for key generation
// Note: In real applications, a cryptographically secure seed needs to be used.
constexpr std::array<uint32_t, 3> kSeed = {314, 1592, 657};
const char* small_db_path = "/home/kali/reb/fully-homomorphic-encryption-main/transpiler/examples/sgbd_short/data/script_default_verybig";

TFHEParameters params(kMainMinimumLambda);
TFHESecretKeySet key(params, kSeed);

TEST(PerformanceTest, SELECT_ALL_FROM_EMPLOYES_WHERE_AGE_EQUAL_32) {

    delete_db();
    load_script(small_db_path, key); 

    vector<Tfhe<short>> cipher_result = {};
    for (int i = 0; i < dbSize; ++i) {
        Tfhe<short> tmp(params);
        cipher_result.push_back(std::move(tmp));
    }

    const char* op = "==";
    const char* tableName = "Employes";
    const char* columnName = "age";
    short condition = 32;
    auto condition_cipher = Tfhe<short>::Encrypt(condition, key);

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    Table * t = select_where(cipher_result, tableName, columnName, condition_cipher, op, key.cloud());

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    vector<short> result = {};

    for (int i = 0; i < t->getNbRow(); ++i) {
        result.push_back(cipher_result[i].Decrypt(key));
    }

    EXPECT_EQ(result.size(), 20);

    EXPECT_EQ(result[0], 1);
    EXPECT_EQ(result[1], 0);
    EXPECT_EQ(result[2], 1);
    EXPECT_EQ(result[3], 0);
    EXPECT_EQ(result[4], 0);
    EXPECT_EQ(result[5], 0);
    EXPECT_EQ(result[6], 0);
    EXPECT_EQ(result[7], 1);
    EXPECT_EQ(result[8], 0);
    EXPECT_EQ(result[9], 0);
    EXPECT_EQ(result[10], 0);
    EXPECT_EQ(result[11], 0);
    EXPECT_EQ(result[12], 0);
    EXPECT_EQ(result[13], 0);
    EXPECT_EQ(result[14], 0);
    EXPECT_EQ(result[15], 0);
    EXPECT_EQ(result[16], 0);
    EXPECT_EQ(result[17], 0);
    EXPECT_EQ(result[18], 0);
    EXPECT_EQ(result[19], 0);
    
    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;
    
}


TEST(PerformanceTest, SELECT_DISTINCT_AGE_FROM_EMPLOYES) {

    delete_db();
    load_script(small_db_path, key); 

    vector<Tfhe<short>> cipher_tmp = {};
    for (int i = 0; i < dbSize; ++i) {
        Tfhe<short> tmp(params);
        cipher_tmp.push_back(std::move(tmp));
    }

    vector<Tfhe<short>> cipher_result = {};
    for (int i = 0; i < dbSize; ++i) {
        Tfhe<short> tmp(params);
        cipher_result.push_back(std::move(tmp));
    }

    const char* tableName = "Employes";
    const char* columnName = "age";

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    Table * t = select_distinct(cipher_result, cipher_tmp, tableName, columnName, key.cloud());

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    vector<short> result = {};

    for (int i = 0; i < t->getNbRow(); ++i) {
        if (i == 0) {
            result.push_back(1);
        } else {
            result.push_back(cipher_result[i].Decrypt(key));
        }
    }

    EXPECT_EQ(result.size(), 20);

    EXPECT_EQ(result[0], 1);
    EXPECT_EQ(result[1], 1);
    EXPECT_EQ(result[2], 0);
    EXPECT_EQ(result[3], 1);
    EXPECT_EQ(result[4], 1);
    EXPECT_EQ(result[5], 0);
    EXPECT_EQ(result[6], 0);
    EXPECT_EQ(result[7], 0);
    EXPECT_EQ(result[8], 1);
    EXPECT_EQ(result[9], 1);
    EXPECT_EQ(result[10], 1);
    EXPECT_EQ(result[11], 1);
    EXPECT_EQ(result[12], 1);
    EXPECT_EQ(result[13], 1);
    EXPECT_EQ(result[14], 1);
    EXPECT_EQ(result[15], 1);
    EXPECT_EQ(result[16], 1);
    EXPECT_EQ(result[17], 1);
    EXPECT_EQ(result[18], 1);
    EXPECT_EQ(result[19], 1);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;
}


TEST(PerformanceTest, SELECT_ALL_FROM_EMPLOYES_INNER_JOIN_DEPARTEMENT) {

    delete_db();
    load_script(small_db_path, key); 

    vector<Tfhe<short>> cipher_result = {};

    for (int i = 0; i < dbSize * dbSize; ++i) {
        Tfhe<short> index(params);
        cipher_result.push_back(std::move(index));
    }

    const char* tableName_1 = "Employes";
    const char* columnName_1 = "ID";

    const char* tableName_2 = "Departements";
    const char* columnName_2 = "employe_id";

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    std::tuple<int, int> size = inner_join(cipher_result, tableName_1, tableName_2, columnName_1, columnName_2, key.cloud());

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    vector<short> result = {};

    for (int i = 0; i < (std::get<0>(size) * std::get<1>(size)); ++i) {
        result.push_back(cipher_result[i].Decrypt(key));   
    }

    EXPECT_EQ(result.size(), 60);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;
}

TEST(PerformanceTest, SELECT_SUM_AGE_FROM_EMPLOYES) {
    delete_db();
    load_script(small_db_path, key); 

    const char* tableName = "Employes";
    const char* columnName = "age";

    Tfhe<short> cipher_result(params);

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();
    
    sum(cipher_result, tableName, columnName, key.cloud());

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decrypt(key);

    EXPECT_EQ(result, 726);
    
    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;
    
}


TEST(PerformanceTest, SELECT_AVG_AGE_FROM_EMPLOYES) {
    delete_db();
    load_script(small_db_path, key); 

    const char* tableName = "Employes";
    const char* columnName = "age";

    vector<Tfhe<short>> cipher_tmp = {};
    for (int i = 0; i < dbSize; ++i) {
        Tfhe<short> index(params);
        cipher_tmp.push_back(std::move(index));
    }

    Tfhe<short> cipher_tmp_2(params);
    Tfhe<short> cipher_result(params);

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();
    
    avg(cipher_result, cipher_tmp_2, cipher_tmp, tableName, columnName, key.cloud());

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decrypt(key);
    
    EXPECT_EQ(result, 36);
    
    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;
    
}

TEST(PerformanceTest, SELECT_COUNT_FROM_EMPLOYES_WHERE_AGE_INFERIOR_33) {
    delete_db();
    load_script(small_db_path, key); 

    const char* tableName = "Employes";
    const char* columnName = "age";
    const char* op = "<";
    short condition = 33;

    auto condition_cipher = Tfhe<short>::Encrypt(condition, key);

    Tfhe<short> cipher_result(params);

    vector<Tfhe<short>> cipher_tmp = {};

    for (int i = 0; i < dbSize; ++i) {
        Tfhe<short> index(params);
        cipher_tmp.push_back(std::move(index));
    }

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();
    
    count_where(cipher_result, cipher_tmp, tableName, columnName, condition_cipher, op, key.cloud());

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decrypt(key);
    
    EXPECT_EQ(result, 12);
    
    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;
    
}

TEST(PerformanceTest, SELECT_SUM_AGE_FROM_EMPLOYES_WHERE_AGE_SUPERIOR_28) {
    delete_db();
    load_script(small_db_path, key); 

    const char* tableName = "Employes";
    const char* columnName = "age";
    const char* op = ">";
    short condition = 28;

    auto condition_cipher = Tfhe<short>::Encrypt(condition, key);

    Tfhe<short> cipher_result(params);
    vector<Tfhe<short>> cipher_tmp = {};

    for (int i = 0; i < dbSize; ++i) {
        Tfhe<short> index(params);
        cipher_tmp.push_back(std::move(index));
    }

    vector<Tfhe<short>> cipher_tmp_2 = {};

    for (int i = 0; i < dbSize; ++i) {
        Tfhe<short> index(params);
        cipher_tmp_2.push_back(std::move(index));
    }

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();
    
    sum_where(cipher_result, cipher_tmp, cipher_tmp_2, tableName, columnName, condition_cipher, op, key.cloud());

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decrypt(key);
    
    EXPECT_EQ(result, 547);
    
    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;
    
}


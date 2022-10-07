/*
*   File name: performance_test.cc
*   
*   Description: 
*   Test of performance for all the type of queries for n = 4 (n: number of entries in the table). 
*
*
*   Author: Rébecca Tevaearai
*   Date: October 2022
*/

#include "transpiler/examples/sgbd_cleartext/utils.h"
#include "transpiler/examples/sgbd_cleartext/queries.h"
#include "transpiler/examples/sgbd_cleartext/server.h"

#include "transpiler/data/cleartext_data.h"
#include "gtest/gtest.h"
#include <string>
#include <stdlib.h>


const char* db_path = "/home/kali/reb/fully-homomorphic-encryption-main/transpiler/examples/sgbd_cleartext/data/script_default";


TEST(PerformanceTest, SELECT_ALL_FROM_EMPLOYES_WHERE_AGE_EQUAL_32) {

    delete_db();
    load_script(db_path); 

    vector<Encoded<short>> cipher_result = {};
    
    for (int i = 0; i < dbSize; ++i) {
        Encoded<short> tmp;
        cipher_result.push_back(std::move(tmp));
    }
    
    const char* op = "==";
    const char* tableName = "Employes";
    const char* columnName = "age";
    short condition = 32;

    Encoded<short> condition_cipher(condition);

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    Table * t = select_where(cipher_result, tableName, columnName, condition_cipher, op);

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    vector<short> result = {};

    for (int i = 0; i < t->getNbRow(); ++i) {
        result.push_back(cipher_result[i].Decode());
    }

    EXPECT_EQ(result.size(), 4);

    EXPECT_EQ(result[0], 1);
    EXPECT_EQ(result[1], 0);
    EXPECT_EQ(result[2], 1);
    EXPECT_EQ(result[3], 0);
    
    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;
    
}


TEST(PerformanceTest, SELECT_DISTINCT_AGE_FROM_EMPLOYES) {

    delete_db();
    load_script(db_path); 

    vector<Encoded<short>> cipher_tmp = {};
    
    for (int i = 0; i < dbSize; ++i) {
        Encoded<short> tmp;
        cipher_tmp.push_back(std::move(tmp));
    }
    
    vector<Encoded<short>> cipher_result = {};
    
    for (int i = 0; i < dbSize; ++i) {
        Encoded<short> tmp;
        cipher_result.push_back(std::move(tmp));
    }
    
    const char* tableName = "Employes";
    const char* columnName = "age";

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    Table * t = select_distinct(cipher_result, cipher_tmp, tableName, columnName);

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    vector<short> result = {};

    for (int i = 0; i < t->getNbRow(); ++i) {
        if (i == 0) {
            result.push_back(1);
        } else {
            result.push_back(cipher_result[i].Decode());
        }
    }

    EXPECT_EQ(result.size(), 4);

    EXPECT_EQ(result[0], 1);
    EXPECT_EQ(result[1], 1);
    EXPECT_EQ(result[2], 0);
    EXPECT_EQ(result[3], 1);

    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;
}

TEST(PerformanceTest, SELECT_ALL_FROM_EMPLOYES_INNER_JOIN_DEPARTEMENT) {

    delete_db();
    load_script(db_path); 

    vector<Encoded<short>> cipher_result = {};
    
    for (int i = 0; i < dbSize * dbSize; ++i) {
        Encoded<short> tmp;
        cipher_result.push_back(std::move(tmp));
    }

    const char* tableName_1 = "Employes";
    const char* columnName_1 = "ID";

    const char* tableName_2 = "Departements";
    const char* columnName_2 = "employe_id";

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    std::tuple<int, int> size = inner_join(cipher_result, tableName_1, tableName_2, columnName_1, columnName_2);

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    vector<short> result = {};

    for (int i = 0; i < (std::get<0>(size) * std::get<1>(size)); ++i) {
        result.push_back(cipher_result[i].Decode());   
    }

    EXPECT_EQ(result.size(), 12);
/*
    EXPECT_EQ(result[0], 0);
    EXPECT_EQ(result[1], 1);
    EXPECT_EQ(result[2], 0);
    EXPECT_EQ(result[3], 0);
    EXPECT_EQ(result[4], 1);
    EXPECT_EQ(result[5], 0);
    EXPECT_EQ(result[6], 0);
    EXPECT_EQ(result[7], 0);
*/
    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;
}

TEST(PerformanceTest, SELECT_SUM_AGE_FROM_EMPLOYES) {
    delete_db();
    load_script(db_path); 

    const char* tableName = "Employes";
    const char* columnName = "age";

    Encoded<short> cipher_result;

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();
    
    sum(cipher_result, tableName, columnName);

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();

    EXPECT_EQ(result, 140);
    
    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;
    
}


TEST(PerformanceTest, SELECT_AVG_AGE_FROM_EMPLOYES) {
    delete_db();
    load_script(db_path); 

    const char* tableName = "Employes";
    const char* columnName = "age";

    vector<Encoded<short>> cipher_tmp = {};
    
    for (int i = 0; i < dbSize; ++i) {
        Encoded<short> tmp;
        cipher_tmp.push_back(std::move(tmp));
    }

    Encoded<short> cipher_tmp_2;
    Encoded<short> cipher_result;

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();
    
    avg(cipher_result, cipher_tmp_2, cipher_tmp, tableName, columnName);

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();
    
    EXPECT_EQ(result, 35);
    
    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;
}


TEST(PerformanceTest, SELECT_COUNT_FROM_EMPLOYES_WHERE_AGE_INFERIOR_33) {
    delete_db();
    load_script(db_path); 

    const char* tableName = "Employes";
    const char* columnName = "age";
    const char* op = "<";
    short condition = 33;
    Encoded<short> condition_cipher(condition);

    Encoded<short> cipher_result;

    vector<Encoded<short>> cipher_tmp = {};
    
    for (int i = 0; i < dbSize; ++i) {
        Encoded<short> tmp;
        cipher_tmp.push_back(std::move(tmp));
    }

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();
    
    count_where(cipher_result, cipher_tmp, tableName, columnName, condition_cipher, op);

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();
    
    EXPECT_EQ(result, 3);
    
    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;
    
}

TEST(PerformanceTest, SELECT_SUM_AGE_FROM_EMPLOYES_WHERE_AGE_SUPERIOR_28) {
    delete_db();
    load_script(db_path); 

    const char* tableName = "Employes";
    const char* columnName = "age";
    const char* op = ">";
    short condition = 28;

    Encoded<short> condition_cipher(condition);

    Encoded<short> cipher_result;

    vector<Encoded<short>> cipher_tmp = {};
    
    for (int i = 0; i < dbSize; ++i) {
        Encoded<short> tmp;
        cipher_tmp.push_back(std::move(tmp));
    }

    vector<Encoded<short>> cipher_tmp_2 = {};
    
    for (int i = 0; i < dbSize; ++i) {
        Encoded<short> tmp;
        cipher_tmp_2.push_back(std::move(tmp));
    }

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();
    
    sum_where(cipher_result, cipher_tmp, cipher_tmp_2, tableName, columnName, condition_cipher, op);

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    auto result = cipher_result.Decode();
    
    EXPECT_EQ(result, 113);
    
    std::cout << std::endl << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl << std::endl;
    
}



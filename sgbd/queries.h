#ifndef SGBD_QUERIES_H_
#define SGBD_QUERIES_H_

#include <cstddef>
#include <stdlib.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <tuple>
#include <utility>
#include <exception>
#include <stdexcept>

#include "absl/strings/numbers.h"
#include "absl/strings/str_format.h"
#include "absl/types/span.h"
#include "tfhe/tfhe.h"
#include "tfhe/tfhe_io.h"
#include "transpiler/data/tfhe_data.h"
#include "xls/common/logging/logging.h"

#include "server.h"
#include "utils.h"

#include "transpiler/examples/sgbd/utils.h"
#include "transpiler/examples/sgbd/server.h"

constexpr int dbSize = 20;
const int SPACE = 10;

using std::string;
using std::vector;

void insert(string tableName, const vector<int>& v_int, TFHESecretKeySet& key) {
    vector<Tfhe<int>> v_int_cipher = {};

    for(int i = 0; i < v_int.size(); ++i) {
        v_int_cipher.push_back(Tfhe<int>::Encrypt(v_int[i], key));
    }

    insert_row(tableName, v_int_cipher);

    return;
}


void load_script(string path, TFHESecretKeySet& key) {

    std::fstream file;
    file.open(path, std::ios::in);

    if (file.is_open()) { 
        string line;
        while(getline(file, line)) { 
            size_t pos_start = line.find('(');
            size_t pos_end = line.find(')');

            if (pos_start == string::npos || pos_end == string::npos) {
                continue;
            } 

            string value = line.substr(pos_start + 1, pos_end - pos_start - 1);

            size_t pos = line.find(' ');
            string type = line.substr(0, pos);

            pos = line.find(' ', pos + 1);
            size_t pos_2 = line.find(' ', pos + 1);

            string tableName = line.substr(pos + 1, pos_2 - pos -1);

            if (type == "CREATE") {
                vector<std::tuple<string, string>> v = {};
                vector<string> strs = {};
                split(value, strs, ',');

                for (int i = 0; i < strs.size(); ++i) {
                    vector<string> val;
                    split(strs[i], val, ' ');
                    if (val.size() == 3) {
                        v.push_back(std::make_tuple(val[1], val[2]));
                    } else {
                        v.push_back(std::make_tuple(val[0], val[1]));
                    }
                }          

                create_table(tableName, v);

            } else if (type == "INSERT") {
                vector<string> strs;
                split(value, strs, ',');

                vector<int> v_int = {};
                for (int i = 0; i < strs.size(); ++i) {

                    if (strs[i].find('\'') != string::npos) {

                        vector<string> tmp;
                        split(strs[i], tmp, '\'');
                        char c = tmp[1][0];
                        v_int.push_back((int)c);

                    } else {
                        v_int.push_back(stoi(strs[i]));
                    }
                }

                insert(tableName, v_int, key);

            } else {
                std::cout << "Incorrect format" << std::endl;
            }
        }
        file.close();

    } else {
        std::cout << "Cannot open file" << std::endl;
    }

    return;
}


void display_result(string tableName, vector<vector<string>> row, int nb_columns) {

    std::cout << tableName << std::endl;

    for (int i = 0; i < nb_columns; i++) {
        std::cout << "+";
        for (int j = 0; j <= SPACE; j++) {
            std::cout << "-";
        }
    }
    std::cout << "+" << std::endl;

    for (int i = 0; i < nb_columns; ++i) {
        std::cout << "|" << std::setw(SPACE) << std::right << row[0][i] << " ";
    }

    std::cout << "|" << std::endl;

    for (int i = 0; i < nb_columns; i++) {
        std::cout << "+";
        for (int j = 0; j <= SPACE; j++) {
            std::cout << "-";
        }
    }

    std::cout << "+" << std::endl;

    for (int i = 1; i < row.size(); ++i) {
        for (int j = 0; j < nb_columns; ++j) {
            std::cout << "|" << std::setw(SPACE) << std::right << row[i][j] << " ";
        }
        std::cout << "|" << std::endl;
    }

    for (int i = 0; i < nb_columns; i++) {
        std::cout << "+";
        for (int j = 0; j <= SPACE; j++) {
            std::cout << "-";
        }
    }
    std::cout << "+" << std::endl;
}


void query_where(string tableName, string columnName, string op, int condition, TFHESecretKeySet& key, TFHEParameters& params) {

    auto condition_cipher = Tfhe<int>::Encrypt(condition, key);

    vector<Tfhe<int>> cipher_result = {};

    for (int i = 0; i < dbSize; ++i) {
        Tfhe<int> index(params);
        cipher_result.push_back(std::move(index));
    }

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();
    
    Table * t = select_where(cipher_result, tableName, columnName, condition_cipher, op, key.cloud());

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    vector<vector<string>> result_row = {};
    vector<string> colNames = t->getColumnName();
    int nb_col = colNames.size();

    result_row.push_back(colNames);

    for (int i = 0; i < t->getNbRow(); ++i) {
        if (cipher_result[i].Decrypt(key) == 1) {
            vector<string> v = t->get_row(i, key);
            result_row.push_back(v);
        }    
    }

    std::cout << "\nResult: " << std::endl;
    display_result(tableName, result_row, nb_col);


    std::cout << "\t\t\t\t\tComputation done" << std::endl;
    std::cout << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl;

    return;
}


void query_sum(string tableName, string columnName, TFHESecretKeySet& key, TFHEParameters& params) {

    Tfhe<int> cipher_result(params);

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();
    
    sum(cipher_result, tableName, columnName, key.cloud());

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    std::cout << "Result: " << cipher_result.Decrypt(key) << std::endl;


    std::cout << "\t\t\t\t\tComputation done" << std::endl;
    std::cout << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl;

    return;
}


void query_distinct(string tableName, string columnName, TFHESecretKeySet& key, TFHEParameters& params) {

    vector<Tfhe<int>> cipher_result = {};

    for (int i = 0; i < dbSize; ++i) {
        Tfhe<int> index(params);
        cipher_result.push_back(std::move(index));
    }

    vector<Tfhe<int>> cipher_tmp = {};

    for (int i = 0; i < dbSize; ++i) {
        Tfhe<int> index(params);
        cipher_tmp.push_back(std::move(index));
    }

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();
    
    Table * t = select_distinct(cipher_result, cipher_tmp, tableName, columnName, key.cloud());

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    vector<vector<string>> result_row = {};
    vector<string> colNames = t->getColumnName();
    int nb_col = colNames.size();

    result_row.push_back(colNames);

    for (int i = 0; i < t->getNbRow(); ++i) {
        if (i == 0) {
            vector<string> v = t->get_row(i, key);
            result_row.push_back(v);
        } else {
            if (cipher_result[i].Decrypt(key) == 1) {
                vector<string> v = t->get_row(i, key);
                result_row.push_back(v);
            }
        }
    }

    std::cout << "\nResult:" << std::endl;
    display_result(tableName, result_row, nb_col);


    std::cout << "\t\t\t\t\tComputation done" << std::endl;
    std::cout << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl;
    return;
}


void query_join(string tableName_1, string tableName_2, string columnName_1, string columnName_2, TFHESecretKeySet& key, TFHEParameters& params) {
    try {

        vector<Tfhe<int>> index_cipher = {};

        for (int i = 0; i < dbSize * dbSize; ++i) {
            Tfhe<int> index(params);
            index_cipher.push_back(std::move(index));
        }

        absl::Time start_time = absl::Now();
        double cpu_start_time = clock();

        std::tuple<int, int> size = inner_join(index_cipher, tableName_1, tableName_2, columnName_1, columnName_2, key.cloud());

        double cpu_end_time = clock();
        absl::Time end_time = absl::Now();

        int tableIndex_1 = database.getTableIndex(tableName_1);
        int tableIndex_2 = database.getTableIndex(tableName_2);

        vector<vector<string>> result_row = {};

        vector<string> colNames_1 = database.tables[tableIndex_1].getColumnName();
        vector<string> colNames_2 = database.tables[tableIndex_2].getColumnName();
        
        int nb_col_1 = colNames_1.size();
        int nb_col_2 = colNames_2.size();

        vector<string> v;
        v.resize(colNames_1.size() + colNames_2.size());
        std::move(colNames_1.begin(), colNames_1.end(), v.begin());
        std::move(colNames_2.begin(), colNames_2.end(), v.begin() + colNames_1.size());

        result_row.push_back(v);

        int k = 0;
        for (int i = 0; i < std::get<0>(size) * std::get<1>(size); ++i) {

            if ((i != 0) && ((i % std::get<1>(size)) == 0)) {
                ++k;
            } 
            if (index_cipher[i].Decrypt(key) == 1) {
                vector<string> v1 = database.tables[tableIndex_1].get_row(k, key);
                vector<string> v2 = database.tables[tableIndex_2].get_row(i % std::get<1>(size), key);

                vector<string> vec;
                vec.resize(v1.size() + v2.size());
                std::move(v1.begin(), v1.end(), vec.begin());
                std::move(v2.begin(), v2.end(), vec.begin() + v1.size());
                result_row.push_back(vec);
            }
        }

        std::cout << "\nResult:" << std::endl;
        display_result(tableName_1 + " " + tableName_2, result_row, nb_col_1 + nb_col_2);


        std::cout << "\t\t\t\t\tComputation done" << std::endl;
        std::cout << "\t\t\t\t\tTotal time: "
            << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
        std::cout << "\t\t\t\t\t  CPU time: "
            << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl;

        return;

    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return;
    }
}


void query_avg(string tableName, string columnName, TFHESecretKeySet& key, TFHEParameters& params) {

    vector<Tfhe<int>> cipher_tmp = {};
    for (int i = 0; i < dbSize; ++i) {
        Tfhe<int> index(params);
        cipher_tmp.push_back(std::move(index));
    }

    Tfhe<int> cipher_result(params);
    Tfhe<int> cipher_r(params);

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    avg(cipher_result, cipher_r, cipher_tmp, tableName, columnName, key.cloud());
    
    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    std::cout << "Result: " << cipher_result.Decrypt(key) << std::endl;

    std::cout << "\t\t\t\t\tComputation done" << std::endl;
    std::cout << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl;

    return;
}


void query_count(string tableName) {

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    int count = simple_count(tableName);

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    std::cout << "Result: " << count << std::endl;
    

    std::cout << "\t\t\t\t\tComputation done" << std::endl;
    std::cout << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl;

    return;

}


void query_sum_where(string tableName, string columnName, string op, int condition, TFHESecretKeySet& key, TFHEParameters& params) {
    
    auto condition_cipher = Tfhe<int>::Encrypt(condition, key);

    Tfhe<int> cipher_result(params);
    vector<Tfhe<int>> cipher_tmp = {};

    for (int i = 0; i < dbSize; ++i) {
        Tfhe<int> index(params);
        cipher_tmp.push_back(std::move(index));
    }

    vector<Tfhe<int>> cipher_tmp_2 = {};

    for (int i = 0; i < dbSize; ++i) {
        Tfhe<int> index(params);
        cipher_tmp_2.push_back(std::move(index));
    }

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();
    
    sum_where(cipher_result, cipher_tmp, cipher_tmp_2, tableName, columnName, condition_cipher, op, key.cloud());

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    std::cout << "\nResult: " << cipher_result.Decrypt(key) << std::endl;


    std::cout << "\t\t\t\t\tComputation done" << std::endl;
    std::cout << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl;

    return;
}


void query_count_where(string tableName, string columnName, string op, int condition, TFHESecretKeySet& key, TFHEParameters& params) {

    auto condition_cipher = Tfhe<int>::Encrypt(condition, key);

    Tfhe<int> cipher_result(params);
    vector<Tfhe<int>> cipher_tmp = {};

    for (int i = 0; i < dbSize; ++i) {
        Tfhe<int> index(params);
        cipher_tmp.push_back(std::move(index));
    }

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();
    
    count_where(cipher_result, cipher_tmp, tableName, columnName, condition_cipher, op, key.cloud());

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    std::cout << "\nResult: " << cipher_result.Decrypt(key) << std::endl;


    std::cout << "\t\t\t\t\tComputation done" << std::endl;
    std::cout << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << std::endl;
    std::cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << std::endl;

    return;
}


void show_tables(TFHESecretKeySet& key) {
    
    int size = database.tables.size();

    for (int i = 0; i < size; ++i) {
        string tableName = database.tables[i].name;
        
        vector<vector<string>> result_row = {};
        vector<string> colNames = database.tables[i].getColumnName();
        result_row.push_back(colNames);
        int nb_col = database.tables[i].c_int.size();
        int nb_row = database.tables[i].c_int[0].values.size();

        for (int j = 0; j < nb_row; ++j) {
            vector<string> v = database.tables[i].get_row(j, key);
            result_row.push_back(v);
        }

        std::cout << std::endl;
        display_result(tableName, result_row, nb_col);
    }

} 

#endif /* _QUERY_H_ */

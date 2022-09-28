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

constexpr int kMainMinimumLambda = 120;
// Max number of row for a Table.
constexpr int dbSize = 20;
const int SPACE = 10;

// Random seed for key generation
// Note: In real applications, a cryptographically secure seed needs to be used.
constexpr std::array<uint32_t, 3> kSeed = {314, 1592, 657};

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


string get_tableName_from_query(string query, int index) {
    vector<string> v;
    split(query, v, ' ');

    return v[index];
}


string get_tableName_from_select_query(string query) {
    size_t pos = query.find("FROM");

    if (pos == string::npos) {
        throw std::invalid_argument("get_tableName_from_select_query: FROM not found");
    }

    pos += 5;
    size_t pos_end = query.find(' ', pos);

    return query.substr(pos, pos_end - pos);
}


vector<string> get_columns_from_select_query(string query) {
    vector<string> columns = {};
    size_t pos_start = query.find('(');
    size_t pos_end = query.find(')');

    if ((pos_start == string::npos) || pos_end == string::npos) {
        throw std::invalid_argument("get_columns_from_select_query: no column found");
    }

    string tmp = query.substr(pos_start + 1, pos_end - pos_start - 1);
    vector<string> strs;
    split(tmp, strs, ',');

    for (int i = 0; i < strs.size(); ++i) {
        strs[i].erase(remove(strs[i].begin(), strs[i].end(), ' '), strs[i].end());
        columns.push_back(strs[i]);
    }       

    return columns;
}


void handle_select(string query, TFHESecretKeySet& key, TFHEParameters& params) {
    try {
        string tableName = get_tableName_from_select_query(query);
        vector<string> columnsNames = get_columns_from_select_query(query);
        vector<string> queries = {};

        if (query.find("DISTINCT") != string::npos) {
            queries.push_back("DISTINCT");

        } else if (query.find("SUM") != string::npos) {
            queries.push_back("SUM");

        } else if (query.find("AVG") != string::npos) {
            queries.push_back("AVG");

        } else if (query.find("COUNT") != string::npos) {
            queries.push_back("COUNT");

        }

        size_t pos_join = query.find("JOIN");
        vector<string> join_params = {};

        if (pos_join != string::npos) {
            queries.push_back("JOIN");
            string join_param = query.substr(pos_join + 5);
            split(join_param, join_params, ' ');

            size_t pos_on = query.find("ON");
            string tableName2 = query.substr(pos_join + 5, pos_on - pos_join - pos_join + 5);

            join_params.push_back(tableName2);
        }

        size_t pos_where = query.find("WHERE");
        vector<string> where_params = {};

        if (pos_where != string::npos) {
            queries.push_back("WHERE");
            string where_param = query.substr(pos_where + 6);
            split(where_param, where_params, ' ');        
        }

        if (queries.size() == 1) {
            if (queries[0] == "DISTINCT") {
                query_distinct(tableName, columnsNames[0], key, params);

            } else if (queries[0] == "COUNT") {
                query_count(tableName);

            } else if (queries[0] == "SUM") {
                query_sum(tableName, columnsNames[0], key, params);

            } else if (queries[0] == "AVG") {
                query_avg(tableName, columnsNames[0], key, params);

            } else if (queries[0] == "WHERE") {
                query_where(tableName, where_params[0], where_params[1], stoi(where_params[2]), key, params);

            } else if (queries[0] == "JOIN") {
                query_join(tableName, join_params[0], join_params[2], join_params[4], key, params);
            }

        } else if (queries.size() == 2) {
            if (queries[1] == "WHERE") {
                if (queries[0] == "SUM") {
                    query_sum_where(tableName, where_params[0], where_params[1], stoi(where_params[2]), key, params);

                } else if (queries[0] == "COUNT") {
                    query_count_where(tableName, where_params[0], where_params[1], stoi(where_params[2]), key, params);

                } else {
                    std::cout << "Not implemented" << std::endl;
                }
            } else {
                std::cout << "Incorrect format or not implemented" << std::endl;
            }
        } else {
            std::cout << "Incorrect format or not implemented" << std::endl;
        }
        return;

    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return;
    }
}


void handle_insert(string query, TFHESecretKeySet& key) {

    string tableName = get_tableName_from_query(query, 2);
    size_t pos_start = query.find('(');
    size_t pos_end = query.find(')');

    if (pos_start == string::npos || pos_end == string::npos) {
        std::cout << "Incorrect format for value" << std::endl;
        return;
    } 

    string value = query.substr(pos_start + 1, pos_end - pos_start - 1);
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
            try {
                v_int.push_back(stoi(strs[i]));
            } catch (std::exception& e) {
                std::cout << e.what() << std::endl;
                return;
            } 
        }
    }

    insert(tableName, v_int, key);

    return;
}


void handle_create(string query) {

    string tableName = get_tableName_from_query(query, 2);

    size_t pos_start = query.find('(');
    size_t pos_end = query.find(')');

    if (pos_start == string::npos || pos_end == string::npos) {
        std::cout << "Incorrect format for value" << std::endl;
        return;
    } 

    string value = query.substr(pos_start + 1, pos_end - pos_start - 1);

    vector<std::tuple<string, string>> v = {};
    vector<string> strs;
    split(value, strs, ',');

    for (int i = 0; i < strs.size(); ++i) {
        vector<string> val;
        split(strs[i], val, ' ');
        if (val.size() == 3) {
            v.push_back(std::make_tuple(val[1], val[2]));
        } else if (val.size() == 2) {
            v.push_back(std::make_tuple(val[0], val[1]));
        } else {
            std::cout << "Incorrect format" << std::endl;
            return;
        }
    }

    create_table(tableName, v);

    return;
}


void handle_query(TFHESecretKeySet& key, TFHEParameters& params) {
    string query = "";
    
    while(true) {
        std::cout << "Enter query (on one line, end with ;): \n";

        char c;
        while ((c = getchar()) != ';') {
            query.push_back(c);
        }
        break;
        
    }
    
    size_t pos = query.find(' ');
    string type;
    type = query.substr(1, pos - 1);

    if (type == "CREATE") {
        handle_create(query);

    } else if (type == "INSERT") {
        handle_insert(query, key);

    } else if (type == "SELECT") {
        handle_select(query, key, params);

    } else {
        std::cout << "Incorrect format" << std::endl;
        return;
    }

    return;
}


int main() {

    TFHEParameters params(kMainMinimumLambda);
    TFHESecretKeySet key(params, kSeed);

    while (true) {
        int input;
        std::cout << "\nChoose one of the following options:\n";
        std::cout << "1. query\n";
        std::cout << "2. load script\n";
        std::cout << "3. show tables\n";
        std::cout << "4. clear database\n";
        std::cout << "5. quit\n";
        std::cin >> input;

        switch (input) {
            case 1:
                handle_query(key, params);
                break;
            case 2: {
                string path = "";
                std::cout << "Enter full script path" << std::endl;
                std::cin >> path;
                load_script(path, key);
                break;
            }
            case 3:
                show_tables(key);
                break;
            case 4:
                delete_db();
                break;
            case 5:
                return 0;
            default:
                std::cout << "Invalid number" << std::endl;
                break;
        }
    }

    return 0;
}

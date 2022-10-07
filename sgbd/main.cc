/*
*   File name: main.cc
*   
*   Description: 
*   Generate the parameters for the FHE.
*   Display the application menu and handle the parsing of the queries. 
*
*
*   Author: Rébecca Tevaearai
*   Date: October 2022
*/

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
#include <typeinfo>

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
#include "transpiler/examples/sgbd/queries.h"

constexpr int kMainMinimumLambda = 120;

// Random seed for key generation
// Note: In real applications, a cryptographically secure seed needs to be used.
constexpr std::array<uint32_t, 3> kSeed = {314, 1592, 657};

using std::string;
using std::vector;


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

// A hand made function to parse user select query. 
// It would be better to replace it with a library in the futur.
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
                // stoi doesn't work if with char, convert it with (int)
                if (where_params[2].find("\'") != string::npos) {
                    query_where(tableName, where_params[0], where_params[1], (int)where_params[2][1], key, params);
                } else {
                    query_where(tableName, where_params[0], where_params[1], stoi(where_params[2]), key, params);
                }

            } else if (queries[0] == "JOIN") {
                query_join(tableName, join_params[0], join_params[2], join_params[4], key, params);
            }

        } else if (queries.size() == 2) {
            if (queries[1] == "WHERE") {
                if (queries[0] == "SUM") {
                    // stoi doesn't work if with char, convert it with (int)
                    if (where_params[2].find("\'") != string::npos) {
                        query_sum_where(tableName, where_params[0], where_params[1], (int)where_params[2][1], key, params);
                    } else {
                        query_sum_where(tableName, where_params[0], where_params[1], stoi(where_params[2]), key, params);
                    }
                } else if (queries[0] == "COUNT") {
                    // stoi doesn't work with char, convert it with (int)
                    if (where_params[2].find("\'") != string::npos) {
                        query_count_where(tableName, where_params[0], where_params[1], (int)where_params[2][1], key, params);
                    } else {
                        query_count_where(tableName, where_params[0], where_params[1], stoi(where_params[2]), key, params);
                    }
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

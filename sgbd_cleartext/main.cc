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
//#include "tfhe/tfhe.h"
//#include "tfhe/tfhe_io.h"
#include "transpiler/data/cleartext_data.h"
#include "xls/common/logging/logging.h"

#include "server.h"
#include "utils.h"

#include "transpiler/examples/sgbd_cleartext/utils.h"
#include "transpiler/examples/sgbd_cleartext/server.h"
#include "transpiler/examples/sgbd_cleartext/queries.h"

//constexpr int kMainMinimumLambda = 120;

// Random seed for key generation
// Note: In real applications, a cryptographically secure seed needs to be used.
//constexpr std::array<uint32_t, 3> kSeed = {314, 1592, 657};

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


void handle_select(string query) {
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
                query_distinct(tableName, columnsNames[0]);

            } else if (queries[0] == "COUNT") {
                query_count(tableName);

            } else if (queries[0] == "SUM") {
                query_sum(tableName, columnsNames[0]);

            } else if (queries[0] == "AVG") {
                query_avg(tableName, columnsNames[0]);

            } else if (queries[0] == "WHERE") {
                query_where(tableName, where_params[0], where_params[1], stoi(where_params[2]));

            } else if (queries[0] == "JOIN") {
                query_join(tableName, join_params[0], join_params[2], join_params[4]);
            }

        } else if (queries.size() == 2) {
            if (queries[1] == "WHERE") {
                if (queries[0] == "SUM") {
                    query_sum_where(tableName, where_params[0], where_params[1], stoi(where_params[2]));

                } else if (queries[0] == "COUNT") {
                    query_count_where(tableName, where_params[0], where_params[1], stoi(where_params[2]));

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


void handle_insert(string query) {

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

    vector<short> v_short = {};
    for (int i = 0; i < strs.size(); ++i) {

        if (strs[i].find('\'') != string::npos) {

            vector<string> tmp;
            split(strs[i], tmp, '\'');
            char c = tmp[1][0];
            v_short.push_back((short)c);

        } else {
            try {
                v_short.push_back(stoi(strs[i]));
            } catch (std::exception& e) {
                std::cout << e.what() << std::endl;
                return;
            } 
        }
    }

    insert(tableName, v_short);

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


void handle_query() {
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
        handle_insert(query);

    } else if (type == "SELECT") {
        handle_select(query);

    } else {
        std::cout << "Incorrect format" << std::endl;
        return;
    }

    return;
}


int main() {

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
                handle_query();
                break;
            case 2: {
                string path = "";
                std::cout << "Enter full script path" << std::endl;
                std::cin >> path;
                load_script(path);
                break;
            }
            case 3:
                show_tables();
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
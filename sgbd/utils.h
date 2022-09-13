#ifndef _UTILS_H_
#define _UTILS_H_

#include <exception>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <vector>
#include <string>
#include <tuple>

#include "transpiler/data/tfhe_data.h"

using namespace std;


template<typename T>
struct Column {
    string tableName;
    string name;
    vector<T> values;
};

struct Table {
    string name;
    int nbColumn;
    vector<tuple<string, string>> columnName_and_type;
    vector<Column<Tfhe<int>>> c_int;
    vector<Column<Tfhe<char>>> c_char;

    int getColumnIndex(string columnName) {
        for (int i = 0; i < columnName_and_type.size(); ++i) {
            if (get<0>(columnName_and_type[i]) == columnName) {
                return i;
            }
        }

        throw invalid_argument("Table, getColumnIndex: no column found");
    }

/*
    int * getColumnPointer(string columnName) {
        for (int i = 0; i < columnName_and_type.size(); ++i) {

            if (get<0>(columnName_and_type[i]) == columnName) {

                if (get<1>(columnName_and_type[i]) == "int") {
                    return getColumnIntPointer(columnName);

                } else if (get<1>(columnName_and_type[i]) == "char") {
                    return getColumnCharPointer(columnName);
                }
            }
        }

        throw invalid_argument("Table, getColumnPointer: no column found");
    }
*/
    int getColumnIntIndex(string columnName) {
        for (int i = 0 ; i < c_int.size(); ++i) {
            if (c_int[i].name == columnName) {
                return i;
            }
        }

        throw invalid_argument("Table, getColumnIntIndex: no column found");
    }

    Column<Tfhe<int>> * getColumnIntPointer(string columnName) {
        for (int i = 0 ; i < c_int.size(); ++i) {
            if (c_int[i].name == columnName) {
                return &c_int[i];
            }
        }

        throw invalid_argument("Table, getColumnIntPointer: no column found");
    }

    int getColumncharIndex(string columnName) {
        for (int i = 0 ; i < c_char.size(); ++i) {
            if (c_char[i].name == columnName) {
                return i;
            }
        }

        throw invalid_argument("Table, getColumncharIndex: no column found");
    }
/*
    int * getColumnCharPointer(string columnName) {
        for (int i = 0 ; i < c_char.size(); ++i) {
            if (c_char[i].name == columnName) {
                return &c_char[i];
            }
        }

        throw invalid_argument("Table, getColumncharPointer: no column found");
    }
*/
    vector<string> getColumnName() {
        vector<string> v = {};
        for (int i = 0; i < columnName_and_type.size(); ++i) {
            v.push_back(get<0>(columnName_and_type[i]));
        }
        return v;
    }

    vector<string> get_row(int index, TFHESecretKeySet& key) {

        vector<string> row = {};

        for(int i = 0; i < columnName_and_type.size(); ++i) {

            if (get<1>(columnName_and_type[i]) == "int") {

                int j = getColumnIntIndex(get<0>(columnName_and_type[i]));
                auto tmp = c_int[j].values[index].Decrypt(key);
                row.push_back(to_string(tmp));

            } else if (get<1>(columnName_and_type[i]) == "char") {

                int j = getColumncharIndex(get<0>(columnName_and_type[i]));
                auto tmp = c_char[j].values[index].Decrypt(key);
                string s(1, tmp);
                row.push_back(s);
            }
        }

        return row;
    }

};

struct Database {
    vector<Table> tables;

    int getTableIndex(string tableName) { 
        for (int i = 0; i < tables.size(); ++i) {
            if (tables[i].name == tableName) {
                return i;
            }
        } 
        throw invalid_argument("Database, getTableIndex: no table found");
    }
};

Database database = {
    {},
};


#endif /* _UTILS_H_ */
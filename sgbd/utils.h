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

struct Query {
    string name;
    vector<string> tablesNames;
    vector<Tfhe<int>> arg;
    vector<string> op;
};

template<typename T>
struct Column {
    string tableName;
    string name;
    string type;
    vector<T> values;
};

struct Table {
    string name;
    vector<Column<Tfhe<int>>> c_int;
    //int nbColumn;
    //vector<tuple<string, string>> columnName_and_type;
    //vector<Column<Tfhe<char>>> c_char;

    int getNbRow() {
        return c_int[0].values.size();
    }

    int getColumnIndex(string columnName) {
        for (int i = 0; i < c_int.size(); ++i) {
            if (c_int[i].name == columnName) {
                return i;
            }
        }

        throw invalid_argument("Table, getColumnIndex: no column found");
    }

    Column<Tfhe<int>> * getColumnPointer(string columnName) {
        for (int i = 0; i < c_int.size(); ++i) {
            if (c_int[i].name == columnName) {
                return &c_int[i];
            }
        }

        throw invalid_argument("Table, getColumnIndex: no column found");
    }

    vector<string> getColumnName() {
        vector<string> v = {};
        for (int i = 0; i < c_int.size(); ++i) {
            v.push_back(c_int[i].name);
        }
        return v;
    }

    vector<string> get_row(int index, TFHESecretKeySet& key) {

        vector<string> row = {};

        for(int i = 0; i < c_int.size(); ++i) {
            auto tmp = c_int[i].values[index].Decrypt(key);
            if (c_int[i].type == "char") {
                char c = char(tmp);
                string str;
                str += c;
                row.push_back(str);
            } else {
                row.push_back(to_string(tmp));
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

#ifndef SGBD_UTILS_H_
#define SGBD_UTILS_H_

#include <exception>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <vector>
#include <tuple>

#include "transpiler/data/tfhe_data.h"

using std::string;
using std::vector;

size_t split(const string &txt, vector<string> &strs, char ch) {
    size_t pos = txt.find(ch);
    size_t initialPos = 0;
    strs.clear();

    while(pos != string::npos) {
        strs.push_back(txt.substr(initialPos, pos - initialPos));
        initialPos = pos + 1;

        pos = txt.find(ch, initialPos);
    }

    strs.push_back(txt.substr(initialPos, std::min(pos,txt.size()) - initialPos + 1));

    return strs.size();
}


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

    int getNbRow() {
        if (c_int.size() == 0) {
            return 0;
        } else {
            return c_int[0].values.size();
        }
    }

    int getColumnIndex(string columnName) {
        for (int i = 0; i < c_int.size(); ++i) {
            if (c_int[i].name == columnName) {
                return i;
            }
        }

        throw std::invalid_argument("Table, getColumnIndex: no column found");
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

        if (index > this->getNbRow()) {
            return row;
        }

        for(int i = 0; i < c_int.size(); ++i) {
            auto tmp = c_int[i].values[index].Decrypt(key);
            if (c_int[i].type == "char") {
                char c = char(tmp);
                string str;
                str += c;
                row.push_back(str);
            } else {
                row.push_back(std::to_string(tmp));
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
        throw std::invalid_argument("Database, getTableIndex: no table found");
    }
};


Database database = {
    {},
};


#endif /* SGBD_UTILS_H_ */

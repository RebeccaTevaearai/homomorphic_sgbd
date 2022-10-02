#ifndef SGBD_SERVER_H_
#define SGBD_SERVER_H_

#include <stdexcept>
#include <stdlib.h>

#include <ios>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <unistd.h>
#include <stdio.h>
#include <tuple>
#include <exception>

#include "absl/strings/numbers.h"
#include "absl/strings/str_format.h"
#include "absl/types/span.h"
//#include "api/api_equality_short.h"
//#include "api/api_inequality_short.h"
//#include "api/api_inferior_or_equal_int.h"
//#include "api/api_sum.h"
//#include "api/api_superior_or_equal_int.h"
#include "tfhe/tfhe.h"
#include "tfhe/tfhe_io.h"
#include "transpiler/data/tfhe_data.h"
#include "xls/common/logging/logging.h"

#include "transpiler/examples/sgbd_short/utils.h"

/*
#include "transpiler/examples/sgbd_short/api/api_and.h"
#include "transpiler/examples/sgbd_short/api/api_sum.h"
#include "transpiler/examples/sgbd_short/api/api_div.h"
#include "transpiler/examples/sgbd_short/api/api_equality_int.h"
#include "transpiler/examples/sgbd_short/api/api_product.h"
#include "transpiler/examples/sgbd_short/api/api_inequality_int.h"
#include "transpiler/examples/sgbd_short/api/api_superior_int.h"
#include "transpiler/examples/sgbd_short/api/api_superior_or_equal_int.h"
#include "transpiler/examples/sgbd_short/api/api_inferior_int.h"
#include "transpiler/examples/sgbd_short/api/api_inferior_or_equal_int.h"
*/

#include "transpiler/examples/sgbd_short/api_tfhe_and_short.h"
#include "transpiler/examples/sgbd_short/api_tfhe_sum_short.h"
#include "transpiler/examples/sgbd_short/api_tfhe_div_short.h"
#include "transpiler/examples/sgbd_short/api_tfhe_equality_short.h"
#include "transpiler/examples/sgbd_short/api_tfhe_product_short.h"
#include "transpiler/examples/sgbd_short/api_tfhe_inequality_short.h"
#include "transpiler/examples/sgbd_short/api_tfhe_superior_short.h"
#include "transpiler/examples/sgbd_short/api_tfhe_superior_or_equal_short.h"
#include "transpiler/examples/sgbd_short/api_tfhe_inferior_short.h"
#include "transpiler/examples/sgbd_short/api_tfhe_inferior_or_equal_short.h"

using std::string;
using std::vector;
using std::tuple;

extern Database database;


void create_table(string tableName, vector<std::tuple<string, string>>& columnName_and_type) {
    
    Table t = {
        tableName,
        {},
    };
    
    for (auto i : columnName_and_type) {

        Column<Tfhe<short>> c = {
            tableName,
            std::get<0>(i),
            std::get<1>(i),
            {},
        };
        t.c_short.push_back(std::move(c));
    }

    database.tables.push_back(std::move(t));

    return;
}


void delete_db() {
    database.tables.clear();
}


void insert_row(string tableName, vector<Tfhe<short>>& v_short) {

    int i = database.getTableIndex(tableName);

    if (v_short.size() != database.tables[i].c_short.size()) {
        throw std::invalid_argument("INSERT ROW: nb values doesn't match nb of column");
    }

    for (int j = 0; j < v_short.size(); ++j) {
        database.tables[i].c_short[j].values.push_back(std::move(v_short[j]));
    }
    
    return;
}


Table * select_where(vector<Tfhe<short>>& cipher_result, string tableName, string columnName, Tfhe<short>& condition_cipher, string op, const TFheGateBootstrappingCloudKeySet* bk) {

    int i = database.getTableIndex(tableName);
    
    int j = database.tables[i].getColumnIndex(columnName);

    for (int l = 0; l < database.tables[i].c_short[j].values.size(); ++l) {
        if (op == "==" || op == "=") {
            XLS_CHECK_OK(equality_short(cipher_result[l], database.tables[i].c_short[j].values[l], condition_cipher, bk));
        } else if (op == ">") {
            XLS_CHECK_OK(superior_short(cipher_result[l], database.tables[i].c_short[j].values[l], condition_cipher, bk));
        } else if (op == "<") {
            XLS_CHECK_OK(inferior_short(cipher_result[l], database.tables[i].c_short[j].values[l], condition_cipher, bk));
        } else if (op == "!=") {
            XLS_CHECK_OK(inequality_short(cipher_result[l], database.tables[i].c_short[j].values[l], condition_cipher, bk));
        } else if (op == ">=") {
            XLS_CHECK_OK(superior_or_equal_short(cipher_result[l], database.tables[i].c_short[j].values[l], condition_cipher, bk));
        } else if (op == "<=") {
            XLS_CHECK_OK(inferior_or_equal_short(cipher_result[l], database.tables[i].c_short[j].values[l], condition_cipher, bk));
        }      
    }
    
    return &database.tables[i];
}


Table * select_distinct(vector<Tfhe<short>>& cipher_result, vector<Tfhe<short>>& cipher_tmp, string tableName, string columnName, const TFheGateBootstrappingCloudKeySet* bk) {
    
    int i = database.getTableIndex(tableName);
    int j = database.tables[i].getColumnIndex(columnName);

    short table_size = database.tables[i].c_short[j].values.size();
    
    for (int k = 1; k < table_size; ++k) {
        for (int l = k - 1; l >= 0; --l) {
            if (l == (k - 1)) {
                XLS_CHECK_OK(inequality_short(cipher_result[k], database.tables[i].c_short[j].values[l], database.tables[i].c_short[j].values[k], bk));
            } else {
                XLS_CHECK_OK(inequality_short(cipher_tmp[k], database.tables[i].c_short[j].values[l], database.tables[i].c_short[j].values[k], bk));
                XLS_CHECK_OK(tfhe_and_short(cipher_result[k], cipher_tmp[k], cipher_result[k], bk));
            }
        }
    }

    return &database.tables[i];
}

int simple_count(string tableName) {
    int index = database.getTableIndex(tableName);
    return database.tables[index].getNbRow();
}


void tfhe_count(Tfhe<short>& cipher_result, vector<Tfhe<short>>& cipher_index, string tableName, const TFheGateBootstrappingCloudKeySet* bk) {

    for (int i = 0; i < cipher_index.size(); ++i) {
        XLS_CHECK_OK(tfhe_sum_short(cipher_result, cipher_result, cipher_index[i], bk));
    }

}


void sum(Tfhe<short>& cipher_result, string tableName, string columnName, const TFheGateBootstrappingCloudKeySet* bk) {

    int i = database.getTableIndex(tableName);
    int j = database.tables[i].getColumnIndex(columnName);

    int table_size = database.tables[i].c_short[j].values.size();

    if (table_size > 1) {
        XLS_CHECK_OK(tfhe_sum_short(cipher_result, database.tables[i].c_short[j].values[0], database.tables[i].c_short[j].values[1], bk));
        
        for (int k = 2; k < table_size; ++k) {
            XLS_CHECK_OK(tfhe_sum_short(cipher_result, cipher_result, database.tables[i].c_short[j].values[k], bk));
        }
    } else {
        XLS_CHECK_OK(inequality_short(cipher_result, cipher_result, cipher_result, bk));
        XLS_CHECK_OK(tfhe_sum_short(cipher_result, cipher_result, database.tables[i].c_short[j].values[0], bk));
    }

}


void avg(Tfhe<short>& cipher_r, Tfhe<short>& cipher_result, vector<Tfhe<short>>& cipher_tmp, string tableName, string columnName, const TFheGateBootstrappingCloudKeySet* bk) {
    
    int i = database.getTableIndex(tableName);
    int j = database.tables[i].getColumnIndex(columnName);

    int table_size = database.tables[i].c_short[j].values.size();

    for(int k = 0; k < table_size; ++k) {
        XLS_CHECK_OK(equality_short(cipher_tmp[k], database.tables[i].c_short[j].values[k], database.tables[i].c_short[j].values[k], bk));
        if (k > 0) {
            XLS_CHECK_OK(tfhe_sum_short(cipher_tmp[0], cipher_tmp[0], cipher_tmp[k], bk));
        }
    }

    if (table_size > 1) {
        XLS_CHECK_OK(tfhe_sum_short(cipher_result, database.tables[i].c_short[j].values[0], database.tables[i].c_short[j].values[1], bk));
        
        for (int k = 2; k < table_size; ++k) {
            XLS_CHECK_OK(tfhe_sum_short(cipher_result, cipher_result, database.tables[i].c_short[j].values[k], bk));
        }
    } else {
        XLS_CHECK_OK(inequality_short(cipher_result, cipher_result, cipher_result, bk));
        XLS_CHECK_OK(tfhe_sum_short(cipher_result, cipher_result, database.tables[i].c_short[j].values[0], bk));
    }

    XLS_CHECK_OK(tfhe_div_short(cipher_r, cipher_result, cipher_tmp[0], bk));

}


tuple<int, int> inner_join(vector<Tfhe<short>>& cipher_result, string tableName1, string tableName2, string columnName1, string columnName2, const TFheGateBootstrappingCloudKeySet* bk) {
    
    int table_index1 = database.getTableIndex(tableName1);
    int table_index2 = database.getTableIndex(tableName2);

    int column_index_1 = database.tables[table_index1].getColumnIndex(columnName1);
    int column_index_2 = database.tables[table_index2].getColumnIndex(columnName2);

    int k = 0;
    int table_1_size = database.tables[table_index1].c_short[column_index_1].values.size();
    int table_2_size = database.tables[table_index2].c_short[column_index_2].values.size();

    for (int i = 0; i < table_1_size; ++i) {
        for (int j = 0; j < table_2_size; ++j) {
            XLS_CHECK_OK(equality_short(cipher_result[k], database.tables[table_index1].c_short[column_index_1].values[i], database.tables[table_index2].c_short[column_index_2].values[j], bk));
            ++k;
        }
    }

    return std::make_tuple(table_1_size, table_2_size);

}


void sum_where(Tfhe<short>& cipher_result, vector<Tfhe<short>>& cipher_tmp, vector<Tfhe<short>>& cipher_tmp_2, string tableName, string columnName, Tfhe<short>& condition_cipher, string op, const TFheGateBootstrappingCloudKeySet* bk) {
    int index = database.getTableIndex(tableName);
    int size = database.tables[index].getNbRow();

    int column_index = database.tables[index].getColumnIndex(columnName);

    select_where(cipher_tmp, tableName, columnName, condition_cipher, op, bk);

    for (int i = 0; i < size; ++i) {
        XLS_CHECK_OK(tfhe_product_short(cipher_tmp_2[i], cipher_tmp[i], database.tables[index].c_short[column_index].values[i], bk));
    }

    if (size > 1) {
        XLS_CHECK_OK(tfhe_sum_short(cipher_result, cipher_tmp_2[0], cipher_tmp_2[1], bk));
        
        for (int k = 2; k < size; ++k) {
            XLS_CHECK_OK(tfhe_sum_short(cipher_result, cipher_result, cipher_tmp_2[k], bk));
        }
    } else {
        XLS_CHECK_OK(inequality_short(cipher_result, cipher_tmp_2[0], cipher_tmp_2[0], bk));
        XLS_CHECK_OK(tfhe_sum_short(cipher_result, cipher_result, cipher_tmp_2[0], bk));
    }
}


void count_where(Tfhe<short>& cipher_result, vector<Tfhe<short>>& cipher_tmp, string tableName, string columnName, Tfhe<short>& condition_cipher, string op, const TFheGateBootstrappingCloudKeySet* bk) {

    select_where(cipher_tmp, tableName, columnName, condition_cipher, op, bk);

    int index = database.getTableIndex(tableName);
    int size = database.tables[index].getNbRow();

    if (size > 1) {
        XLS_CHECK_OK(tfhe_sum_short(cipher_result, cipher_tmp[0], cipher_tmp[1], bk));
        
        for (int k = 2; k < size; ++k) {
            XLS_CHECK_OK(tfhe_sum_short(cipher_result, cipher_result, cipher_tmp[k], bk));
        }
    } else {
        XLS_CHECK_OK(inequality_short(cipher_result, cipher_tmp[0], cipher_tmp[0], bk));
        XLS_CHECK_OK(tfhe_sum_short(cipher_result, cipher_result, cipher_tmp[0], bk));
    }

}


#endif //SGBD_SERVER_H_
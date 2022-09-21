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
#include "api/api_equality_int.h"
#include "api/api_inequality_int.h"
#include "api/api_sum.h"
#include "tfhe/tfhe.h"
#include "tfhe/tfhe_io.h"
#include "transpiler/data/tfhe_data.h"
#include "xls/common/logging/logging.h"

#include "transpiler/examples/sgbd/utils.h"

#include "transpiler/examples/sgbd/api/api_and.h"
#include "transpiler/examples/sgbd/api/api_sum.h"
#include "transpiler/examples/sgbd/api/api_div.h"
#include "transpiler/examples/sgbd/api/api_equality_int.h"
#include "transpiler/examples/sgbd/api/api_equality_char.h"
#include "transpiler/examples/sgbd/api/api_inequality_int.h"
#include "transpiler/examples/sgbd/api/api_superior_int.h"
#include "transpiler/examples/sgbd/api/api_inferior_int.h"

#include "transpiler/examples/sgbd/api_tfhe_and.h"
#include "transpiler/examples/sgbd/api_tfhe_sum.h"
#include "transpiler/examples/sgbd/api_tfhe_div.h"
#include "transpiler/examples/sgbd/api_tfhe_equality_int.h"
#include "transpiler/examples/sgbd/api_tfhe_equality_char.h"
#include "transpiler/examples/sgbd/api_tfhe_inequality_int.h"
#include "transpiler/examples/sgbd/api_tfhe_superior_int.h"
#include "transpiler/examples/sgbd/api_tfhe_inferior_int.h"


using namespace std;

extern Database database;



// CREATE
void create_table(string tableName, vector<tuple<string, string>>& columnName_and_type) {
    
    Table t = {
        tableName,
        //(int)columnName_and_type.size(),
        //columnName_and_type,
        {},
    };
    
    // Construire chaque colonne
    for (auto i : columnName_and_type) {

        Column<Tfhe<int>> c = {
            tableName,
            get<0>(i),
            get<1>(i),
            {},
        };
        t.c_int.push_back(move(c));
    }

    database.tables.push_back(move(t));

    return;
}


// INSERT
void insert_row(string tableName, vector<Tfhe<int>>& v_int) {

    int i = database.getTableIndex(tableName);

    // vérifier si même nombre d'argument que de colonne
    if (v_int.size() != database.tables[i].c_int.size()) {
        throw invalid_argument("INSERT ROW: nb values doesn't match nb of column");
    }

    for (int j = 0; j < v_int.size(); ++j) {
        database.tables[i].c_int[j].values.push_back(move(v_int[j]));
    }
    
    return;
}

// SELECT
template<typename T>
Table * select_where(vector<Tfhe<int>>& cipher_result, string tableName, string columnName, T& condition_cipher, string op, const TFheGateBootstrappingCloudKeySet* bk) {

    int i = database.getTableIndex(tableName);
    
    int j = database.tables[i].getColumnIndex(columnName);

    //string type = get<1>(database.tables[i].columnName_and_type[j]);

    for (int l = 0; l < database.tables[i].c_int[j].values.size(); ++l) {
        if (op == "==") {
            XLS_CHECK_OK(equality_int(cipher_result[l], database.tables[i].c_int[j].values[l], condition_cipher, bk));
        } else if (op == ">") {
            XLS_CHECK_OK(superior_int(cipher_result[l], database.tables[i].c_int[j].values[l], condition_cipher, bk));
        } else if (op == "<") {
            XLS_CHECK_OK(inferior_int(cipher_result[l], database.tables[i].c_int[j].values[l], condition_cipher, bk));
        } else if (op == "!=") {
            XLS_CHECK_OK(inequality_int(cipher_result[l], database.tables[i].c_int[j].values[l], condition_cipher, bk));
        }        
    }
    
    return &database.tables[i];
}


Table * select_distinct(vector<Tfhe<int>>& cipher_result, vector<Tfhe<int>>& cipher_tmp, string tableName, string columnName, const TFheGateBootstrappingCloudKeySet* bk) {
    int i = database.getTableIndex(tableName);
    
    int j = database.tables[i].getColumnIndex(columnName);

    int table_size = database.tables[i].c_int[j].values.size();

    // TODO: check if table is empty

    //XLS_CHECK_OK(equality_int(cipher_tmp[0], database.tables[i].c_int[j].values[0], database.tables[i].c_int[j].values[0], bk));
    
    for (int k = 1; k < table_size; ++k) {
        for (int l = k - 1; l >= 0; --l) {
            if (l == k) {
                XLS_CHECK_OK(inequality_int(cipher_result[k], database.tables[i].c_int[j].values[l], database.tables[i].c_int[j].values[k], bk));
            } else {
                XLS_CHECK_OK(inequality_int(cipher_tmp[k], database.tables[i].c_int[j].values[l], database.tables[i].c_int[j].values[k], bk));
                XLS_CHECK_OK(tfhe_and(cipher_result[k], cipher_tmp[k], cipher_result[k], bk));
            }
        }
    }

    return &database.tables[i];
}

void tfhe_count(Tfhe<int>& cipher_result, vector<Tfhe<int>>& cipher_index, string tableName, const TFheGateBootstrappingCloudKeySet* bk) {

    for (int i = 0; i < cipher_index.size(); ++i) {
        XLS_CHECK_OK(tfhe_sum(cipher_result, cipher_result, cipher_index[i], bk));
    }

}

void sum(Tfhe<int>& cipher_result, string tableName, string columnName, const TFheGateBootstrappingCloudKeySet* bk) {

    int i = database.getTableIndex(tableName);
    int j = database.tables[i].getColumnIndex(columnName);

    int table_size = database.tables[i].c_int[j].values.size();

    // TODO: check if table is empty

    if (table_size > 1) {
        XLS_CHECK_OK(tfhe_sum(cipher_result, database.tables[i].c_int[j].values[0], database.tables[i].c_int[j].values[1], bk));
        
        for (int k = 2; k < table_size; ++k) {
            XLS_CHECK_OK(tfhe_sum(cipher_result, cipher_result, database.tables[i].c_int[j].values[k], bk));
        }
    } else {
        XLS_CHECK_OK(inequality_int(cipher_result, cipher_result, cipher_result, bk));
        XLS_CHECK_OK(tfhe_sum(cipher_result, cipher_result, database.tables[i].c_int[j].values[0], bk));
    }

}


void avg(Tfhe<int>& cipher_r, Tfhe<int>& cipher_result, vector<Tfhe<int>>& cipher_tmp, string tableName, string columnName, const TFheGateBootstrappingCloudKeySet* bk) {
    
    int i = database.getTableIndex(tableName);
    int j = database.tables[i].getColumnIndex(columnName);

    int table_size = database.tables[i].c_int[j].values.size();

    for(int k = 0; k < table_size; ++k) {
        XLS_CHECK_OK(equality_int(cipher_tmp[k], database.tables[i].c_int[j].values[k], database.tables[i].c_int[j].values[k], bk));
        if (k > 0) {
            XLS_CHECK_OK(tfhe_sum(cipher_tmp[0], cipher_tmp[0], cipher_tmp[k], bk));
        }
    }

    if (table_size > 1) {
        XLS_CHECK_OK(tfhe_sum(cipher_result, database.tables[i].c_int[j].values[0], database.tables[i].c_int[j].values[1], bk));
        
        for (int k = 2; k < table_size; ++k) {
            XLS_CHECK_OK(tfhe_sum(cipher_result, cipher_result, database.tables[i].c_int[j].values[k], bk));
        }
    } else {
        XLS_CHECK_OK(inequality_int(cipher_result, cipher_result, cipher_result, bk));
        XLS_CHECK_OK(tfhe_sum(cipher_result, cipher_result, database.tables[i].c_int[j].values[0], bk));
    }

    XLS_CHECK_OK(tfhe_div(cipher_r, cipher_result, cipher_tmp[0], bk));

}


// JOIN
tuple<int, int> inner_join(vector<Tfhe<int>>& cipher_result, string tableName1, string tableName2, string columnName1, string columnName2, const TFheGateBootstrappingCloudKeySet* bk) {
    
    // TODO: vérifier si columnName2 est une clé étrangère de columnName1

    int table_index1 = database.getTableIndex(tableName1);
    int table_index2 = database.getTableIndex(tableName2);

    int column_index_1 = database.tables[table_index1].getColumnIndex(columnName1);
    int column_index_2 = database.tables[table_index2].getColumnIndex(columnName2);

    int k = 0;
    int table_1_size = database.tables[table_index1].c_int[column_index_1].values.size();
    int table_2_size = database.tables[table_index2].c_int[column_index_2].values.size();

    for (int i = 0; i < table_1_size; ++i) {
        for (int j = 0; j < table_2_size; ++j) {
            XLS_CHECK_OK(equality_int(cipher_result[k], database.tables[table_index1].c_int[column_index_1].values[i], database.tables[table_index2].c_int[column_index_2].values[j], bk));
            ++k;
        }
    }

    return make_tuple(table_1_size, table_2_size);

}

/*
void query_handler(vector<Tfhe<int>>& cipher_result, vector<vector<Tfhe<int>>>& cipher_tmp, vector<Query> queries, const TFheGateBootstrappingCloudKeySet* bk) {

    // TODO: verifier si asser de taille de cipher_result

    for (int i = 0; i < queries.size(); ++i) {
        if (queries[i].name == "select_where") {
            select_where(cipher_result, queries[i].tablesNames[0], queries[i].arg[0], queries[i].arg[1], queries[i].op, bk);

        } else if (queries[i].name == "count") {

        } else if (queries[i].name == "avg") {

        } else if (queries[i].name == "sum") {

        } else if (queries[i].name == "inner_join") {

        } else {
            // throw error
        }
    }


}
*/
// DELETE
/*
void delete() {

}
*/
// UPDATE

/*
void update(vector<Tfhe<int>>& cipher_tmp, ) {
    
    
}
*/
// SHOW TABLE

#endif //SGBD_SERVER_H_

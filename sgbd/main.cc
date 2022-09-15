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
#include <utility>

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
constexpr int dbSize = 5;
const int SPACE = 10;

// Random seed for key generation
// Note: In real applications, a cryptographically secure seed needs to be used.
constexpr std::array<uint32_t, 3> kSeed = {314, 1592, 657};


using namespace std;

//printf("Current working dir: %s\n", get_current_dir_name());

void insert(string tableName, const vector<int>& v_int, const vector<char>& v_char, TFHESecretKeySet& key) {
    vector<Tfhe<int>> v_int_cipher = {};
    vector<Tfhe<char>> v_char_cipher = {};

    for(int i = 0; i < v_int.size(); ++i) {
        v_int_cipher.push_back(Tfhe<int>::Encrypt(v_int[i], key));
    }

    for(int i = 0; i < v_char.size(); ++i) {
        v_char_cipher.push_back(Tfhe<char>::Encrypt(v_char[i], key));
    }

    insert_row(tableName, v_int_cipher, v_char_cipher);

    return;
}

void create_database(TFHESecretKeySet& key) {

    // CREATE Table Employes (ID, nom, age)
    vector<tuple<string, string>> v = {make_tuple("ID", "int"), make_tuple("nom", "char"), make_tuple("age", "int")};

    create_table("Employes", v);

    // INSERT INTO Employes VALUES ([0], [b], [32]) 

    int v1 = 0;
    char v2 = 'b';
    int v3 = 32;

    vector<int> v_int = {};
    vector<char> v_char = {};

    v_int.push_back(v1);
    v_char.push_back(v2);
    v_int.push_back(v3);

    insert("Employes", v_int, v_char, key);

    // INSERT INTO Employes VALUES ([1], [a], [27]) 

    int v4 = 1;
    char v5 = 'a';
    int v6 = 27;

    v_int = {};
    v_char = {};

    v_int.push_back(v4);
    v_char.push_back(v5);
    v_int.push_back(v6);

    insert("Employes", v_int, v_char, key);

    // INSERT INTO Employes VALUES ([2], [m], [32]]) 

    v1 = 2;
    v2 = 'm';
    v3 = 32;

    v_int = {};
    v_char = {};

    v_int.push_back(v1);
    v_char.push_back(v2);
    v_int.push_back(v3);

    insert("Employes", v_int, v_char, key);

    // INSERT INTO Employes VALUES ([3], [l], [49]) 
/*
    v1 = 3;
    v2 = 'l';
    v3 = 49;

    v_int = {};
    v_char = {};

    v_int.push_back(v1);
    v_char.push_back(v2);
    v_int.push_back(v3);

    insert("Employes", v_int, v_char, key);
*/

    // CREATE Table Departements (ID, nom, employe_id)
    v = {make_tuple("ID", "int"), make_tuple("nom", "char"), make_tuple("employe_id", "int")};

    create_table("Departements", v);

    // INSERT INTO Departements VALUES ([0], [f], [3]) 

    v1 = 0;
    v2 = 'f';
    v3 = 2;

    v_int = {};
    v_char = {};

    v_int.push_back(v1);
    v_char.push_back(v2);
    v_int.push_back(v3);

    insert("Departements", v_int, v_char, key);

    // INSERT INTO Departements VALUES ([1], [g], [0]) 

    v1 = 1;
    v2 = 'g';
    v3 = 0;

    v_int = {};
    v_char = {};

    v_int.push_back(v1);
    v_char.push_back(v2);
    v_int.push_back(v3);

    insert("Departements", v_int, v_char, key);

}

void display_result(string tableName, vector<vector<string>> row, int nb_columns) {

    cout << tableName << "\n";

    for (int i = 0; i < nb_columns; i++) {
        cout << "+";
        for (int j = 0; j <= SPACE; j++) {
            cout << "-";
        }
    }
    cout << "+" << endl;

    for (int i = 0; i < nb_columns; ++i) {
        cout << "|" << setw(SPACE) << right << row[0][i] << " ";
    }

    cout << "|" << endl;

    for (int i = 0; i < nb_columns; i++) {
        cout << "+";
        for (int j = 0; j <= SPACE; j++) {
            cout << "-";
        }
    }

    cout << "+" << endl;

    for (int i = 1; i < row.size(); ++i) {
        for (int j = 0; j < nb_columns; ++j) {
            cout << "|" << setw(SPACE) << right << row[i][j] << " ";
        }
        cout << "|" << endl;
    }

    for (int i = 0; i < nb_columns; i++) {
        cout << "+";
        for (int j = 0; j <= SPACE; j++) {
            cout << "-";
        }
    }
    cout << "+" << endl;
}


void query_where(TFHESecretKeySet& key, TFHEParameters& params) {
/*
    // get tableName
    string tableName;
    cout << "\nTable name: ";
    cin >> tableName;

    // get columnName
    string columnName;
    cout << "\nColumn name: ";
    cin >> columnName;

    // get op
    string op;
    cout << "\nOperator: ";
    cin >> op;

    // get condition
    int condition;
    cout << "\nConditon: ";
    cin >> condition;

*/
    string tableName = "Employes";
    string columnName = "age";
    string op = "==";
    int condition = 32;


    auto condition_cipher = Tfhe<int>::Encrypt(condition, key);

    vector<Tfhe<int>> cipher_result = {};

    for (int i = 0; i < dbSize; ++i) {
        Tfhe<int> index(params);
        cipher_result.push_back(move(index));
    }

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();
    
    int size = select_where(cipher_result, tableName, columnName, condition_cipher, op, key.cloud());

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    int tableIndex = database.getTableIndex(tableName);

    vector<vector<string>> result_row = {};
    vector<string> colNames = database.tables[tableIndex].getColumnName();
    int nb_col = colNames.size();

    result_row.push_back(colNames);

    for (int i = 0; i < size; ++i) {
        if (cipher_result[i].Decrypt(key) == 1) {
            vector<string> v = database.tables[tableIndex].get_row(i, key);
            result_row.push_back(v);
        }    
    }


    cout << "\nResult:\n";
    display_result("Employes", result_row, nb_col);



    cout << "\t\t\t\t\tComputation done" << endl;
    cout << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << endl;
    cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << endl;

    return;
}

void query_sum(TFHESecretKeySet& key, TFHEParameters& params) {

    string tableName = "Employes";
    string columnName = "age";

    Tfhe<int> cipher_result(params);

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();
    
    sum(cipher_result, tableName, columnName, key.cloud());

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    cout << "Result: \n";
    auto r = cipher_result.Decrypt(key);
    cout << r << "\n";

    cout << "\t\t\t\t\tComputation done" << endl;
    cout << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << endl;
    cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << endl;

    return;
}

void query_distinct(TFHESecretKeySet& key, TFHEParameters& params) {
    string tableName = "Employes";
    string columnName = "age";

    vector<Tfhe<int>> cipher_result = {};

    for (int i = 0; i < dbSize; ++i) {
        Tfhe<int> index(params);
        cipher_result.push_back(move(index));
    }

    vector<Tfhe<int>> cipher_tmp = {};

    for (int i = 0; i < dbSize; ++i) {
        Tfhe<int> index(params);
        cipher_tmp.push_back(move(index));
    }


    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();
    
    int size = select_distinct(cipher_result, cipher_tmp, tableName, columnName, key.cloud());

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();


    int tableIndex = database.getTableIndex(tableName);

    vector<vector<string>> result_row = {};
    vector<string> colNames = database.tables[tableIndex].getColumnName();
    int nb_col = colNames.size();

    result_row.push_back(colNames);

    for (int i = 0; i < size; ++i) {
        if (i == 0) {
            vector<string> v = database.tables[tableIndex].get_row(i, key);
            result_row.push_back(v);
        } else {
            if (cipher_result[i].Decrypt(key) == 1) {
                vector<string> v = database.tables[tableIndex].get_row(i, key);
                result_row.push_back(v);
            }    
        }
    }

    cout << "\nResult:\n";
    display_result(tableName, result_row, nb_col);



    cout << "\t\t\t\t\tComputation done" << endl;
    cout << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << endl;
    cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << endl;

    return;
}

void query_innerjoin(TFHESecretKeySet& key, TFHEParameters& params) {
    string tableName_1= "Employes";
    string columnName_1 = "ID";
    string tableName_2 = "Departements";
    string columnName_2 = "employe_id";

    vector<Tfhe<int>> index_cipher = {};

    for (int i = 0; i < dbSize * dbSize; ++i) {
        Tfhe<int> index(params);
        index_cipher.push_back(move(index));
    }

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();
    
    tuple<int, int> size = inner_join(index_cipher, tableName_1, tableName_2, columnName_1, columnName_2, key.cloud());

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    cout << "Result: \n";

    for (int i = 0; i < get<0>(size) * get<1>(size); ++i) {
        if (index_cipher[i].Decrypt(key) == 1) {
            cout << "1\n";
        } else {
            cout << "0\n";
        }
    }



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
    for (int i = 0; i < get<0>(size) * get<1>(size); ++i) {

        if ((i != 0) && ((i % get<1>(size)) == 0)) {
            ++k;
        } 
        if (index_cipher[i].Decrypt(key) == 1) {
            vector<string> v1 = database.tables[tableIndex_1].get_row(k, key);
            vector<string> v2 = database.tables[tableIndex_2].get_row(i % get<1>(size), key);

            vector<string> vec;
            vec.resize(v1.size() + v2.size());
            std::move(v1.begin(), v1.end(), vec.begin());
            std::move(v2.begin(), v2.end(), vec.begin() + v1.size());
            result_row.push_back(vec);
        }
  
    }

    cout << "\nResult:\n";
    display_result(tableName_1 + " " + tableName_2, result_row, nb_col_1 + nb_col_2);


    cout << "\t\t\t\t\tComputation done" << endl;
    cout << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << endl;
    cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << endl;

    return;
}

void query_avg(TFHESecretKeySet& key, TFHEParameters& params) {
    string tableName = "Employes";
    string columnName = "age";


    vector<Tfhe<int>> cipher_tmp = {};
    for (int i = 0; i < dbSize; ++i) {
        Tfhe<int> index(params);
        cipher_tmp.push_back(move(index));
    }

    Tfhe<int> cipher_result(params);
    Tfhe<int> cipher_r(params);

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();

    avg(cipher_result, cipher_r, cipher_tmp, tableName, columnName, key.cloud());
    
    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    cout << "Result: \n";
    auto r = cipher_result.Decrypt(key);
    cout << r << "\n";

    cout << "\t\t\t\t\tComputation done" << endl;
    cout << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << endl;
    cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << endl;

    return;
}

void show_tables(TFHESecretKeySet& key) {
    
    int size = database.tables.size();

    for (int i = 0; i < size; ++i) {
        string tableName = database.tables[i].name;
        
        vector<vector<string>> result_row = {};
        vector<string> colNames = database.tables[i].getColumnName();
        result_row.push_back(colNames);
        int nb_col = database.tables[i].nbColumn;
        int nb_row = database.tables[i].c_int[0].values.size();

        for (int j = 0; j < nb_row; ++j) {
            vector<string> v = database.tables[i].get_row(j, key);
            result_row.push_back(v);
        }

        cout << "\n";
        display_result(tableName, result_row, nb_col);
    }

} 

int main() {

    TFHEParameters params(kMainMinimumLambda);
    TFHESecretKeySet key(params, kSeed);

    create_database(key);

    while (true) {
        int input;
        cout << "\nChoose one of the following options:\n";
        cout << "1. SELECT * FROM tableName WHERE condition\n";
        cout << "2. SELECT SUM(column) FROM tableName\n";
        cout << "3. SELECT DISTINCT FROM tableName\n";
        cout << "4. SELECT * FROM tableName_1 INNER JOIN tableName_2\n";
        cout << "5. SELECT AVG(column) FROM tableName\n";
        cout << "6. SHOW tables\n";
        cout << "7. quit\n";
        cin >> input;

        switch (input) {
            case 1:
                query_where(key, params);
                break;
            case 2:
                query_sum(key, params);
                break;
            case 3:
                query_distinct(key, params);
                break;
            case 4:
                query_innerjoin(key, params);
                break;
            case 5:
                query_avg(key, params);
                break;
            case 6: 
                show_tables(key);
                break;
            case 7:
                return 0;
            default:
                cout << "Invalid number\n\n";
        }
    }

    return 0;
}

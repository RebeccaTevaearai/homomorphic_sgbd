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
//#include "transpiler/examples/sgbd_join/api_tfhe.h"
//#include "transpiler/examples/sgbd_join/api.h"

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
/*
    // INSERT INTO Employes VALUES ([2], [m], [18]) 

    v1 = 2;
    v2 = 'm';
    v3 = 18;

    v_int = {};
    v_char = {};

    v_int.push_back(v1);
    v_char.push_back(v2);
    v_int.push_back(v3);

    insert("Employes", v_int, v_char, key);

    // INSERT INTO Employes VALUES ([3], [l], [49]) 

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
    v3 = 3;

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

int main() {

    TFHEParameters params(kMainMinimumLambda);
    TFHESecretKeySet key(params, kSeed);

    create_database(key);

    // result index
    vector<Tfhe<int>> index_cipher = {};

    for (int i = 0; i < dbSize * dbSize; ++i) {
        Tfhe<int> index(params);
        index_cipher.push_back(move(index));
    }

    absl::Time start_time = absl::Now();
    double cpu_start_time = clock();
    
    tuple<int, int> size = inner_join(index_cipher, "Employes", "Departements", "ID", "employe_id", key.cloud());

    double cpu_end_time = clock();
    absl::Time end_time = absl::Now();

    for (int i = 0; i < get<0>(size) * get<1>(size); ++i) {
        cout << index_cipher[i].Decrypt(key) << "\n";
    }

    /*
    int tableIndex = database.getTableIndex("Employes");

    vector<vector<string>> result_row = {};
    vector<string> columnName = database.tables[tableIndex].getColumnName();
    int nb_col = columnName.size();
    result_row.push_back(columnName);

    for (int i = 0; i < nb_row; ++i) {
        if (index_cipher[i].Decrypt(key) == 1) {
            vector<string> v = database.tables[tableIndex].get_row(i, key);
            result_row.push_back(v);
        }    
    }

    cout << "\nResult:\n";

    display_result("Employes", result_row, nb_col);
    */

    cout << "\t\t\t\t\tComputation done" << endl;
    cout << "\t\t\t\t\tTotal time: "
        << absl::ToDoubleSeconds(end_time - start_time) << " secs" << endl;
    cout << "\t\t\t\t\t  CPU time: "
        << (cpu_end_time - cpu_start_time) / 1'000'000 << " secs" << endl;
  
/*
    while (true) {
        string tmp;
        string table_name;
        string column_name;
        unsigned char op;
        unsigned char condition;

        cout << "\nSELECT *";
        cout << "\nFROM ";
        cin >> table_name;
        cout << "\nWHERE ";
        cin >> column_name;
        cin >> op;
        cin >> condition;

        // TODO: do that from server side
        
        vector<TfheRecordT> column_cipher;

        for (auto t: database.tables) {
            if (t.name == table_name) {
                for (auto c : t.column) {
                    if (c.name == column_name) {
                        column_cipher = c.values;
                    } else {
                    cout << "\ncolumn doesn't exist";
                    continue;
                    }
                } 
            } else {
                cout << "\ntable doesn't exist";
                continue;
            }
        }
            
    }
*/

    return 0;
}
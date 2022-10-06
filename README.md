# Homomorphic sgbd prototype

Prototype of an SGBD using fully homomorphic encryption written in C++. The library [google/fully-homomorphic-encryption](https://github.com/google/fully-homomorphic-encryption) is used to transform some part of the C++ code into evaluation circuit supporting homomorphic computation. The library used in this project for the homomorphic operation is the [TFHE](https://github.com/tfhe/tfhe). 

The google library only work on linux distribution and personnaly I only managed to make it work on an debian distribution (kali-linux).

## Installation
1. Install the library [google/fully-homomorphic-encryption](https://github.com/google/fully-homomorphic-encryption). Follow the instructions of the README in the [/transpiler](https://github.com/google/fully-homomorphic-encryption/tree/main/transpiler) folder.
2. Download the file on this repository and move them in the /transpiler/examples folder of the library.
3. To run the prototype, go to the fully-homomorphic-encryption/ directory, and run the following command: 
```
bazel run //transpiler/examples/sgbd:sgbd
```


## Usage
To run the prototype using encrypted int values
```
bazel run //transpiler/examples/sgbd:sgbd
```
  
To run the prototype using encrypted short values
```
bazel run //transpiler/examples/sgbd_short:sgbd_short
```
  
To run the prototype using cleartext values
```
bazel run //transpiler/examples/sgbd_cleartext:sgbd_cleartext
```

## Script
A script can be loaded with the `load script` function to create a database, enter the full path of the file when prompted. Use one of the default script contained in the `/data` folder or create a new one. The script need to respect the following format: 

```
CREATE TABLE tableName (columnName1 type, columnName2 type, ...);

INSERT INTO tableName VALUES (v1, v2, ...);
```

The type of the column can be either `int` or `char` for the prototype using encrypted int and cleartext, and `short`or `char` for the prototype using encrypted short.

`Char` value must be declare with single quote: 'v'.

The prototype does not manage constraint like primary key and other, therefore they cannot be declare in the script.

## Query
To start using queries, choose the `query` function and enter the query on one line, with a `;` to end it.

#### CREATE TABLE tableName (columnName1 type, columnName2 type, ...);
Create a new table in the database. There is no duplicate verification, do not create two tables with the same name.

Example: 
```
CREATE TABLE Employes (ID int, nom char, age int);
```

#### INSERT INTO tableName VALUES (v1, v2, ...);
Insert a new row in a table. There is no verification for unique value or other type of constraint.

Example: 
```
INSERT INTO Employes VALUES (0, 'b', 32);
```

#### SELECT (*) FROM tableName WHERE columnName op condition;` 
Return the rows from a table that satisfy a condition.

Only `(*)` can be used. The operator `op` can be one of the following: `=, ==, !=, >, <, >=, <=`. `columnName` `op` and `condition` need to be seperate by a space. If the condition is a `char`, value must be declare with single quote. 

Example: 
```
SELECT (*) FROM Employes WHERE age = 32;
```

```
SELECT (*) FROM Employes WHERE nom > 'a';
```

#### SELECT DISTINCT(columnName) FROM tableName;

Example: 
```
SELECT DISTINCT(age) FROM Employes;
```

#### SELECT (*) FROM tableName1 INNER JOIN tableName2 ON columnName1 = columnName2;

Only `(*)` can be used. There is no verification to check if columnName2 is a foreign key of columnName1, the user is responsible for choosing the right column.

Example: 
```
SELECT (*) FROM Employes INNER JOIN Departements ON ID = employe_id;
```

#### SELECT SUM(columnName) FROM tableName;
Return the sum of the value of a column.


Example: 
```
SELECT SUM(age) FROM Employes;
```

#### SELECT AVG(columnName) FROM tableName;
Return the average value of a column.


Example: 
```
SELECT AVG(age) FROM Employes;
```

#### SELECT SUM(columnName) FROM tableName WHERE columnName op condition;
Return the sum of a column of all the row that satisfy a certain condition.

Example: 
```
SELECT SUM(age) FROM Employes WHERE nom > 'a';
```

#### SELECT COUNT(*) FROM tableName WHERE columnName op condition;
Return the number of row that satisfy a condition. Only `(*)` can be used.

Example: 
```
SELECT COUNT(*) FROM Employes WHERE age <= 32;
```

## Testing
There is three different test for the queries that can be run, only the size of the data change.
```
bazel run //transpiler/examples/[sgbd|sgbd_short|sgbd_cleartext]:[performance_test|performance_test_big|performance_test_verybig]
```

To test the evaluation circuit, run the foloowing command:
```
bazel run //transpiler/examples/[sgbd|sgbd_short|sgbd_cleartext]:api_test

```


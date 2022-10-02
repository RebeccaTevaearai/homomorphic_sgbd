# Homomorphic sgbd prototype

## Installation
1. Install the library [google/fully-homomorphic-encryption](https://github.com/google/fully-homomorphic-encryption). Follow the instructions of the README in the [/transpiler](https://github.com/google/fully-homomorphic-encryption/tree/main/transpiler) folder.
2. Download the file on this repository and move them in the /transpiler/examples folder of the library.
3. To run the prototype, go to the fully-homomorphic-encryption/ directory, and run the following command: 
```
bazel run //transpiler/examples/sgbd:sgbd
```


## Commande
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
  
There is three different test that can be run, only the size of the data change.
```
bazel run //transpiler/examples/[sgbd | sgbd_short | sgbd_cleartext]:[performance_test | performance_test_big | performance_test_verybig]
```

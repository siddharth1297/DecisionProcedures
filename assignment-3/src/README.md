### Setup
* **Bison Version:** atleast 3.5 (strictly required).

### Run
```
$ cd src/
$ make
$ ./main test/1.txt
```

### Development Steps
- [x] Generate AST
- [x] Generate DAG from AST
- [x] Apply Union-Find
- [x] Check against negation or not equal
- [ ] Add New test cases

### Reference
The C++ lex/bison is reffered(almost copied & renamed) from: [FLEX AND BISON IN C++](http://www.jonathanbeard.io/tutorials/FlexBisonC++.html)
[Print container in custom format](https://stackoverflow.com/questions/4077609/overloading-output-stream-operator-for-vectort)
[Hash function for unordered containers](https://marknelson.us/posts/2011/09/03/hash-functions-for-c-unordered-containers.html)

### TODO: Doubts
1. Confirm the precesion and associativity of "=" & "!="?

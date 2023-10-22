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
- [ ] Apply Union-Find
- [ ] Check against negation or not equal
- [ ] Add New test cases

### Reference
The C++ lex/bison is reffered(almost copied & renamed) from: [FLEX AND BISON IN C++](http://www.jonathanbeard.io/tutorials/FlexBisonC++.html)


### TODO: Doubts
1. Confirm the precesion and associativity of "=" & "!="?

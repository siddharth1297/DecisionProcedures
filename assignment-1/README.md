# Assignment-1: Implement Tseitin Transform

## Author
Siddharth Nayak
MT22128
siddharth22128@iiitd.ac.in

## Instruction
test files are in src/test
Refer to the result.txt file for satisfiability of the expression in the files.
```
cd src/
```
Build
```
$ make
```
Run
```
$ ./main < test/infile_101
```
The above produces dimacs.cnf file.
Feed the dimacs.cnf file to the minisat for checking satisfaibility.

## Constraints
1. Each file must contain only one line
2. Variables must be alphanumenric. e.x: p, p1, p11, P, T
3. No variable should start with 'a' or 'F'.
4. It doesn't support true/false value in the expression

## TODO
1. Free memory
2. Add support for true/false value
3. User specified output file

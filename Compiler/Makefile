#! /bin/bash
bison -d -o compiler.cpp  compiler.y
flex -o mylex.cpp compiler.l
#g++ -std=c++11 -o compiler compiler.cpp mylex.cpp main.cpp
g++ -std=c++11 -o compiler compiler.cpp mylex.cpp main.cpp 
./compiler > ./MIPS/intercode.txt
echo "--------------AST-code----------------"
cat ./MIPS/intercode.txt
python ./MIPS/MIPS.py >> mips2
echo ""
echo "--------------OBJ-ASM----------------"
cat ./MIPS/objectcode.asm
python ./write.py
echo ""
echo "---------------Error-Report-----------------"
cat ../result/ErrorLog.txt

# encoding: utf-8
# IR-tree
record = []
with open("./MIPS/intercode.txt") as f:
    start = False
    for line in f:
        if "IRTREE" in line:
            start = True
        if "Abstract Syntax" in line:
            start = False
        if "Function-Table" in line:
            start = False
        if start == False:
            continue
        if len(line) == 0:
            start = False
            continue
        record.append(line)
with open("../result/IRT.txt","w+") as t:
    for line in record:
      if "IR TREE" not in line:
            t.writelines(line)

del record[:]
count = 0
errornum = 0

with open("./MIPS/intercode.txt") as f:
    s = ""
    for line in f:
        count += 1
        if "error--" in line:
            errornum += 1
            s = "line " + str(count) + " : syntax error\n"
            record.append(s)

if errornum > 0:
    with open("../result/Errorlog.txt","w+") as s:
        for t in record:
            s.writelines(t)

Function_table = []

with open("./MIPS/intercode.txt") as f:
    start = False
    for line in f:
        if "Function-Table" in line:
            start = True
        if start == False:
            continue
        if len(line) == 0:
            start = False
            continue
        Function_table.append(line)
with open("../result/Function_table.txt","w+") as t:
    for line in Function_table:
      if "Function-Table" not in line:
         t.writelines(line)

del record[:]
with open("./MIPS/intercode.txt") as f:
    start = False
    for line in f:
        if "Abstract Syntax" in line:
            start = True
        if "Function-Table" in line:
            start = False
        if start == False:
            continue
        if len(line) == 0:
            start = False
            continue
        record.append(line)
with open("../result/AST.txt","w+") as t:
    for l in record:
        if "Abstract Syntax" not in l:
            t.writelines(l)

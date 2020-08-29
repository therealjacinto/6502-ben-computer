import re
import os, sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import utilities

def isMethod(line):
    if re.match("^[_a-zA-Z][^0-9]", line):
        return True
    return False

def isLabel(line):
    if re.match("^l[0-9]", line):
        return True
    return False

def isComment(line):
    if line.startswith(";"):
        return True
    return False

def isSectionHeader(line):
    if re.match("^\tsection\t", line):
        return True
    return False

def isGlobalHeader(line):
    if line.startswith("\tglobal"):
        return True
    return False

def isData(line):
    if line.startswith("\tbyte"):
        return True
    return False

def isBlank(line):
    if line == "":
        return True
    return False

def formatMethodName(line):
    return line.strip().replace(":","")

def formatLabelName(line):
    return line.strip().replace(":","")

def isOp(line):
    if re.match("^\t[a-z]{3}[\t\n ]", line):
        return True
    elif re.match("^ {2}[a-z]{3}[\t\n ]", line):
        return True
    return False

def formatOp(line):
    if line.startswith("\tbyte"):
        opCode = "byte"
        arg = line[6:].strip()
    else:
        opCode = line[1:4]
        arg = line[5:].strip()
    return (opCode, arg)

def formatOpCode(opTuple):
    return opTuple[0].lower()

def formatOpArg(opTuple):
    if re.match("^#[0-9]", opTuple[1]):
        return utilities.convertDecToHex(opTuple[1][1:])
    elif opTuple[1] == "sp+1":
        return "21"
    elif re.match("^[0-9]", opTuple[1]):
        return utilities.convertDecToHex(opTuple[1])
    elif opTuple[1].startswith("_"):
        return opTuple[1]
    elif opTuple[1].startswith("#%"):
        return utilities.convertBinToHex(opTuple[1][2:])
    elif opTuple[1].startswith("#$"):
        return opTuple[1][2:]
    return opTuple[1]

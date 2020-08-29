import re

def formatOp(line):
    opcode = line[17:20]
    arg = line[21:].strip()
    return (opcode, arg)

def formatAddr(line):
    return line[1:5]

def formatOpArg(opTuple):
    if opTuple[1].startswith("#$"):
        return opTuple[1][2:]
    elif re.match("^\$[0-9]", opTuple[1]):
        return opTuple[1][1:]
    return opTuple[1].lower()

def formatOpCode(opTuple):
    return opTuple[0].lower()
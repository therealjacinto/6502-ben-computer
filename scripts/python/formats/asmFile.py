import re
import os, sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import utilities

def isMethod(line):
    cleanline = cleanupLine(line)
    if ".asciiz" in cleanline or ".word" in cleanline:
        return False
    if re.match("^[_a-zA-Z][^0-9]*:", cleanline):
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

def containsComment(line):
    if ";" in line:
        return True
    return False

def removeComment(line):
    position = line.find(";")
    return line[0:position]

def cleanupLine(line):
    if containsComment(line):
        return removeComment(line).strip()
    else:
        return line.strip()

def isSectionHeader(line):
    if line.startswith("\tsection\t"):
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
    cleanline = cleanupLine(line)
    if cleanline == "":
        return True
    return False

def isDeclaration(line):
    if re.match("^[a-zA-Z]{1}[a-zA-Z0-9 ]*=", line):
        return True
    return False

def isOrg(line):
    if re.match("^(  |\t).org", line):
        return True
    return False

def formatOrg(line):
    cleanline = cleanupLine(line)
    return cleanline.replace(".org", "").replace("$","").strip()

def formatDeclaration(line):
    cleanline = cleanupLine(line)
    formattedList = cleanline.replace(" ", "").split("=")
    value = ""
    if formattedList[1].startswith("$"):
        value = formattedList[1][1:]
    elif formattedList[1].startswith("%"):
        value = utilities.convertBinToHex(formattedList[1][1:])
    return (formattedList[0], value)

def formatMethodName(line):
    cleanline = cleanupLine(line)
    return cleanline.replace(":","")

def formatLabelName(line):
    cleanline = cleanupLine(line)
    return cleanline.replace(":","")

def isOp(line):
    if re.match("^(  |\t)[a-z]{3}[\t\n ]", line):
        return True
    return False

def formatOp(line):
    cleanline = cleanupLine(line)
    if cleanline.startswith("byte"):
        opCode = "byte"
        arg = cleanline[5:]
    else:
        opCode = cleanline[0:3]
        arg = cleanline[4:]
    return (opCode, arg)

def formatOpCode(opTuple):
    return opTuple[0].lower()

def formatOpArg(opTuple):
    if re.match("^#[0-9]", opTuple[1]):
        return utilities.convertDecToHex(opTuple[1][1:]).replace('"', "")
    if re.match('^#"[0-9]', opTuple[1]):
        return utilities.convertAsciiToHex(opTuple[1][1:].replace('"', ""))
    elif re.match("^#[a-zA-Z]", opTuple[1]):
        return opTuple[1][1:]
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

def formatSectionName(line):
    cleanline = cleanupLine(line)
    return cleanline.replace("section","").strip('"').strip()

def formatGlobalName(line):
    cleanline = cleanupLine(line)
    return cleanline.replace("global","").strip('"').strip()

def isasciiz(line):
    cleanline = cleanupLine(line)
    if ".asciiz" in cleanline:
        return True
    return False

def isword(line):
    cleanline = cleanupLine(line)
    if ".word" in cleanline and not cleanline.startswith(".word"):
        return True
    return False

def formatAsciiz(line):
    cleanline = cleanupLine(line)
    formattedLine = cleanline.split(":")
    formattedLine[1] = formattedLine[1].replace(" .asciiz ", "").replace('"',"")
    return (formattedLine[0], formattedLine[1])

def formatWord(line):
    cleanline = cleanupLine(line)
    formattedLine = cleanline.split(":")
    formattedLine[1] = formattedLine[1].replace(" .word ", "").replace('"',"")
    return (formattedLine[0], formattedLine[1])

def isOffset(opArg):
    if "," in opArg:
        return True
    return False
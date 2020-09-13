import os
import re
import struct

import formats.asmFile as asmFile
import formats.disFile as disFile

startOfROMDec = 32768
branches = ["bbr", "bbs", "bcc", "bcs", "beq", "bmi", "bne", "bpl", "bra", "bvc", "bvs", "jmp"]
jumps = ["jmp", "jsr"]

def lineMatch(asmOpTuple, disFileTuple, locationDict, filename, unknownList=None, filepointer=None, globalList=None, declarationDict=None, methodName=None, methodDict=None):
    asmOpCode = asmFile.formatOpCode(asmOpTuple)
    asmOpArg = asmFile.formatOpArg(asmOpTuple)
    disOpCode = disFile.formatOpCode(disFileTuple)
    disOpArg = disFile.formatOpArg(disFileTuple)
    
    # Both op codes must match
    if asmOpCode == disOpCode:
        # Everything matches
        if asmOpArg == disOpArg:
            return True
        # Argument to op code is a label
        elif re.match("^l[1-9]", asmOpArg):
            # If the label location is known, it must match
            if asmOpArg in locationDict:
                if locationDict[asmOpArg] == disOpArg:
                    return True
            # Label location is not known but it could be label with filename 
            elif makeNameUniqueFromPath(asmOpArg, filename) in locationDict:
                if locationDict[makeNameUniqueFromPath(asmOpArg, filename)] == disOpArg:
                    return True
            # Label location is not known, but it is a branch so we can assume it might be
            elif asmOpCode in branches:
                return True
        # Pointer somewhere
        elif (asmOpArg.startswith("#>(") or asmOpArg.startswith("#<(")) and asmOpArg.startswith("#"):
            return True
        # Op argument is another method
        elif re.match("^[_a-zA-Z][a-zA-Z]", asmOpArg) and (asmOpCode in jumps or asmOpCode in branches):
            # if method exists in location dict (using file name of parent method. If
            # in the same file then will have same filename appended. Otherwise, the
            # next if statement will catch the globals)
            if asmOpArg in locationDict:
                if locationDict[asmOpArg] == disOpArg:
                    return True
            elif makeNameUniqueFromPath(asmOpArg, filename) in locationDict:
                if locationDict[makeNameUniqueFromPath(asmOpArg, filename)] == disOpArg:
                    return True
            # For global functions in other files. If is global and is a duplicate name
            # then we might be in trouble but then again maybe so will the linker?
            elif globalList is not None and asmOpArg in globalList:
                return True
            # If the method is itself, then just say True for now
            elif asmOpArg == methodName:
                return True
            # There is a circular reference between the two methods. Check if
            # functions reference each other so we aren't caught in a loop
            elif (asmOpArg, filename) in methodDict:
                for action in methodDict[(asmOpArg, filename)]:
                    if asmFile.formatOpArg(action) == methodName:
                        return True
        # For references to data that is global
        elif re.match("^[_a-zA-Z][a-zA-Z]", asmOpArg) and asmOpArg in globalList:
            return True
        # For references to data not on ROM
        elif re.match("^[_a-zA-Z][a-zA-Z]", asmOpArg) and unknownList is not None and (asmOpArg, filename) in unknownList:
            return True
        # Is an offset oparg
        elif re.match("^[_a-zA-Z][a-zA-Z]", asmOpArg) and asmFile.isOffset(asmOpArg):
            startpoint = asmOpArg.split(",")[0]
            if startpoint in globalList:
                return True
        # It is a declaration
        elif declarationDict is not None and (asmOpArg, filename) in declarationDict:
            if declarationDict[(asmOpArg, filename)] == disOpArg:
                return True
        # It is an expression
        elif re.match(r"^#\(.*|.*\)", asmOpArg) or "+" in asmOpArg:
            return True
       

    # Trying to catch -opt-branch optimizations by vasm6502
    elif asmOpCode == "bne" and disOpCode == "beq" and filepointer is not None:
        line = filepointer.readline()
        if disFile.formatOpCode(disFile.formatOp(line)) == "jmp":
            return True
    return False

def convertDecToHex(strInt):
    hexstring = hex(int(strInt))[2:]
    size = len(hexstring)
    if size % 2 != 0:
        size += 1
    return hexstring.zfill(size)

def convertBinToHex(strBin):
    return hex(int(strBin, 2)).lstrip("0x").zfill(2)

def infiniteLoop(methodName, address, asmOpTuple, disFileTuple):
    if asmFile.formatOpArg(asmOpTuple) == methodName:
        if address == disFile.formatOpArg(disFileTuple):
            return True
    return False

def formatFileName(filepath):
    return os.path.splitext(os.path.basename(filepath))[0]

def isGlobal(method, globalList):
    if method in globalList:
        return True
    return False

def makeNameUnique(name, filename):
    return name + "-" + filename

def makeNameUniqueFromPath(name, filename):
    return makeNameUnique(name, formatFileName(filename))

def convertWordToHex(word):
    return str(struct.pack("<H", int(word)))[2:-1].replace("\\x","")

def convertAsciiToHex(asciiStr):
    return hex(int(asciiStr) + 48)[2:]
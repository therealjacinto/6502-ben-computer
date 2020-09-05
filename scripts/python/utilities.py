import os

import formats.asmFile as asmFile
import formats.disFile as disFile

startOfROMDec = 32768
branches = ["bbr", "bbs", "bcc", "bcs", "beq", "bmi", "bne", "bpl", "bra", "bvc", "bvs", "jmp"]

def lineMatch(asmOpTuple, disFileTuple, locationDict, filename, unknownList=None, filepointer=None, globalList=None):
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
        elif asmOpArg.startswith("l"):
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
        elif asmOpArg.startswith("_"):
            # if method exists in location dict (using file name of parent method. If
            # in the same file then will have same filename appended. Otherwise, the
            # next if statement will catch the globals)
            if asmOpArg in locationDict or makeNameUniqueFromPath(asmOpArg, filename) in locationDict:
                if locationDict[asmOpArg] == disOpArg:
                    return True
            # For global functions in other files. If is global and is a duplicate name
            # then we might be in trouble but then again maybe so will the linker?
            elif globalList is not None and asmOpArg in globalList:
                return True
            # For references to data not on ROM
            elif unknownList is not None:
                if asmOpArg in unknownList or makeNameUniqueFromPath(asmOpArg, filename) in unknownList:
                    return True
    # Trying to catch -opt-branch optimizations by vasm6502
    elif asmOpCode == "bne" and disOpCode == "beq" and filepointer is not None:
        line = filepointer.readline()
        if disFile.formatOpCode(disFile.formatOp(line)) == "jmp":
            return True
    return False

def convertDecToHex(strInt):
    return hex(int(strInt)).lstrip("0x").zfill(2)

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
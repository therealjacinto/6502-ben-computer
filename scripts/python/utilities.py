import formats.asmFile as asmFile
import formats.monFile as monFile

startOfROMDec = 32768

def lineMatch(asmOpTuple, disFileTuple, locationDict):
    branches = ["bcs", "bcc", "bne", "beq"]
    if asmFile.formatOpCode(asmOpTuple) == monFile.formatOpCode(disFileTuple):
        if asmFile.formatOpArg(asmOpTuple) == monFile.formatOpArg(disFileTuple):
            return True
        elif asmFile.formatOpArg(asmOpTuple).startswith("l") and asmFile.formatOpCode(asmOpTuple) in branches:
           return True
        elif asmFile.formatOpArg(asmOpTuple).startswith("#>(") and asmFile.formatOpArg(asmOpTuple).startswith("#"):
            return True
        elif asmFile.formatOpArg(asmOpTuple).startswith("#<(") and asmFile.formatOpArg(asmOpTuple).startswith("#"):
            return True
        elif asmFile.formatOpArg(asmOpTuple).startswith("_"):
            if asmFile.formatOpArg(asmOpTuple) in locationDict:
                if locationDict[asmFile.formatOpArg(asmOpTuple)] == monFile.formatOpArg(disFileTuple):
                    return True
        #else:
        #    print(asmFile.formatOpArg(asmOpTuple), monFile.formatOpArg(disFileTuple))
        return False

def convertDecToHex(strInt):
    return hex(int(strInt)).lstrip("0x").zfill(2)

def convertBinToHex(strBin):
    return hex(int(strBin, 2)).lstrip("0x").zfill(2)

def infiniteLoop(methodName, address, asmOpTuple, disFileTuple):
    if asmFile.formatOpArg(asmOpTuple) == methodName:
        if address == monFile.formatOpArg(disFileTuple):
            return True
    return False
import os
import re
import csv
import sys

import utilities
import formats.asmFile as asmFile
import formats.disFile as disFile

# Parse file line by line and build lists of actions for each method
def getMethods(filename, methodDict, unknownList, uniqueList, duplicateDict):
    with open(filename, "r") as fp:
        line = fp.readline()
        while line:
            # Try and obtain non-Rom data sections
            if asmFile.isSectionHeader(line):
                if asmFile.formatSectionName(line) == "data":
                    line = fp.readline()
                    methodName = asmFile.formatMethodName(line)
                    unknownList.append((methodName, filename))
                    if methodName not in uniqueList:
                        uniqueList.append(methodName)
                    else:
                        if methodName in duplicateDict:
                            duplicateDict[methodName] += 1
                        else:
                            duplicateDict[methodName] = 2
                fp.readline()
            # Create method object
            elif asmFile.isMethod(line):
                methodName = asmFile.formatMethodName(line)
                methodDict[(methodName, filename)] = []
                if methodName not in uniqueList:
                    uniqueList.append(methodName)
                else:
                    if methodName in duplicateDict:
                        duplicateDict[methodName] += 1
                    else:
                        duplicateDict[methodName] = 2
                line = fp.readline()
                # Build list of actions
                while True:
                    if asmFile.isOp(line):
                        methodDict[(methodName, filename)].append(asmFile.formatOp(line))
                        line = fp.readline()
                    elif asmFile.isComment(line) or asmFile.isLabel(line) or asmFile.isBlank(line):
                        line = fp.readline()
                    else:
                        break
                continue
            line = fp.readline()

# Parse file line by line and build lists of actions for each label. Also, if
# label is a data label, build it into the data dict.
def getLabels(filename, labelDict, dataDict, uniqueList, duplicateDict):
    with open(filename, "r") as fp:
        line = fp.readline()
        while line:
            if asmFile.isLabel(line):
                labelName = asmFile.formatLabelName(line)
                line = fp.readline()
                # Build Data object
                if asmFile.isData(line):
                    dataDict[(labelName, filename)] = []
                    if labelName not in uniqueList:
                        uniqueList.append(labelName)
                    else:
                        if labelName in duplicateDict:
                            duplicateDict[labelName] += 1
                        else:
                            duplicateDict[labelName] = 2
                    while True:
                        if asmFile.isData(line):
                            dataDict[(labelName, filename)].append(asmFile.formatOpArg(asmFile.formatOp(line)))
                            line = fp.readline()
                        elif asmFile.isComment(line):
                            line = fp.readline()
                        else:
                            break
                # Build label object with actions
                else:
                    labelDict[(labelName, filename)] = []
                    if labelName not in uniqueList:
                        uniqueList.append(labelName)
                    else:
                        if labelName in duplicateDict:
                            duplicateDict[labelName] += 1
                        else:
                            duplicateDict[labelName] = 2
                    while True:
                        if asmFile.isOp(line):
                            labelDict[(labelName, filename)].append(asmFile.formatOp(line))
                            line = fp.readline()
                        elif asmFile.isComment(line):
                            line = fp.readline()
                        else:
                            break
            else:
                line = fp.readline()

# Iterate through all methods to find location in the filename
def findMethods(filename, methodDict, locationDict, duplicateDict, unknownList, globalList, declarationDict=None):
    for method, actions in methodDict.items():
        possibleLocations = []
        locationDictMethod = method[0]
        if method[0] in duplicateDict and duplicateDict[method[0]] > 1:
            locationDictMethod = utilities.makeNameUniqueFromPath(method[0], method[1])
        # Ignore if location is already known
        if locationDictMethod in locationDict:
            continue
        possibleLocations = []
        findMethod(filename, method, locationDictMethod, actions, possibleLocations, locationDict, unknownList, globalList, methodDict, declarationDict)
        if len(possibleLocations) == 1:
            locationDict[locationDictMethod] = possibleLocations[0]

# Find method location base on entire list of actions. Currently, it looks for
# the first occurance and returns since there is a small likelihood that two
# methods have the same list of actions
def findMethod(filename, method, methodName, actions, possibleLocations, locationDict, unknownList, globalList, methodDict, declarationDict=None):
    with open(filename, "r") as fp:
        line = fp.readline()
        actions_i = 0
        while line:
            if actions_i == 0:
                address = disFile.formatAddr(line)
            # Try and catch exit
            if len(actions) == 1 and utilities.infiniteLoop(method[0], address, actions[0], disFile.formatOp(line)):
                possibleLocations.append(address)
                line = fp.readline()
                continue
            while actions_i < len(actions) and len(actions) > 1 and utilities.lineMatch(actions[actions_i], disFile.formatOp(line), locationDict, method[1], unknownList, fp, globalList, declarationDict, method[0], methodDict=methodDict):
                actions_i += 1
                line = fp.readline()
            if actions_i == len(actions):
                possibleLocations.append(address)
                actions_i = 0
                line = fp.readline()
                continue
            else:
                # Reset counter
                actions_i = 0
            line = fp.readline()

# Find location of labels in filename
def findLabels(filename, labelDict, locationDict, duplicateDict, ignoreFileList):
    for label, actions in labelDict.items():
        locationDictLabel = label[0]
        possibleLocations = []
        if label[0] in duplicateDict and duplicateDict[label[0]] > 1:
            locationDictLabel = utilities.makeNameUniqueFromPath(label[0], label[1])
        # Ignore if label location already found or label file name is in ignore list or if 
        # there are no actions associated with the label
        if locationDictLabel in locationDict or label[1] in ignoreFileList or len(actions) < 1:
            continue
        possibleLocations = []
        findLabel(filename, label, locationDictLabel, actions, locationDict, possibleLocations)
        if len(possibleLocations) == 1:
            locationDict[locationDictLabel] = possibleLocations[0]

# Builds possibleLocations dict of all locations of the action
def findLabel(filename, label, locationDictLabel, actions, locationDict, possibleLocations):
    with open(filename, "r") as fp:
        line = fp.readline()
        actions_i = 0
        while line:
            if actions_i == 0:
                address = disFile.formatAddr(line)
            while actions_i < len(actions) and utilities.lineMatch(actions[actions_i], disFile.formatOp(line), locationDict, label[1]):
                actions_i += 1
                line = fp.readline()
            if actions_i == len(actions):
                possibleLocations.append(address)
                actions_i = 0
                line = fp.readline()
                continue
            else:
                actions_i = 0
            line = fp.readline()


def findLabelsFromMethods(filename, labelDict, methodDict, locationDict, duplicateDict, ignoreFileList):
    for label in labelDict:
        locationDictLabel = label[0]
        if label[0] in duplicateDict and duplicateDict[label[0]] > 1:
            locationDictLabel = utilities.makeNameUniqueFromPath(label[0], label[1])

        if locationDictLabel in locationDict or label[1] in ignoreFileList:
            continue
        
        for method, actions in methodDict.items():
            # Ignore if not from the same file because there are no global labels
            if label[1] != method[1] or method[1] in ignoreFileList:
                continue
            locationDictMethod = method[0]
            if method[0] not in locationDict:
                locationDictMethod = utilities.makeNameUniqueFromPath(method[0], method[1])
            for i, action in enumerate(actions):
                if label[0] == asmFile.formatOpArg(action):
                    methodAddr = locationDict[locationDictMethod]
                    with open(filename) as fp:
                        line = fp.readline()
                        while line:
                            if line.startswith("$" + methodAddr):
                                for _ in range(0, i):
                                    line = fp.readline()
                                # Its possible depending on how many branch optimizations have occured by 
                                # the vasm assembler that this isn't the correct spot. From here on out,
                                # we will read until we reach.
                                while (action[0] != disFile.formatOpCode(disFile.formatOp(line))):
                                    line = fp.readline()
                                locationDict[locationDictLabel] = disFile.formatOpArg(disFile.formatOp(line))
                                break
                            line = fp.readline()
                if label[0] in locationDict:
                    break
            if label[0] in locationDict:
                break

def findData(filename, locationDictLabel, dataList, locationDict, duplicateDict):
    byteString = b''
    for hexByte in dataList:
        byteString += bytes.fromhex(hexByte)
    with open(filename, 'rb') as fp:
        s = fp.read()
    locationDict[locationDictLabel] = utilities.convertDecToHex(s.find(byteString) + utilities.startOfROMDec)

def findDatas(filename, dataDict, locationDict, duplicateDict, ignoreFileList):
    for label, data in dataDict.items():
        locationDictLabel = label[0] 
        if label[0] in duplicateDict and duplicateDict[label[0]] > 1:
            locationDictLabel = utilities.makeNameUniqueFromPath(label[0], label[1])
        if locationDictLabel in locationDict or label[1] in ignoreFileList:
            continue
        findData(filename, locationDictLabel, data, locationDict, duplicateDict)

def findIgnoreFiles(methodDict, locationDict, ignoreFileList, duplicateDict):
    for method in methodDict:
        if method[0] not in locationDict and utilities.makeNameUniqueFromPath(method[0], method[1]) not in locationDict:
            ignoreFileList.append(method[1])
            if method[0] in duplicateDict:
                duplicateDict[method[0]] -= 1

def getGlobals(filename, globalList, unknownList, dataDict):
    with open(filename, "r") as fp:
        line = fp.readline()
        while line:
            if asmFile.isGlobalHeader(line):
                globalName = asmFile.formatGlobalName(line)
                line = fp.readline()
                if not asmFile.isMethod(line) and (globalName, filename) not in unknownList:
                    globalList.append(globalName)
                else:
                    line = fp.readline()
            elif asmFile.isasciiz(line):
                (globalName, data) = asmFile.formatAsciiz(line)
                dataList = []
                for c in data:
                    dataList.append("{0:02x}".format(ord(c)))
                dataDict[(globalName, filename)] = dataList
                globalList.append(globalName)
                line = fp.readline()
            elif asmFile.isword(line):
                (globalName, data) = asmFile.formatWord(line)
                dataList = []
                formattedData = utilities.convertWordToHex(data)
                formattedList = [formattedData[i:i+2] for i in range(0, len(formattedData), 2)]
                dataDict[(globalName, filename)] = formattedList
                globalList.append(globalName)
                line = fp.readline()
            else:
                line = fp.readline()

def purgeMethods(methodDict, locationDict, duplicateDict, ignoreFileList):
    for method in methodDict:
        if method[1] in ignoreFileList:
            if method[0] in locationDict:
                del locationDict[method[0]]
                if method[0] in duplicateDict:
                    duplicateDict[method[0]] -= 1
            elif utilities.makeNameUniqueFromPath(method[0], method[1]) in locationDict:
                del locationDict[utilities.makeNameUniqueFromPath(method[0], method[1])]
                if method[0] in duplicateDict:
                    duplicateDict[method[0]] -= 1
    for method in methodDict:
        if utilities.makeNameUniqueFromPath(method[0], method[1]) in locationDict and duplicateDict[method[0]] < 2:
            # Remove from location dict and add new name
            locationDict[method[0]] = locationDict[utilities.makeNameUniqueFromPath(method[0], method[1])]
            del locationDict[utilities.makeNameUniqueFromPath(method[0], method[1])]

def purgeLabels(labelDict, duplicateDict, ignoreFileList):
    for label in labelDict:
        if label[1] in ignoreFileList and label[0] in duplicateCount:
            duplicateDict[label[0]] -= 1

def getDeclarations(filename, declarationDict):
    with open(filename, "r") as fp:
        line = fp.readline()
        while line:
            if asmFile.isDeclaration(line):
                declaration = asmFile.formatDeclaration(line)
                declarationDict[(declaration[0], filename)] = declaration[1]
            line = fp.readline()

def saveDeclarations(declarationDict, ignoreFileList, locationDict):
    for declaration in declarationDict:
        if declaration[1] not in ignoreFileList:
            locationDict[declaration[0]] = declarationDict[declaration]

if __name__ == "__main__":
    usage = sys.argv[0] + ":\nAssembly file parser. This script attempts to "
    usage += "find the address locations of methods and labels in an assembly "
    usage += "file in a bin file. This script requires 4 arguments:\n"
    usage += "\t" + sys.argv[0] + " [location of bin file] [directory of "
    usage += "assembly files] [location of disassembly file] [location to "
    usage += "save output]"

    if len(sys.argv) != 5:
        print(usage)
        exit(1)

    binFile = sys.argv[1]
    asmDirectory = sys.argv[2]
    outFile = sys.argv[3]
    locationsFile = sys.argv[4]

    # Dictionaries to store actions and data
    methods = {}
    labels = {}
    data = {}

    # Lists to keep track of existing names
    uniqueNames = []
    duplicateCount = {}
    globalNames = []

    # Non-ROM locations
    nonRomLocations = []
    
    # List of files to ignore because they might not be relevant to binary
    ignorefile = []

    # Final dict of locations 
    locations = {}

    # Find declarations
    declarations = {}

    # List of files in build directory
    asmFiles = []
    for file in os.listdir(asmDirectory):
        if file.endswith(".s") and file != "startup-reg.s":
            asmFiles.append(os.path.join(asmDirectory, file))

    for file in asmFiles:
        getDeclarations(file, declarations)
        getMethods(file, methods, nonRomLocations, uniqueNames, duplicateCount)
        getLabels(file, labels, data, uniqueNames, duplicateCount)
        getGlobals(file, globalNames, nonRomLocations, data)

    lastSize = -1
    while len(locations) != lastSize:
        lastSize = len(locations)
        findMethods(outFile, methods, locations, duplicateCount, nonRomLocations, globalNames, declarations)
    findIgnoreFiles(methods, locations, ignorefile, duplicateCount)

    purgeMethods(methods, locations, duplicateCount, ignorefile)
    saveDeclarations(declarations, ignorefile, locations)
    combinedLabels = {**labels, **data}
    purgeLabels(combinedLabels, duplicateCount, ignorefile)
    
    findDatas(binFile, data, locations, duplicateCount, ignorefile)
    findLabelsFromMethods(outFile, labels, methods, locations, duplicateCount, ignorefile)
    lastSize = -1
    while len(locations) != lastSize:
        findLabels(outFile, labels, locations, duplicateCount, ignorefile)
        lastSize = len(locations)
    findLabels(outFile, labels, locations, duplicateCount, ignorefile)

    w = csv.writer(open(locationsFile, "w"))
    for key, val in locations.items():
        w.writerow([key, val])

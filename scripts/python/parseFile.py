import os
import re
import csv
import sys

import utilities
import formats.asmFile as asmFile
import formats.monFile as monFile

def getMethods(filename, methodDict):
    dataIgnore = ["data"]
    with open(filename, "r") as fp:
        line = fp.readline()
        while line:
            if asmFile.isSectionHeader(line) and asmFile.formatSectionName(line) in dataIgnore:
                fp.readline()
            elif asmFile.isMethod(line):
                methodName = asmFile.formatMethodName(line)
                methodDict[methodName] = []
                line = fp.readline()
                while True:
                    if asmFile.isOp(line):
                        methodDict[methodName].append(asmFile.formatOp(line))
                        line = fp.readline()
                    elif asmFile.isComment(line) or asmFile.isLabel(line) or asmFile.isBlank(line):
                        line = fp.readline()
                    else:
                        break
            line = fp.readline()

def findMethod(filename, method, actions, locationDict):
    with open(filename, "r") as fp:
        line = fp.readline()
        actions_i = 0
        while line:
            if actions_i == 0:
                address = monFile.formatAddr(line)
            # Try and catch exit
            if len(actions) == 1 and utilities.infiniteLoop(method, address, actions[actions_i], monFile.formatOp(line)):
                locationDict[method] = address
                break
            while actions_i < len(actions) and utilities.lineMatch(actions[actions_i], monFile.formatOp(line), locationDict):
                actions_i += 1
                line = fp.readline()
            if actions_i == len(actions):
                locationDict[method] = address
                break
            else:
                actions_i = 0
            line = fp.readline()

def findMethods(filename, methodDict, locationDict):
    for method, actions in methodDict.items():
        if method in locationDict:
            continue
        findMethod(filename, method, actions, locationDict)

def findLabel(filename, label, actions, locationDict, possibleLocations):
    with open(filename, "r") as fp:
        line = fp.readline()
        actions_i = 0
        while line:
            if actions_i == 0:
                address = monFile.formatAddr(line)
            while actions_i < len(actions) and utilities.lineMatch(actions[actions_i], monFile.formatOp(line), locationDict):
                actions_i += 1
                line = fp.readline()
            if actions_i == len(actions):
                #locationDict[label] = address
                possibleLocations[label].append(address)
                actions_i = 0
                continue
            else:
                actions_i = 0
            line = fp.readline()

def findLabels(filename, labelDict, locationDict):
    possibleLocations = {}
    for label, actions in labelDict.items():
        if label in locationDict:
            continue
        if len(actions) < 3:
            continue
        if label not in possibleLocations:
            possibleLocations[label] = []
        findLabel(filename, label, actions, locationDict, possibleLocations)
        if len(possibleLocations[label]) == 1:
            locationDict[label] = possibleLocations[label][0]

def getLabels(filename, labelDict, dataDict):
    with open(filename, "r") as fp:
        line = fp.readline()
        while line:
            if asmFile.isLabel(line):
                labelName = asmFile.formatLabelName(line)
                line = fp.readline()
                if asmFile.isData(line):
                    dataDict[labelName] = []
                    while True:
                        if asmFile.isData(line):
                            dataDict[labelName].append(asmFile.formatOpArg(asmFile.formatOp(line)))
                            line = fp.readline()
                        elif asmFile.isComment(line):
                            line = fp.readline()
                        else:
                            break
                else:
                    labelDict[labelName] = []
                    while True:
                        if asmFile.isOp(line):
                            labelDict[labelName].append(asmFile.formatOp(line))
                            line = fp.readline()
                        elif asmFile.isComment(line):
                            line = fp.readline()
                        else:
                            break
            else:
                line = fp.readline()

def findLabelsFromMethods(filename, labelDict, methodDict, locationDict):
    for label in labelDict:
        if label in locationDict:
            continue
        for method, actions in methodDict.items():
            if method not in locationDict:
                continue
            for i, action in enumerate(actions):
                if label == asmFile.formatOpArg(action):
                    methodAddr = locationDict[method]
                    with open(filename) as fp:
                        line = fp.readline()
                        while line:
                            if line.startswith("$" + methodAddr):
                                for _ in range(0, i):
                                    line = fp.readline()
                                locationDict[label] = monFile.formatOpArg(monFile.formatOp(line))
                                break
                            line = fp.readline()
                if label in locationDict:
                    break
            if label in locationDict:
                break

def findData(filename, label, dataDict, locationDict):
    byteString = b''
    for hexByte in dataDict:
        byteString += bytes.fromhex(hexByte)

    with open(filename, 'rb') as fp:
        s = fp.read()
    locationDict[label] = utilities.convertDecToHex(s.find(byteString) + utilities.startOfROMDec)

def findDatas(filename, dataDict, locationDict):
    for label, data in dataDict.items():
        if label in locationDict:
            continue
        findData(filename, label, data, locationDict)

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("You need 2 arguments!")
        exit(1)
    binFile = sys.argv[1]
    outFile = sys.argv[2]
    locationsFile = sys.argv[3]

    asmFiles = []
    methods = {}
    labels = {}
    data = {}
    locations = {}

    for file in os.listdir("build"):
        if file.endswith(".s") and file != "startup-reg.s":
            asmFiles.append(os.path.join("build", file))

    for file in asmFiles:
        getMethods(file, methods)
        getLabels(file, labels, data)

    findDatas(binFile, data, locations)
    findMethods(outFile, methods, locations)
    findMethods(outFile, methods, locations)
    findLabelsFromMethods(outFile, labels, methods, locations)
    findLabels(outFile, labels, locations)

    w = csv.writer(open(locationsFile, "w"))
    for key, val in locations.items():
        w.writerow([key, val])

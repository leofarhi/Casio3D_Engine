import shutil
#fscanf
import re
import os
import sys
import struct
import unicodedata

class Vector3:
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z
    
    @staticmethod
    def fromString(string):
        string = string.replace("(", "")
        string = string.replace(")", "")
        string = string.replace(" ", "")
        string = string.split(",")
        return Vector3(float(string[0]), float(string[1]), float(string[2]))
    
    def __str__(self):
        return "(" + str(self.x) + ", " + str(self.y) + ", " + str(self.z) + ")"


class StackPlaceBlock:
    def __init__(self, position, size, id, variation):
        self.blockPos = position
        self.SizeStack = size
        self.ID = id
        self.variation = variation

def ReadWorld(path):
    # Open file
    file = open(path, "r")
    # Read file
    #read %d => size
    size = int(file.readline().replace("\n", ""))
    Lst = []
    for i in range(size):
        #fscanf(file,"%d %d %d %d %d %d %d\n",&ID,&x,&y,&z,&sx,&sy,&sz);
        ID, variation, x, y, z, sx, sy, sz = map(int, file.readline().replace("\n", "").split(" "))
        Lst.append(StackPlaceBlock(Vector3(x, y, z), Vector3(sx, sy, sz), ID, variation))   

    # Close file
    file.close()
    return Lst

def ConvertString(string):
    string = unicodedata.normalize('NFD', string).encode('ascii', 'ignore').decode("utf-8")
    CanUse="azertyuiopqsdfghjklmwxcvbnAZERTYUIOPQSDFGHJKLMWXCVBN1234567890 _+*/?."
    newStr = ""
    for i in string:
        if i in CanUse:
            newStr +=i
    return newStr

def SaveString(fp,string):
    string = ConvertString(string)
    fp.write(struct.pack('>i', len(string)))
    fp.write(b''.join(struct.pack('>B', n) for n in str.encode(string)))

def ConvertWorlds(pathFrom,pathTo):
    filesDatas = []
    for file in os.listdir(pathFrom):
        datas = ReadWorld(pathFrom + "/" + file)
        filesDatas.append((file, datas))
    with open(pathTo, 'wb') as f:
        f.write(struct.pack('>i', len(filesDatas)))
        for (file,datas) in filesDatas:
            #write file name
            file = "worlds/"+file
            print(file)
            SaveString(f,file)
            f.write(struct.pack('>i', len(datas)))
            for data in datas:
                f.write(struct.pack('>i', data.ID))
                f.write(struct.pack('>i', data.variation))
                f.write(struct.pack('>i', data.blockPos.x))
                f.write(struct.pack('>i', data.blockPos.y))
                f.write(struct.pack('>i', data.blockPos.z))
                f.write(struct.pack('>i', data.SizeStack.x))
                f.write(struct.pack('>i', data.SizeStack.y))
                f.write(struct.pack('>i', data.SizeStack.z))
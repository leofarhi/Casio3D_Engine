import ConvertMake.RPG.RPG_MakerTilesets as Tilesets
import json
import os,sys
import PIL.Image as Image
from PIL import ImageChops
import numpy as np

import struct
import unicodedata

def are_images_equal(img1, img2):
    equal_size = img1.height == img2.height and img1.width == img2.width

    if img1.mode == img2.mode == "RGBA":
        img1_alphas = [pixel[3] for pixel in img1.getdata()]
        img2_alphas = [pixel[3] for pixel in img2.getdata()]
        equal_alphas = img1_alphas == img2_alphas
    else:
        equal_alphas = True

    equal_content = not ImageChops.difference(
        img1.convert("RGB"), img2.convert("RGB")
    ).getbbox()

    return equal_size and equal_alphas and equal_content

class Map:
    def __init__(self) -> None:
        self.width = 0
        self.height = 0
        self.couches = []
        self.mapName = ""

        self.NewIDS = {}
        self.NewTilesheet = None
        self.NewCouches = []

    def load(self,path):
        self.mapName = os.path.basename(path).split(".")[0]
        with open(path,"r") as file:
            data = json.load(file)
            self.width = data["width"]
            self.height = data["height"]
            couches = data["data"]
        #split couches into layers with width*height size
        couches = [couches[i:i+self.width*self.height] for i in range(0,len(couches),self.width*self.height)]
        #keeps only the 5 first layers
        couches = couches[:4]
        #transform to 3 layers
        self.couches = [[0 for i in range(self.width*self.height)] for i in range(3)]
        for indexCouche,couche in enumerate(couches):
            for index,ID in enumerate(couche):
                if ID != 0:
                    self.couches[0][index] = self.couches[1][index]
                    self.couches[1][index] = self.couches[2][index]
                    self.couches[2][index] = ID
    
    def Generate(self):
        Tilesets.LoadImages(self.mapName)
        allIDS = set()
        for couche in self.couches:
            for ID in couche:
                allIDS.add(ID)
        allIDS = list(allIDS)
        if 0 in allIDS:
            allIDS.remove(0)
        IMGS = []
        self.NewIDS = {0:0}
        count = 1
        for index,ID in enumerate(allIDS):
            imgList = Tilesets.All[ID]
            #Convert [[[R,G,B,A],...],...] to PIL.Image
            img = Image.fromarray(np.array(imgList,dtype=np.uint8))
            #check if img is already in IMGS (same data)
            for i,img2 in enumerate(IMGS):
                if are_images_equal(img,img2):
                    self.NewIDS[ID] = i+1
                    break
            else:
                IMGS.append(img)
                self.NewIDS[ID] = count
                count+=1
        if len(IMGS) >= 256:
            raise Exception("Too many images")
        #Create a new image with all tiles
        width,height = IMGS[0].size
        newImg = Image.new("RGBA",(width*len(IMGS),height))
        for i,img in enumerate(IMGS):
            newImg.paste(img,(i*width,0))
        self.NewTilesheet = newImg
        #Create new couches with new IDS
        for couche in self.couches:
            newCouche = []
            for ID in couche:
                newCouche.append(self.NewIDS[ID])
            self.NewCouches.append(newCouche)

    def ConvertToWindow(self,path):
        with open(path,"w") as file:
            file.write(str(self.width)+"\n")
            file.write(str(self.height)+"\n")
            for couche in self.NewCouches:
                for ID in couche:
                    file.write(str(ID)+" ")
                file.write("\n")

    def ConvertToCasio(self,path):
        with open(path,"wb") as file:
            file.write(struct.pack('>I', self.width))
            file.write(struct.pack('>I', self.height))
            for couche in self.NewCouches:
                for ID in couche:
                    file.write(struct.pack('>B', ID))

    def SaveTilesheet(self,path):
        self.NewTilesheet.save(path)
        
    
def ConvertMapRPG(path):
    map = Map()
    map.load(path)
    map.Generate()
    return map
    
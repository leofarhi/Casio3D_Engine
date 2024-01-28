import os,sys
import PIL.Image as Image
import numpy as np

path = os.path.dirname(os.path.realpath(__file__))
#find "assets" folder in os.listdir(path) else path = os.path.dirname(path), stop while path != "/"
while "assets" not in os.listdir(path):
    path = os.path.dirname(path)
    if path == "/":
        raise FileNotFoundError("assets folder not found")
path = path+"/assets/mapMenu/Tilesets/"

taille=8


def subsurface(img,x,y,w,h):
    return [i[x:x+w] for i in(img[y:y+h])]

def FloorType(part):
    Parts=[]
    Sprites=[]
    for x in range(0,taille*2,taille):
        for y in range(0,taille*3,taille):
            Parts.append(subsurface(part,x,y,taille,taille))
    IDs = [ (5, 4, 2, 1),(3, 4, 2, 1),(5, 4, 3, 1),(3, 4, 3, 1),(5, 4, 2, 3),(3, 4, 2, 3),(5, 4, 3, 3),(3, 4, 3, 3),(5, 3, 2, 1),(3, 3, 2, 1),(5, 3, 3, 1),(3, 3, 3, 1),(5, 3, 2, 3),(3, 3, 2, 3),(5, 3, 3, 3),(3, 3, 3, 3),(2, 1, 2, 1),(2, 1, 3, 1),(2, 1, 2, 3),(2, 1, 3, 3),(4, 4, 1, 1),(4, 4, 1, 3),(4, 3, 1, 1),(4, 3, 1, 3),(5, 4, 5, 4),(5, 3, 5, 4),(3, 4, 5, 4),(3, 3, 5, 4),(5, 5, 2, 2),(3, 5, 2, 2),(5, 5, 3, 2),(3, 5, 3, 2),(2, 1, 5, 4),(4, 5, 1, 2),(0, 1, 1, 1),(0, 1, 1, 3),(4, 4, 4, 4),(4, 3, 0, 4),(5, 5, 5, 0),(3, 5, 5, 0),(2, 2, 2, 2),(2, 2, 3, 2),(1, 1, 0, 4),(1, 2, 1, 2),(2, 2, 5, 0),(4, 5, 0, 5),(0, 0, 0, 0),(0, 0, 0, 0)]
    
    for Index,part in enumerate(IDs):
        i1,i2,i3,i4 = Parts[part[0]],Parts[part[1]],Parts[part[2]],Parts[part[3]]
        HG = [i[:taille//2] for i in(i1[:taille//2])]
        HD = [i[taille//2:] for i in(i3[:taille//2])]
        BG = [i[:taille//2] for i in(i2[taille//2:])]
        BD = [i[taille//2:] for i in(i4[taille//2:])]
        H= [HG[i]+HD[i] for i in range(len(HG))]
        B= [BG[i]+BD[i] for i in range(len(BG))]
        IMG = H+B
        Sprites.append(IMG)
    return Sprites

def WaterfallType(part):
    IDs=[(5, 4, 2, 1),(2, 1, 2, 1),(3, 3, 3, 3),(2, 1, 3, 3)]
    
    Parts=[]
    Sprites=[]
    for x in range(0,taille*2,taille):
        for y in range(0,taille*3,taille):
            Parts.append(subsurface(part,x,y,taille,taille))
    for Index,part in enumerate(IDs):
        i1,i2,i3,i4 = Parts[part[0]],Parts[part[1]],Parts[part[2]],Parts[part[3]]
        HG = [i[:taille//2] for i in(i1[:taille//2])]
        HD = [i[taille//2:] for i in(i3[:taille//2])]
        BG = [i[:taille//2] for i in(i2[taille//2:])]
        BD = [i[taille//2:] for i in(i4[taille//2:])]
        H= [HG[i]+HD[i] for i in range(len(HG))]
        B= [BG[i]+BD[i] for i in range(len(BG))]
        IMG = H+B
        Sprites.append(IMG)
    return Sprites

def WallType(part):#2*2
    IDs=[(3, 2, 1, 0), (1, 0, 1, 0), (2, 2, 0, 0), (0, 0, 0, 0), (3, 2, 3, 2), (1, 0, 3, 2), (2, 2, 2, 2), (0, 0, 2, 2), (3, 3, 1, 1), (1, 1, 1, 1), (2, 3, 0, 1), (0, 1, 0, 1), (3, 3, 3, 3), (1, 1, 3, 3), (2, 3, 2, 3), (0, 1, 2, 3)]

    Parts=[]
    Sprites=[]
    for x in range(0,taille*2,taille):
        for y in range(0,taille*2,taille):
            Parts.append(subsurface(part,x,y,taille,taille))
    for Index,part in enumerate(IDs):
        i1,i2,i3,i4 = Parts[part[0]],Parts[part[1]],Parts[part[2]],Parts[part[3]]
        HG = [i[:taille//2] for i in(i1[:taille//2])]
        HD = [i[taille//2:] for i in(i3[:taille//2])]
        BG = [i[:taille//2] for i in(i2[taille//2:])]
        BD = [i[taille//2:] for i in(i4[taille//2:])]
        H= [HG[i]+HD[i] for i in range(len(HG))]
        B= [BG[i]+BD[i] for i in range(len(BG))]
        IMG = H+B
        Sprites.append(IMG)
    return Sprites


def subsurface(img,x,y,w,h):
    return [i[x:x+w] for i in(img[y:y+h])]
    

All = []

#image format : [[[255, 255, 255, 0],...],...] (RGBA)
class Images:
     pass
globalPath = path
def LoadImages(nameMap):
    global All,globalPath,Images
    All = []
    path= globalPath+nameMap+"/"
    for imgpath in os.listdir(path):
        if imgpath.endswith(".png"):
            setattr(Images,"Tilesheet_"+imgpath.split(".")[0].split("_")[1],np.array(Image.open(path+imgpath).convert("RGBA")).tolist())
        

    for y in range(0,16*taille,taille):
        for x in range(0,8*taille,taille):
            All.append(subsurface(Images.Tilesheet_B,x,y,taille,taille))

    for y in range(0,16*taille,taille):
        for x in range(8*taille,8*taille*2,taille):
            All.append(subsurface(Images.Tilesheet_B,x,y,taille,taille))

    for y in range(0,16*taille,taille):
        for x in range(0,8*taille,taille):
            All.append(subsurface(Images.Tilesheet_C,x,y,taille,taille))

    for y in range(0,16*taille,taille):
        for x in range(8*taille,8*taille*2,taille):
            All.append(subsurface(Images.Tilesheet_C,x,y,taille,taille))

    while len(All)<1536:
        All.append(None)


    for y in range(0,16*taille,taille):
        for x in range(0,8*taille,taille):
            All.append(subsurface(Images.Tilesheet_A5,x,y,taille,taille))

    while len(All)<2048:
        All.append(None)

    All+=FloorType(subsurface(Images.Tilesheet_A1,0,0,taille*2,taille*3))
    All+=FloorType(subsurface(Images.Tilesheet_A1,0,taille*3,taille*2,taille*3))

    All+=FloorType(subsurface(Images.Tilesheet_A1,taille*6,0,taille*2,taille*3))
    All+=FloorType(subsurface(Images.Tilesheet_A1,taille*6,taille*3,taille*2,taille*3))

    All+=FloorType(subsurface(Images.Tilesheet_A1,taille*8,0,taille*2,taille*3))

    All+=WaterfallType(subsurface(Images.Tilesheet_A1,taille*14,0,taille*2,taille*3))
    All+=[None]*(47-3)

    All+=FloorType(subsurface(Images.Tilesheet_A1,taille*8,taille*3,taille*2,taille*3))

    All+=WaterfallType(subsurface(Images.Tilesheet_A1,taille*14,taille*3,taille*2,taille*3))
    All+=[None]*(47-3)



    All+=FloorType(subsurface(Images.Tilesheet_A1,0,taille*6,taille*2,taille*3))

    All+=WaterfallType(subsurface(Images.Tilesheet_A1,taille*6,taille*6,taille*2,taille*3))
    All+=[None]*(47-3)

    All+=FloorType(subsurface(Images.Tilesheet_A1,0,taille*9,taille*2,taille*3))

    All+=WaterfallType(subsurface(Images.Tilesheet_A1,taille*6,taille*9,taille*2,taille*3))
    All+=[None]*(47-3)

    All+=FloorType(subsurface(Images.Tilesheet_A1,taille*8,taille*6,taille*2,taille*3))
    All+=FloorType(subsurface(Images.Tilesheet_A1,taille*8,taille*9,taille*2,taille*3))

    All+=WaterfallType(subsurface(Images.Tilesheet_A1,taille*14,taille*6,taille*2,taille*3))
    All+=[None]*(47-3)
    All+=WaterfallType(subsurface(Images.Tilesheet_A1,taille*14,taille*9,taille*2,taille*3))
    All+=[None]*(47-3)


    for y in range(0,12*taille,taille*3):
        for x in range(0,16*taille,taille*2):
            All+=FloorType(subsurface(Images.Tilesheet_A2,x,y,taille*2,taille*3)) 

    for y in range(0,8*taille,taille*2):
        for x in range(0,16*taille,taille*2):
            All+=WallType(subsurface(Images.Tilesheet_A3,x,y,taille*2,taille*2))
            All+=[None]*(47-15)
            


    for x in range(0,16*taille,taille*2):
            All+=FloorType(subsurface(Images.Tilesheet_A4,x,taille*5*0,taille*2,taille*3)) 
    for x in range(0,16*taille,taille*2):
            All+=WallType(subsurface(Images.Tilesheet_A4,x,taille*3+(taille*5*0),taille*2,taille*2))
            All+=[None]*(47-15)
        
    for x in range(0,16*taille,taille*2):
            All+=FloorType(subsurface(Images.Tilesheet_A4,x,taille*5*1,taille*2,taille*3)) 
    for x in range(0,16*taille,taille*2):
            All+=WallType(subsurface(Images.Tilesheet_A4,x,taille*3+(taille*5*1),taille*2,taille*2))
            All+=[None]*(47-15)
        
    for x in range(0,16*taille,taille*2):
            All+=FloorType(subsurface(Images.Tilesheet_A4,x,taille*5*2,taille*2,taille*3)) 
    for x in range(0,16*taille,taille*2):
            All+=WallType(subsurface(Images.Tilesheet_A4,x,taille*3+(taille*5*2),taille*2,taille*2))
            All+=[None]*(47-15)



    for i in range(len(All)):
        if All[i]==None:
            All[i] = All[0]

import Tuiles
import RPG_MakerTilesets as RPG
import Map
from casioplot import *
import casioplot 

couches = Map.couches
def DrawImg(x,y,Img):
    for y2 in range(len(Img)):
        for x2 in range(len(Img[y2])):
            temp=(Img[y2])[x2]
            if temp[-1]>255//2:
                temp=(temp[0],temp[1],temp[2])
                set_pixel(int(x+x2),int(y+y2),temp)

class Camera:
    def __init__(self):
        self.x=0
        self.y=0


cam = Camera()
while True:
    clear_screen()
    for i in range(4):
        couche = couches[i]
        for y,line in enumerate(couche):
            for x,img in enumerate(line):
                if img!=0:
                    DrawImg(x*8-cam.x,y*8-cam.y,RPG.All[img])
    show_screen()
    
    if casioplot.GetKey(casioplot.pygame.K_UP):
        cam.y-=48//2
    if casioplot.GetKey(casioplot.pygame.K_DOWN):
        cam.y+=48//2
    if casioplot.GetKey(casioplot.pygame.K_LEFT):
        cam.x-=48//2
    if casioplot.GetKey(casioplot.pygame.K_RIGHT):
        cam.x+=48//2
    

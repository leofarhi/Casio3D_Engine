from PIL import Image

#split image to sprites 40x40

image = Image.open("Items.png")
width, height = image.size

for i in range(0, width, 40):
        box = (i, 0, i+40, 40)
        region = image.crop(box)
        region.save("it_"+str(i//40)+".png")
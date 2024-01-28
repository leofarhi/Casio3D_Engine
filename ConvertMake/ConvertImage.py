import sys
import os
import struct
from ConvertMake import fxconv
from PIL import Image


def convert_image_cg(input,profile="rgb565a"):
	params = {"type": "bopti-image","profile": profile}
	if isinstance(input, fxconv.Image.Image):
		img = input.copy()
	else:
		img = fxconv.Image.open(input)
	if img.width >= 65536 or img.height >= 65536:
		raise fxconv.FxconvError(f"'{input}' is too large (max. 65535x65535)")

	# Crop image to key "area"
	area = fxconv.Area(params.get("area", {}), img)
	img = img.crop(area.tuple())

	img = img.convert("RGBA")

	#---
	# Format selection
	#---

	format_name = params.get("profile", "")
	has_alpha = fxconv.image_has_alpha(img)

	# If no format is specified, select rgb565 or rgb565a
	if format_name == "":
		format_name = "rgb565a" if has_alpha else "rgb565"
	# Similarly, if "p8" or "p4" is specified, select the cheapest variation
	elif format_name == "p8":
		format_name = "p8_rgb565a" if has_alpha else "p8_rgb565"
	elif format_name == "p4":
		format_name = "p4_rgb565a" if has_alpha else "p4_rgb565"

	# Otherwise, just use the format as specified
	format = fxconv.CgProfile.find(format_name)
	if format is None:
		raise fxconv.FxconvError(f"unknown image format '{format_name}")

	# Check that we have transparency support if we need it
	if has_alpha and not format.has_alpha:
		raise fxconv.FxconvError(f"'{input}' has transparency, which {format_name} "+
			"doesn't support")

	#---
	# Structure generation
	#---
	return *fxconv.image_encode(img, format), format, img



def ConvertImage(pathImage, pathOutput, profile="rgb565a"):
	data, stride, palette, color_count, format, img = convert_image_cg(pathImage, profile)
	with open(pathOutput, 'wb') as f:
		f.write(struct.pack('>B', format.id))
		f.write(struct.pack('>h', color_count))
		f.write(struct.pack('>H', img.width))
		f.write(struct.pack('>H', img.height))
		f.write(struct.pack('>i', stride))
		data = fxconv.ref(data, padding=4) #type is collections.namedtuple("bytes", "data padding")
		f.write(struct.pack('>Q',len(data.target)))
		f.write(data.target)
		if palette == None:
			f.write(struct.pack('>I',0))
		else:
			palette = fxconv.ref(palette)
			f.write(struct.pack('>I',len(palette.target)))
			f.write(palette.target)


def ConvertImageItems(pathImage, pathOutput,scale=40):
	profile="p4_rgb565a"
	image = Image.open(pathImage)
	width, _ = image.size
	count = width//scale
	with open(pathOutput, 'wb') as f:
		for i in range(0, width, scale):
			box = (i, 0, i+40, 40)
			region = image.crop(box)
			region.save("temp.png")
			data, stride, palette, color_count, format, img = convert_image_cg("temp.png", profile)
			f.write(struct.pack('>B', format.id))
			f.write(struct.pack('>h', color_count))
			f.write(struct.pack('>H', img.width))
			f.write(struct.pack('>H', img.height))
			f.write(struct.pack('>i', stride))
			data = fxconv.ref(data, padding=4) #type is collections.namedtuple("bytes", "data padding")
			f.write(struct.pack('>Q',len(data.target)))
			f.write(data.target)
			if palette == None:
				f.write(struct.pack('>I',0))
			else:
				palette = fxconv.ref(palette)
				f.write(struct.pack('>I',len(palette.target)))
				f.write(palette.target)
	os.remove("temp.png")
#!/bin/python3

import sys
import random
from math import *

# rotate edge
def rotEdge(edge, xUC, yUC):
	x1 = edge[0]; y1 = edge[1]
	x2 = edge[2]; y2 = edge[3]
	edge[0] = y1 * -yUC + x1 * xUC
	edge[1] = x1 * yUC + y1 * xUC
	edge[2] = y2 * -yUC + x2 * xUC
	edge[3] = x2 * yUC + y2 * xUC

# adjust line length to a different width
def adjustLengthToWidth(xA, yA, xB, yB, width): # input width coordinate (x)
	return yA - (xA - width) / (xA - xB) * (yA - yB) # return other coordinate (y)

# adjust line length to a different height
def adjustLengthToHeight(xA, yA, xB, yB, height): # input height coordinate (y)
	return xA - (yA - height) / (yA - yB) * (xA - xB) # return other coordinate (x)

usageMsg = """
usage: svgGridGen.py [output image]
usage: svgGridGen.py [output image] [width]
usage: svgGridGen.py [output image] [width] [height]
usage: svgGridGen.py [output image] [width] [height] [angles]
usage: svgGridGen.py [output image] [width] [height] [angles] [lines]
usage: svgGridGen.py [output image] [width] [height] [angles] [lines] [cuts]
usage: svgGridGen.py [output image] [width] [height] [angles] [lines] [cuts] [deviation 0-100%]
usage: svgGridGen.py [output image] [width] [height] [angles] [lines] [cuts] [deviation 0-100%] [punch 0-100%]
usage: svgGridGen.py [output image] [width] [height] [angles] [lines] [cuts] [deviation 0-100%] [punch 0-100%] [pinch 0-100%]
usage: svgGridGen.py [output image] [width] [height] [angles] [lines] [cuts] [deviation 0-100%] [punch 0-100%] [pinch 0-100%] [seed]

example: svgGridGen.py testImg 800 600 2 20 30 8 90 60 0
"""

if len(sys.argv) == 1:
	print(usageMsg)
	exit()

# default values
width = 800
height = 600
angles = 2
lines = 20
cuts = 30
deviation = 8
punch = 90
pinch = 60
seed = 0

if len(sys.argv) >= 2:
	filename = sys.argv[1]+".svg"

if len(sys.argv) >= 3:
	width = int(sys.argv[2])

if len(sys.argv) >= 4:
	height = int(sys.argv[3])
	
if len(sys.argv) >= 5:
	angles = int(sys.argv[4])
	
if len(sys.argv) >= 6:
	lines = int(sys.argv[5])
	
if len(sys.argv) >= 7:
	cuts = int(sys.argv[6])
	
if len(sys.argv) >= 8:
	deviation = float(sys.argv[7])
	
if len(sys.argv) >= 9:
	punch = float(sys.argv[8])
	
if len(sys.argv) >= 10:
	pinch = float(sys.argv[9])
	
if len(sys.argv) >= 11:
	seed = float(sys.argv[10])

if width > height:
	canvas_size = width
else:
	canvas_size = height


# create line grid
cutLength = canvas_size / cuts
edges = []
for iAngle in range(angles):
	degrees = iAngle * 180 / angles
	ang = degrees * pi / 180
	xUC = cos(ang)
	yUC = sin(ang)

	for i in range(int(lines * -1.5), int(lines * 1.5)): # (range * 1.5) for rotation radius
		y = i / lines * canvas_size
		for j in range(int(cuts * -1.5), int(cuts * 1.5)):
			x1 = cutLength * j
			x2 = cutLength * (j+1)
			edges.append([x1, y, x2, y])
			rotEdge(edges[-1], xUC, yUC)


# get key frames for punch and pinch
random.seed(int(seed))
xKF1A = random.random() * width # key frames for animation
yKF1A = random.random() * height
rKF1A = random.random() * canvas_size
xKF2A = random.random() * width
yKF2A = random.random() * height
rKF2A = random.random() * canvas_size

random.seed(int(seed)+1)
xKF1B = random.random() * width
yKF1B = random.random() * height
rKF1B = random.random() * canvas_size
xKF2B = random.random() * width
yKF2B = random.random() * height
rKF2B = random.random() * canvas_size

# punch
xLoc = xKF1A + (xKF1B - xKF1A) * (seed - int(seed))
yLoc = yKF1A + (yKF1B - yKF1A) * (seed - int(seed))
radius = rKF1A + (rKF1B - rKF1A) * (seed - int(seed))
force = punch / 100
for edge in edges:
	x1 = edge[0] - xLoc; y1 = edge[1] - yLoc
	x2 = edge[2] - xLoc; y2 = edge[3] - yLoc
	
	dist1 = sqrt(x1 * x1 + y1 * y1)
	dist2 = sqrt(x2 * x2 + y2 * y2)
	
	if dist1 < radius:
		punch = (1 - (dist1 / (radius * 2))) * 2
		x1 *= punch * force + (1 - force)
		y1 *= punch * force + (1 - force)
		edge[0] = x1 + xLoc
		edge[1] = y1 + yLoc
	
	if dist2 < radius:
		punch = (1 - (dist2 / (radius * 2))) * 2
		x2 *= punch * force + (1 - force)
		y2 *= punch * force + (1 - force)
		edge[2] = x2 + xLoc
		edge[3] = y2 + yLoc

# pinch
xLoc = xKF2A + (xKF2B - xKF2A) * (seed - int(seed))
yLoc = yKF2A + (yKF2B - yKF2A) * (seed - int(seed))
radius = rKF2A + (rKF2B - rKF2A) * (seed - int(seed))
force = pinch / 100
for edge in edges:
	x1 = edge[0] - xLoc; y1 = edge[1] - yLoc
	x2 = edge[2] - xLoc; y2 = edge[3] - yLoc
	
	dist1 = sqrt(x1 * x1 + y1 * y1)
	dist2 = sqrt(x2 * x2 + y2 * y2)
	
	if dist1 < radius:
		pinch = dist1 / radius
		x1 *= pinch * force + (1 - force)
		y1 *= pinch * force + (1 - force)
		edge[0] = x1 + xLoc
		edge[1] = y1 + yLoc
	
	if dist2 < radius:
		pinch = dist2 / radius
		x2 *= pinch * force + (1 - force)
		y2 *= pinch * force + (1 - force)
		edge[2] = x2 + xLoc
		edge[3] = y2 + yLoc


# random edge-point deviation
random.seed(seed)
pointRange = deviation / 100 * cutLength
for edge in edges:
	edge[0] += (random.random() * 2 - 1) * pointRange
	edge[1] += (random.random() * 2 - 1) * pointRange
	edge[2] += (random.random() * 2 - 1) * pointRange
	edge[3] += (random.random() * 2 - 1) * pointRange


# truncate edges out of window bounds
for i in range(len(edges)-1, -1, -1):
	x1 = edges[i][0]; y1 = edges[i][1]
	x2 = edges[i][2]; y2 = edges[i][3]
	
	# top window
	if y1 < 0 and y2 < 0:
		edges.pop(i)
		continue
	elif y1 < 0:
		edges[i][0] = adjustLengthToHeight(x1, y1, x2, y2, 0)
		edges[i][1] = 0
	elif y2 < 0:
		edges[i][2] = adjustLengthToHeight(x1, y1, x2, y2, 0)
		edges[i][3] = 0
	
	# bottom window
	if y1 > height-1 and y2 > height-1:
		edges.pop(i)
		continue
	elif y1 > height-1:
		edges[i][0] = adjustLengthToHeight(x1, y1, x2, y2, height-1)
		edges[i][1] = height-1
	elif y2 > height-1:
		edges[i][2] = adjustLengthToHeight(x1, y1, x2, y2, height-1)
		edges[i][3] = height-1

	# left window
	if x1 < 0 and x2 < 0:
		edges.pop(i)
		continue
	elif x1 < 0:
		edges[i][1] = adjustLengthToWidth(x1, y1, x2, y2, 0)
		edges[i][0] = 0
	elif x2 < 0:
		edges[i][3] = adjustLengthToWidth(x1, y1, x2, y2, 0)
		edges[i][2] = 0
	
	# right window
	if x1 > width-1 and x2 > width-1:
		edges.pop(i)
		continue
	elif x1 > width-1:
		edges[i][1] = adjustLengthToWidth(x1, y1, x2, y2, width-1)
		edges[i][0] = width-1
	elif x2 > width-1:
		edges[i][3] = adjustLengthToWidth(x1, y1, x2, y2, width-1)
		edges[i][2] = width-1




# write SVG image
filePtr = open(filename, 'w')
bgColor = 0xFFFFFF
thickness = 1.0
color = 0x0

filePtr.write("<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" width=\"%i\" height=\"%i\" viewBox=\"0 0 %i %i\">\n" % (width, height, width, height))
filePtr.write("<rect width=\"100%%\" height=\"100%%\" fill=\"#%06X\" />\n" % (bgColor))
filePtr.write("<style>path{fill:none;stroke-linecap:round}</style>\n")

filePtr.write("<path d=\"")

for edge in edges:
	filePtr.write("M%.3f %.3f L%.3f %.3f " % (edge[0], edge[1], edge[2], edge[3]))

filePtr.write("\" stroke-width=\"%.3fpx\" stroke=\"#%06X\" />\n" % (thickness, color))

filePtr.write("</svg>")
filePtr.close()


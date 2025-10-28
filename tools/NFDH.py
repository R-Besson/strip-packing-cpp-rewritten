# Simple very non-optimal popular NFDH (Next-Fit Decreasing Height)
# algorithm for strip packing

# Requires graphics.py (pip install graphics.py)
import math, sys
from graphics import *

win = GraphWin("NFDH Result", 800, 800)

def printUsage():
    print("Usage: <python> <pythonfile> <infile> <width>")
    print("Example:")
    print("py NFDH.py list.txt 1000  -> Generate packing for list.txt rectangles,")
    print("                             width strip of 1000 width")

if (len(sys.argv) < 3):
    printUsage()
    exit()

try:
    rectfile = open(sys.argv[1], "r")
except IOError:
    print("Could not open file!")
    exit()

def next_fit_decreasing_height(rects, W):
    # Sort the items by height in decreasing order
    rects.sort(key=lambda x: -x[1] )

    bins = [[]]
    current_strip_width = 0
    current_bin_height = 0

    current_height = 0

    total_area = 0
 
    # Iterate through the items
    for rect in rects:
        total_area += rect[0]*rect[1]
        width, height = rect[0], rect[1]

        if (width > W):
            print("Rectangle of size", str(width), str(height), "is too small")
            exit()

        # If the rect fits in the current bin, add it to the bin
        if (current_strip_width + width <= W):
            current_strip_width += width
            current_bin_height = max(current_bin_height, height)
            bins[-1].append(rect)
        else: # If the rect does not fit in the current bin, create a new bin with the rect
            current_height += current_bin_height
            
            bins.append([rect])
            current_strip_width = width
            current_bin_height = height

    current_height += current_bin_height

    return bins, current_height, total_area

items = []
for line in rectfile:
    vars = line.rstrip().split()
    items.append([int(vars[0]),int(vars[1])])

try:
    strip_width = int(float(sys.argv[2]))
except:
    print("Could not read width!")
    exit()

bins, strip_height, total_area = next_fit_decreasing_height(items, strip_width)
optimal = total_area/strip_width

margin = (400-int(strip_width/2), 400-int(strip_height/2))

strip = Rectangle(Point(margin[0], margin[1]), Point(margin[0]+strip_width, margin[1]+strip_height))
strip.draw(win)

print("ALG="+str(strip_height), "OPT="+str(optimal), "ALG/OPT="+str(strip_height/optimal))

currentH = margin[1]
for bin in bins:
    maxH, currentW = 0, margin[0]
    for rect in bin:
        width, height = rect[0], rect[1]

        rect = Rectangle(Point(currentW, currentH), Point(currentW+width, currentH+height))
        rect.setFill("green")
        rect.draw(win)

        currentW += width
        maxH = max(maxH, height)
    currentH += maxH

win.getMouse()
win.close()
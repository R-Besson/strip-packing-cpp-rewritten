# Generate a random list of rectangles
import os, sys, random

def printUsage():
    print("Usage: <python> <pyfile> <outfile> <nbRectangles> <minW> <maxW> <minH> <maxH>")
    print("Example:")
    print("py genlist.py list.txt 1000 1 100 2 200  -> Generate list.txt with 1000 rectangles,")
    print("                                            min width for a rectangle is 1, max is 100,")
    print("                                            min height for a rectangle is 2, max is 200")

if (len(sys.argv) < 5):
    printUsage()
    exit()

FILE = str(sys.argv[1])
N = int(sys.argv[2])
MIN = int(sys.argv[3])
MAX = int(sys.argv[4])

f = open(FILE, "w")
buff = ""
def get():
    return str(random.randint(MIN, MAX))
    
for i in range(N):
    buff += get()  + " " + get() + "\n"
buff = buff[:-1]

f.write(buff)
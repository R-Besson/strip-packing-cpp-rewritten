# Generate a random list of rectangles
import os, sys, random

def print_usage():
    print("Usage: <python> <pyfile> <outfile> <nbRectangles> <minW> <maxW> <minH> <maxH>")
    print("Example:")
    print("py genlist.py list.txt 1000 1 100 2 200  -> Generate list.txt with 1000 rectangles,")
    print("                                            min width for a rectangle is 1, max is 100,")
    print("                                            min height for a rectangle is 2, max is 200")

if (len(sys.argv) < 7):
    print_usage()
    exit()

FILE = str(sys.argv[1])
N = int(sys.argv[2])
MIN_W = int(sys.argv[3])
MAX_W = int(sys.argv[4])
MIN_H = int(sys.argv[5])
MAX_H = int(sys.argv[6])

f = open(FILE, "w")
buff = ""

for i in range(N):
    buff += str(random.randint(MIN_W, MAX_W)) + " " + str(random.randint(MIN_H, MAX_H)) + "\n"
buff = buff[:-1]

f.write(buff)
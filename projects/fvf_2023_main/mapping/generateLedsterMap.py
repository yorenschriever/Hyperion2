from math import *
import json

size = 9
msz = 10
teller = 0

f = 1/1.14
points = []

scale = 1/28*0.5

for y in range(-size, size+1):
    aantal = 2*size - abs(y)+1
    xstart = -aantal/2 + 0.5
    dir = -1 if y % 2 == 0 else 1

    for nr in range(aantal):
        x = (xstart + nr)*dir/f
        points.append({'x': -y*scale, 'y': x*scale})
        teller = teller+1

print('aantal leds midden: ', teller)

for a in range(6):
    hsize = size/f
    begin = a*360//6
    x = -hsize * sin((begin/360)*2*pi-2*pi/12)
    y =  hsize * cos((begin/360)*2*pi-2*pi/12)
    t = 0
    for th in range(begin+72, begin+360, 72):
        for p in range(size):
            x = x + cos(th/360*2*pi)/f
            y = y + sin(th/360*2*pi)/f
            if t != 4*size-1:
                points.append({'x': -y*scale, 'y': x*scale})
                teller = teller+1
            t = t+1

print('number of leds: ', len(points))

# with open("map.json", "w") as outfile:
#     json.dump(points, outfile)

f = open("ledsterMap.hpp", "w")
f.write("PixelMap ledsterMap = {\n")
for point in points:
    f.write("    {.x = " + str(point['x']) + ", .y = " + str(point['y']) + "},\n")
f.write("};\n")
f.close()
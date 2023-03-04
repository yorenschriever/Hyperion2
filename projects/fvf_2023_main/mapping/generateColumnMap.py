from math import *
import json

points = []
scale = 0.5
pitch = 0.006

def line(x_start,y_start,angle, globalAngle):
    a = globalAngle/360*2*pi
    x = x_start * cos(a) + y_start * sin(a)
    y = y_start * cos(a) - x_start * sin(a)

    for p in range(60):
        x = x + cos((angle-globalAngle)/360*2*pi) * pitch
        y = y + sin((angle-globalAngle)/360*2*pi) * pitch
        points.append({'x': x*scale, 'y': y*scale})

def column(angle):
    y = 1.2 
    x = 0 

    #3 staande
    line(x,        y,         90 , angle)
    line(x + 0.02, y + 0.25,  90 , angle)
    line(x,        y + .5,    90 , angle)
    
    # # V
    line(x,   y,   22-90 , angle)
    line(x,   y,  -22-90 , angle)

    # # buiten V
    line(x + 0.05,  y-0.15,   65-90 , angle)
    line(x - 0.05,  y-0.15,  -65-90 , angle)

    # # horizontaal
    line(x + 0.35 ,  y-0.2,  -30 , angle)

for i in range(6):
    column(i*360/6)


print('number of leds: ', len(points))

# with open("map.json", "w") as outfile:
#     json.dump(points, outfile)

f = open("columnMap.hpp", "w")
f.write("PixelMap columnMap = {\n")
for point in points:
    f.write("    {.x = " + str(point['x']) + ", .y = " + str(point['y']) + "},\n")
f.write("};\n")
f.close()
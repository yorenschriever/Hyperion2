from math import *
import json

points = []
scale = 0.5
pitch = 0.005

def line(x_start,y_start,angle):
    x = x_start
    y = y_start
    for p in range(60):
        x = x + cos(angle/360*2*pi) * pitch
        y = y + sin(angle/360*2*pi) * pitch
        points.append({'x': x*scale, 'y': y*scale})

def column(x):
    #3 staande
    line(x,0,90)
    line(x+0.02,0.25,90)
    line(x,0.5,90)
    
    # V
    line(x,0,22-90)
    line(x,0,-22-90)

    # buiten V
    line(x + 0.05,-0.15,65-90)
    line(x - 0.05,-0.15,-65-90)

    # horizontaal
    line(x + 0.375*6/5,-0.3,0)

for i in range(6):
    column(-3 + i*1*(6/5))

print('number of leds: ', len(points))

# with open("map.json", "w") as outfile:
#     json.dump(points, outfile)

f = open("map.hpp", "w")
f.write("PixelMap map = {\n")
for point in points:
    f.write("    {.x = " + str(point['x']) + ", .y = " + str(point['y']) + "},\n")
f.write("};\n")
f.close()
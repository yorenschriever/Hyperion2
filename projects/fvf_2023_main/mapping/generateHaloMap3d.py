from math import *
import json

points = []

scale = 0.284
heightInMeters = 2.5
height = heightInMeters * scale - 0.4

for i in range(50):
    x = -4 + cos(float(i)/50*2*pi)
    y = 0 + sin(float(i)/50*2*pi)
    points.append({'x': -y*scale, 'y': height, 'z': x*scale})

print('number of leds: ', len(points))

f = open("haloMap3d.hpp", "w")
f.write("PixelMap3d haloMap3d = {\n")
for point in points:
    f.write("    {.x = " + str(point['x']) + ", .y = " + str(point['y']) + ", .z = " + str(point['z']) + "},\n")
f.write("};\n")
f.close()
from math import *

points = []

scale = 0.284
heightInMeters = 2.5
height = heightInMeters * scale - 0.4
numLeds = 96

for i in range(numLeds):
    x = -4 + cos(float(i)/numLeds*2*pi)
    y = 0 + sin(float(i)/numLeds*2*pi)
    points.append({'x': -y*scale, 'y': height, 'z': x*scale})

print('number of leds: ', len(points))

f = open("haloMap3d.hpp", "w")
f.write("PixelMap3d haloMap3d = {\n")
for point in points:
    f.write("    {.x = " + str(point['x']) + ", .y = " + str(point['y']) + ", .z = " + str(point['z']) + "},\n")
f.write("};\n")
f.close()
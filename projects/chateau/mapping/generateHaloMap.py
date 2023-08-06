from math import *

points = []

scale=1
radius = 0.8
numLeds = 12

# Create a list of coordinates.
# Coordinates should be in the range [-1,1 X [-1,1].
# This guarantees the the monitor will display al coordinates, 
# and makes it easy to re-use mapped patterns on other maps
for i in range(numLeds):
    x = scale * cos(float(i)/numLeds*2*pi)
    y = scale * sin(float(i)/numLeds*2*pi)
    points.append({'x': x*radius, 'y': y*radius})

print('number of leds: ', len(points))

# write the coordinates to a file
f = open("haloMap.hpp", "w")
f.write("PixelMap haloMap = {\n")
for point in points:
    f.write("    {.x = " + str(point['x']) + ", .y = " + str(point['y'])+ "},\n")
f.write("};\n")
f.close()
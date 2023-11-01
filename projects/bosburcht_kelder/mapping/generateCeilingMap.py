from math import cos, sin,pi

points=[]
for y in range(8):
    angle = y/7 * 2 * pi *0.5
    y3d = sin(angle)
    z3d = cos(angle)
    for x in range(600):
        points.append({'x': x/300 -1, 'y': (y/7 - 0.5) * 0.5, 'y3d': y3d, 'z3d':z3d})   

print ("ceiling map leds: ", len(points))

f = open("ceilingMap.hpp", "w")
f.write("PixelMap ceilingMap = {\n")
for point in points:
    f.write("    {.x = " + str(point['x']) + ", .y = " + str(point['y']) + "},\n")
f.write("};\n\n")
f.close()

f = open("ceilingMap3dCombined.hpp", "w")
f.write("PixelMap3d ceilingMap3dCombined = {\n")
for point in points:
    f.write("    {.z = " + str(point['x']) + ", .y = " + str(point['y3d']) + ", .x = " + str(point['z3d']) + "},\n")
f.write("};\n\n")
f.close()


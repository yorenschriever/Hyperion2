from math import cos, sin,pi

points=[]
for y in range(20):
    angle = y/20 * 2 * pi *0.5
    x = sin(angle) * 0.5
    y = cos(angle) * 0.5
    points.append({'x': x, 'y': y})   

f = open("colanderMap.hpp", "w")
f.write("PixelMap colanderMap = {\n")
for point in points:
    f.write("    {.x = " + str(point['x'] + 0.75) + ", .y = " + str(point['y']) + "},\n")
f.write("};\n\n")
f.close()

f = open("colanderMap3dCombined.hpp", "w")
f.write("PixelMap3d colanderMap3dCombined = {\n")
for point in points:
    f.write("    {.z = " + str(-1) + ", .y = " + str(point['x']) + ", .x = " + str(point['y']) + "},\n")
f.write("};\n\n")
f.close()



ledsPerLine = 32
ledDistance = 0.02
d = ledsPerLine * ledDistance

points = []


def square(x_0, y_0):
    for x in range(ledsPerLine):
        for y in range(ledsPerLine):
            points.append({'x': x_0+x*ledDistance-d/2+ledDistance/2,
                          'y': y_0+y*ledDistance-d/2+ledDistance/2})


square(0, -d)

square(-d, 0)
square(0, 0)
square(d, 0)

square(0, d)


f = open("crossMap.hpp", "w")
f.write("PixelMap crossMap = {\n")
for point in points:
    f.write("    {.x = " + str(point['x']) +
            ", .y = " + str(point['y']) + "},\n")
f.write("};\n\n")
f.close()

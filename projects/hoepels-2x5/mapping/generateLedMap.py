import math

rows = 2
cols = 5
circle = 50
r = 0.18

f = open("ledMap.hpp", "w")
f.write("PixelMap ledMap = {\n")

for col in range(cols):
    for row in range(rows):
        yc = (-rows/cols) + float(row + 0.5) * 2 / cols
        xc = -1 + float(col + 0.5) * 2 / cols
        for step in range(circle):
            a = 2*math.pi / (circle) * step
            x = xc + r * math.cos(a)
            y = yc + r * math.sin(a)
            f.write("    {.x = " + str(x) + ", .y = " + str(y) + "},\n")

f.write("};")
f.close()

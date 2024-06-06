
rows = 8
cols = 5

f = open("ledMap.hpp", "w")
f.write("PixelMap ledMap1 = {\n")

for row in range(rows):
    y = -1 + float(row + 0.5) * 2 / rows
    for col in range(cols):
        x = -1 + float(col) * 2 / cols
        f.write("    {.x = " + str(x) + ", .y = " + str(y) + "},\n")


f.write("};")
f.close()

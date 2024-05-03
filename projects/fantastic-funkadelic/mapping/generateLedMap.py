
rows = 12
cols = 6
leds = 60

def write(start, end):
    for row in range(start,end):
        y_bar = -1 + float(row + 0.5) * 2 / rows
        for col in range(cols):
            x_bar = -1 + float(col) * 2 / cols
            for led in range(leds):
                bar_width = 1.8 / rows
                x = x_bar + float(led) / leds * bar_width
                y = y_bar
                f.write("    {.x = " + str(x) + ", .y = " + str(y) + "},\n")

f = open("ledMap.hpp", "w")

f.write("PixelMap ledMap1 = {\n")
write(0,8)
f.write("};")

f.write("\n\n")

f.write("PixelMap ledMap2 = {\n")
write(8,12)
f.write("};")

f.close()

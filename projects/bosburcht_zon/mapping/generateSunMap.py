from turtle import *
from math import sqrt, cos, asin

turtle = Turtle()

# turtle begins facing to the right
# turning < 0 is counter clockwise

ledsPerLine = 60
ledDistance = 0.005


def ledLine():
    for i in range(ledsPerLine):
        turtle.move(ledDistance)


def triangle():
    turtle.turn(-90)
    turtle.move(ledDistance * ledsPerLine/2, False)
    for i in range(3):
        turtle.turn(120)
        ledLine()  
    turtle.turn(90)


for angle in range(7):
    # go to the start of the inner triangle
    turtle.setPosition(0, 0)
    turtle.setDirection(-90 + (angle+0.5)/7 * 360)
    turtle.move(0.35, False)
    triangle()

    # go to the start of the outer triangle
    turtle.setPosition(0, 0)
    turtle.setDirection(-90 + angle/7 * 360)
    turtle.move(0.35+ledDistance * ledsPerLine, False)
    triangle()

f = open("sunMap.hpp", "w")
f.write("PixelMap sunMap = {\n")
for point in turtle.trail:
    f.write("    {.x = " + str(point['x']) + ", .y = " + str(point['y']) + "},\n")
f.write("};\n\n")
f.close()

def find_z(pos):
    r = sqrt(pos['x']**2 + pos['y']**2)
    estimate1 = cos(asin(r))
    estimate2 = 1-0.5*r
    return (estimate1 + estimate2)/2 - 0.25

f = open("sunMap3d.hpp", "w")
f.write("PixelMap3d sunMap3d = {\n")
for point in turtle.trail:
    f.write("    {.x = " + str(point['x']) + ", .z = " + str(point['y']) + ", .y = " + str(find_z(point)) + "},\n")
f.write("};\n\n")
f.close()
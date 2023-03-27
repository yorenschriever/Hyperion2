from math import *
import json

points = []
scale = 1
pitch = 0.006

def line(x_start,y_start,z_start,theta, phi,globalPhi):
    a = globalPhi/360*2*pi
    x = x_start * cos(a) + y_start * sin(a)
    y = y_start * cos(a) - x_start * sin(a)
    z = z_start

    for p in range(60):
        phiRad = (-phi-globalPhi)/360*2*pi
        thetaRad = (theta)/360*2*pi
        x = x + pitch * sin(thetaRad) * cos(phiRad)
        y = y + pitch * sin(thetaRad) * sin(phiRad)
        z = z + pitch * cos(thetaRad) 
        points.append({'x': y*scale, 'y': z*scale, 'z': x*scale })

def column(angle):
    y = 1 
    x = 0 

    #3 staande
    line(x,   y,      0,      0,0 , angle)
    line(x,   y+0.05,-0.2,  0,0 , angle)
    line(x,   y,      -0.4,   0,0 , angle)
    
    #kraaienpoot
    line(x,   y,      0.4,    45, 90+45 , angle)
    line(x,   y,      0.4,    45, 90    ,  angle)
    line(x,   y,      0.4,    45, 90-45 , angle)

    line(x+0.15,   y,      0.4,    90,  0 , angle)
    line(x-0.15,   y,      0.4,    -90, 0 , angle)

    # line(x-0.35,   y-0.35,      0.55,    -90, -45 , angle)

    # # # V
    # line(x,   y,   22-90 , angle)
    # line(x,   y,  -22-90 , angle)

    # # # buiten V
    # line(x + 0.05,  y-0.15,   65-90 , angle)
    # line(x - 0.05,  y-0.15,  -65-90 , angle)

    # # # horizontaal
    # line(x + 0.35 ,  y-0.2,  -30 , angle)

for i in range(6):
    column(i*360/6)


print('number of leds: ', len(points))

# with open("map.json", "w") as outfile:
#     json.dump(points, outfile)

f = open("columnMap3d.hpp", "w")
f.write("PixelMap3d columnMap3d = {\n")
for point in points:
    f.write("    {.x = " + str(point['x']) + ", .y = " + str(point['y']) + ", .z = " + str(point['z']) + "},\n")
f.write("};\n")
f.close()
from math import *
import json

points = []
scale = 1
pitch = 0.00476

def line(x_start,y_start,z_start,theta, phi,globalPhi, reverse=False):
    a = globalPhi/360*2*pi
    x1 = x_start * cos(a) + y_start * sin(a)
    y1 = y_start * cos(a) - x_start * sin(a)
    z1 = z_start

    for p2 in range(60):
        p = 59-p2 if reverse else p2
        phiRad = (-phi-globalPhi)/360*2*pi
        thetaRad = (theta)/360*2*pi
        x = x1 + pitch * p * sin(thetaRad) * cos(phiRad)
        y = y1 + pitch * p * sin(thetaRad) * sin(phiRad)
        z = z1 + pitch * p * cos(thetaRad) 
        points.append({'x': y*scale, 'y': z*scale, 'z': x*scale })

def column(angle):
    y = 1 
    x = 0 

    lengte = 60*pitch
    hoogtekraaienpoot = lengte * cos(30 / 360 * 2*pi)
    hoogteSter = 0.45

    # print ("bottom",hoogteSter-hoogtekraaienpoot-2*lengte)

    # vanaf boven bekeken een wijzerplaat voorstellen
    # vroeger = tegen de klok in
    # later = met de klok mee
    # het horizontale paar zit later dan de kolom

    #kraaienpoot vroeger
    line(x,   y,      hoogteSter-hoogtekraaienpoot,    45, 90+45 , angle, True)

    #kraaienpoot later
    line(x,   y,      hoogteSter-hoogtekraaienpoot,    45, 90-45 , angle)

    #kraaienpoot midden
    line(x,   y,      hoogteSter-hoogtekraaienpoot,    45, 90    ,  angle,True)

    #staande boven
    line(x,   y,      hoogteSter-hoogtekraaienpoot-1*lengte,   0,0 , angle, True)

    #staande onder
    line(x,   y,      hoogteSter-hoogtekraaienpoot-2*lengte,      0,0 , angle, True)

    #staande midden
    line(x,   y+0.05, hoogteSter-hoogtekraaienpoot-1.5*lengte,  0,0 , angle)

    if (angle == 0 or angle == 120 or angle == 240):
        #horizontaal later
        line(x-0.27,   y,      hoogteSter,    -90, 0 , angle +0)

        #horizontaal vroeger
        line(x+0.27,   y,      hoogteSter,    90,  0 , angle -60, True)
    else :
        #horizontaal vroeger
        line(x+0.27,   y,      hoogteSter,    90,  0 , angle -60)

        #horizontaal later
        line(x-0.27,   y,      hoogteSter,    -90, 0 , angle +0, True)

    #dummy
    #line(x,   y,      hoogteSter-hoogtekraaienpoot,    45, 90    ,  angle,True)


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
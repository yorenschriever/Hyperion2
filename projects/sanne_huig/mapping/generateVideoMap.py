from math import cos, sin,pi
import json

length = 8*60

points=[]
for y in range(2):
    for x in range(length):
        points.append({'x': 2*x/length -1, 'y': y - 0.5})   

print ("video map leds: ", len(points))

with open("videoMap.json", "w") as outfile:
    json.dump(points, outfile)



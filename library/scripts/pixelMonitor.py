
# to run:
# pip3 install pyglet
# python3 pixelMonitor.py monitorMap.json

import pyglet
import sys
import json
from socket import *

f = open(sys.argv[1])
pixelMap = json.load(f)
f.close()

window = pyglet.window.Window(resizable=True)

dotsize = 1/200 #1/250

batchCircles = pyglet.graphics.Batch()
batchSomeLabels = pyglet.graphics.Batch()
batchAllLabels = pyglet.graphics.Batch()

for pixelSetIndex, pixelSet in enumerate(pixelMap):
    pixelSet['circles'] = []
    pixelSet['labels'] = []

    for i, position in enumerate(pixelSet['positions']):
        pixelSet['circles'].append(pyglet.shapes.Circle(x=0, y=0, radius=5, color=(50, 50, 50),batch=batchCircles))
        batchLabels = batchSomeLabels if i%60==0 else batchAllLabels
        pixelSet['labels'].append(pyglet.text.Label(str(i),font_size=10,
                            anchor_x='center', anchor_y='center', batch=batchLabels))

    sock = socket(AF_INET, SOCK_DGRAM)
    sock.bind(("0.0.0.0", pixelSet['port']))
    sock.setblocking(False)

    pixelSet['socket'] = sock
    

line1 = pyglet.shapes.Line(0,0,5000,5000)
line2 = pyglet.shapes.Line(0,0,5000,5000)

@window.event
def on_resize(width, height):
    scale = min(width,height)
    for pixelSet in pixelMap:
        for i, circle in enumerate(pixelSet['circles']):
            x = int((pixelSet['positions'][i]['x'])*scale/2) + width//2
            y = int((pixelSet['positions'][i]['y'])*scale/-2) + height//2
            circle.x = x
            circle.y = y
            circle.radius = int(scale * dotsize)
            pixelSet['labels'][i].x = x
            pixelSet['labels'][i].y = y
    line1.x = width//2
    line1.x2 = width//2
    line2.y = height//2
    line2.y2 = height//2

@window.event
def on_draw():
    window.clear()
    batchCircles.draw()
    # batchSomeLabels.draw()
    # batchAllLabels.draw();
    # line1.draw()
    # line2.draw()
    
def update(dt):
    for pixelSet in pixelMap:
        msg = ""
        while True:
            try:
                msg = pixelSet['socket'].recv(1440*2)
            except:
                break
            if not msg or len(msg)==0:
                break

            #print ("got frame")

        if not msg or len(msg)==0:
            continue;

        circles = pixelSet['circles']
        size = min(len(msg)-2, len(circles)*3)
        for i in range(0,size,3):
            r = msg[i+0]
            g = msg[i+1]
            b = msg[i+2]
            circles[i//3].color = (r,g,b)

try:
    pyglet.clock.schedule_interval(update,1/60.0)
    pyglet.app.run()
except KeyboardInterrupt:
    sys.exit(0) 
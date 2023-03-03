
# to run:
# pip3 install pyglet
# python3 pixelMonitor.py pixelMap.json 9611

import pyglet
import sys
import json
from socket import *

f = open(sys.argv[1])
pixelMap = json.load(f)
f.close()

port = int(sys.argv[2])

window = pyglet.window.Window(resizable=True)

dotsize = 1/100 #1/250

circles = []
labels = []
for i, position in enumerate(pixelMap):
    circles.append(pyglet.shapes.Circle(x=0, y=0, radius=5, color=(50, 50, 50)))
    labels.append(pyglet.text.Label(str(i),font_size=10,
                          anchor_x='center', anchor_y='center'))

sock = socket(AF_INET, SOCK_DGRAM)
sock.bind(("0.0.0.0", port))
sock.setblocking(False)

line1 = pyglet.shapes.Line(0,0,5000,5000)
line2 = pyglet.shapes.Line(0,0,5000,5000)

@window.event
def on_resize(width, height):
    scale = min(width,height)
    for i, circle in enumerate(circles):
        x = int((pixelMap[i]['x'])*scale/2) + width//2
        y = int((pixelMap[i]['y'])*scale/-2) + height//2
        circle.x = x
        circle.y = y
        circle.radius = int(scale * dotsize)
        labels[i].x = x
        labels[i].y = y
    line1.x = width//2
    line1.x2 = width//2
    line2.y = height//2
    line2.y2 = height//2

@window.event
def on_draw():
    window.clear()
    for circle in circles:
        circle.draw()
    #for label in labels:
    #    label.draw()
    #line1.draw()
    #line2.draw()
    
def update(dt):
    while True:
        try:
            msg = sock.recv(4096)
        except:
            break
        if not msg or len(msg)==0:
            break

        #print ("got frame")

        size = min(len(msg)-2, len(pixelMap)*3)
        for i in range(0,size,3):
            r = msg[i+0]
            g = msg[i+1]
            b = msg[i+2]
            circles[i//3].color = (r,g,b)

pyglet.clock.schedule_interval(update,1/60.0)
pyglet.app.run()

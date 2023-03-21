
# to run:
# pip3 install pyglet
# python3 pixelMonitor3d.py monitorMap.json

import pyglet
import sys
import json
from socket import *
from pyglet.gl import *
from pyglet.math import Mat4, Vec3
import random
from math import *

f = open(sys.argv[1])
pixelMap = json.load(f)
f.close()

try:
    # Try and create a window with multisampling (antialiasing)
    config = Config(sample_buffers=1, samples=4, depth_size=16, double_buffer=True)
    window = pyglet.window.Window(width=960, height=540, resizable=True, config=config)
    # print ("full")
except pyglet.window.NoSuchConfigException:
    # Fall back to no multisampling if not supported
    window = pyglet.window.Window(resizable=True)
    # print ("fallback")

dotsize = 1/50 
num_vertices=0

@window.event
def on_draw():
    window.clear()
    batch.draw()


@window.event
def on_resize(width, height):
    window.viewport = (0, 0, *window.get_framebuffer_size())
    window.projection = Mat4.perspective_projection(window.aspect_ratio, z_near=0.1, z_far=255, fov=80)
    # glViewport(0, 0, width, height)
    glViewport(0, 0, width*2, height*2)
    return pyglet.event.EVENT_HANDLED

def update(dt):
    global time, last_fps, frames
    time += dt

    frames = frames+1
    if (time - last_fps > 1):
        last_fps = time
        print("fps",frames)
        frames=0
    
    updateLights()

    rot_y = Mat4.from_rotation(time/8, Vec3(0, 1, 0))
    rot_z = Mat4.from_rotation(-0.1, Vec3(1, 0, 0))
    # i apply the vector to the grid only, but everything in the batch rotates with it.
    # i dont know why. but this is what i want, so why bother 
    grid_model.matrix = Mat4.from_translation((0, 0, -2)) @rot_z @ rot_y

def setup():
    # One-time GL setup
    glClearColor(0.1, 0.1, 0.1, 1)
    glEnable(GL_DEPTH_TEST)
    glEnable(GL_CULL_FACE)
    on_resize(*window.size)

    # Uncomment this line for a wireframe view:
    # glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)

def sphere_shape(cx,cy,cz):
    # http://www.songho.ca/opengl/gl_sphere.html
    vertices = []
    normals = []

    r=0.02
    stacks=2
    sectors=4

    for i in range(stacks+1):
        phi = pi/2 - pi * float(i) / stacks
        
        for j in range(sectors+1):
            theta = 2 * pi * float (j) / sectors            

            x = cos(phi) * cos(theta) 
            y = cos(phi) * sin(theta)
            z = sin(phi)

            vertices.extend([r*x+cx, r*y+cy, r*z+cz])
            normals.extend( [  x,   y,   z])
        
    # Create a list of triangle indices.
    indices = []
    for i in range(stacks):
        k1 = i * (sectors+1)
        k2 = k1 + sectors+1
        for j in range(sectors):
            if i != 0:
                indices.extend([k1,k2,k1+1])
            if i != stacks-1:
                indices.extend([k1+1,k2,k2+1])
            k1 = k1+1
            k2 = k2+1

    return [vertices, normals, indices]

def createMaterial(r,g,b):
    diffuse = [r, g, b, 1.0]
    ambient = [r, g, b, 1.0]
    specular = [1.0, 1.0, 1.0, 1.0]
    emission = [0.0, 0.0, 0.0, 1.0]
    shininess = 50

    material = pyglet.model.Material("custom", diffuse, ambient, specular, emission, shininess)
    return material

def sphere(x,y,z, shader, batch):
    global num_vertices
    [vertices, normals, indices] = sphere_shape(x,y,z)        
    material = createMaterial(0.1,0.1,0.1)
    group = pyglet.model.MaterialGroup(material=material, program=shader)

    num_vertices = len(vertices)//3

    vertex_list = shader.vertex_list_indexed(num_vertices, GL_TRIANGLES, indices, batch, group,
                                             vertices=('f', vertices),
                                             normals=('f', normals),
                                             colors=('f', material.diffuse * (num_vertices)))
    
    return pyglet.model.Model([vertex_list], [group], batch )

def grid():

    slices = 10
    vertices = []
    indices = []
    normal = [0,1,0]

    for j in range (slices+1):
        for i in range(slices+1):
            x = -1. + 2.*float(i)/float(slices)
            y = -1.0
            z = -1. + 2.*float(j)/float(slices)
            vertices.extend([x,y,z]) 

    for j in range (slices):
        for i in range(slices):
            row1 = j * (slices+1)
            row2 = (j+1) * (slices+1)

            indices.extend([row1+i, row1+i+1, row1+i+1, row2+i+1]) 
            indices.extend([row2+i+1, row2+i, row2+i, row1+i]) 

    material = createMaterial(1,1,1) 
    group = pyglet.model.MaterialGroup(material=material, program=shader)

    vertex_list = shader.vertex_list_indexed(len(vertices)//3, GL_LINES, indices, batch, group,
                                             vertices=('f', vertices),
                                             normals=('f', normal* (len(vertices) // 3)),
                                             colors=('f', material.diffuse * (len(vertices) // 3)))

    return pyglet.model.Model([vertex_list], [group], batch )

def drawLights():
    global pixelMap

    for pixelSet in pixelMap:
        pixelSet['circles'] = []
        pixelSet['labels'] = []

        for position in pixelSet['positions']:
            pixelSet['circles'].append(
                sphere(
                    float(position['x']),
                    float(position['y']),
                    float(position['z']),
                    shader, 
                    batch
                )
            )

        # print("creating socket")
        sock = socket(AF_INET, SOCK_DGRAM)
        sock.bind(("0.0.0.0", pixelSet['port']))
        sock.setblocking(False)

        pixelSet['socket'] = sock

    
def updateLights():
    global pixelMap,num_vertices
    for pixelSet in pixelMap:
        msg = ""
        # print ("checking socket")
        while True:
            try:
                msg = pixelSet['socket'].recv(1440*2)
            except:
                break
            if not msg or len(msg)==0:
                break

            # print ("got frame")

        if not msg or len(msg)==0:
            continue;

        circles = pixelSet['circles']
        size = min(len(msg)-2, len(circles)*3)
        for i in range(0,size,3):
            material = createMaterial(
                float(msg[i+0])/255.,
                float(msg[i+1])/255.,
                float(msg[i+2])/255.
            )
            circles[i//3].vertex_lists[0].set_attribute_data('colors',  material.diffuse * (num_vertices))


setup()
time = 0.0
last_fps=0.0
frames=0

batch = pyglet.graphics.Batch()
shader = pyglet.model.get_default_shader()

grid_model = grid()
corner1 = sphere( 1, 1, 1, shader, batch)
corner2 = sphere( 1,-1, 1, shader, batch)
corner3 = sphere(-1,-1, 1, shader, batch)
corner4 = sphere(-1, 1, 1, shader, batch)
corner5 = sphere( 1, 1,-1, shader, batch)
corner6 = sphere( 1,-1,-1, shader, batch)
corner7 = sphere(-1,-1,-1, shader, batch)
corner8 = sphere(-1, 1,-1, shader, batch)

drawLights()

try:
    pyglet.clock.schedule_interval(update, 1./60.)
    pyglet.app.run()
except KeyboardInterrupt:
    sys.exit(0) 




# batchCircles = pyglet.graphics.Batch()
# batchLabels = pyglet.graphics.Batch()

# for pixelSetIndex, pixelSet in enumerate(pixelMap):
#     pixelSet['circles'] = []
#     pixelSet['labels'] = []

#     for i, position in enumerate(pixelSet['positions']):
#         # pixelSet['circles'].append(pyglet.shapes.Circle(x=0, y=0, radius=5, color=(50, 50, 50),batch=batchCircles))
#         # pixelSet['labels'].append(pyglet.text.Label(str(i),font_size=10,
#         #                     anchor_x='center', anchor_y='center', batch=batchLabels))

#     sock = socket(AF_INET, SOCK_DGRAM)
#     sock.bind(("0.0.0.0", pixelSet['port']))
#     sock.setblocking(False)

#     pixelSet['socket'] = sock
    

# line1 = pyglet.shapes.Line(0,0,5000,5000)
# line2 = pyglet.shapes.Line(0,0,5000,5000)

# @window.event
# def on_resize(width, height):
#     scale = min(width,height)
#     for pixelSet in pixelMap:
#         for i, circle in enumerate(pixelSet['circles']):
#             x = int((pixelSet['positions'][i]['x'])*scale/2) + width//2
#             y = int((pixelSet['positions'][i]['y'])*scale/-2) + height//2
#             circle.x = x
#             circle.y = y
#             circle.radius = int(scale * dotsize)
#             pixelSet['labels'][i].x = x
#             pixelSet['labels'][i].y = y
#     line1.x = width//2
#     line1.x2 = width//2
#     line2.y = height//2
#     line2.y2 = height//2

# @window.event
# def on_draw():
#     window.clear()

#     glMatrixMode(GL_PROJECTION)
#     glLoadIdentity()
#     gluPerspective(90, 1, 0.1, 100)

#     batchCircles.draw()
#     # batchLabels.draw();
#     # line1.draw()
#     # line2.draw()

#     glFlush()
    
# def update(dt):
#     for pixelSet in pixelMap:
#         msg = ""
#         while True:
#             try:
#                 msg = pixelSet['socket'].recv(1440*2)
#             except:
#                 break
#             if not msg or len(msg)==0:
#                 break

#             #print ("got frame")

#         if not msg or len(msg)==0:
#             continue;

#         circles = pixelSet['circles']
#         size = min(len(msg)-2, len(circles)*3)
#         for i in range(0,size,3):
#             r = msg[i+0]
#             g = msg[i+1]
#             b = msg[i+2]
#             circles[i//3].color = (r,g,b)

# try:
#     pyglet.clock.schedule_interval(update,1/60.0)
#     pyglet.app.run()
# except KeyboardInterrupt:
#     sys.exit(0) 
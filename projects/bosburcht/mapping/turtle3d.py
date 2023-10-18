from math import pi, cos, sin

class Turtle3d:

    pos_x=0
    pos_y=0
    pos_z=0
    direction_hor=0
    direction_ver=0
    trail=[]
    
    def __init__(self, pos_x=0, pos_y=0, pos_z=0, direction_hor=0, direction_ver=0):
        self.pos_x = pos_x
        self.pos_y = pos_y
        self.pos_z = pos_z
        self.direction_hor = direction_hor
        self.direction_ver = direction_ver

    def turnHorizontal(self,angle):
        self.direction_hor += angle
        if (self.direction_hor < 0): 
            self.direction_hor += 360
        if (self.direction_hor >= 360): 
            self.direction_hor -= 360

    def turnVertical(self,angle):
        self.direction_ver += angle
        if (self.direction_ver < 0): 
            self.direction_ver += 360
        if (self.direction_ver >= 360): 
            self.direction_ver -= 360

    def move(self,distance, addPositionToTrail=True):
        self.pos_x += distance * sin(self.direction_ver * 2 * pi / 360) * cos(self.direction_hor * 2 * pi / 360)
        self.pos_y += distance * sin(self.direction_ver * 2 * pi / 360) * sin(self.direction_hor * 2 * pi / 360)
        self.pos_z += distance * cos(self.direction_ver * 2 * pi / 360)
        if addPositionToTrail:
            self.addPositionToTrail()

    def addPositionToTrail(self):
        self.trail.append({'x': self.pos_x, 'y': self.pos_y, 'z': self.pos_z})

    def clearTrail(self):
        self.trail=[]

    def setPosition(self, pos_x, pos_y, pos_z):
        self.pos_x = pos_x
        self.pos_y = pos_y
        self.pos_z = pos_z

    def setDirection(self, direction_hor, direction_ver):
        self.direction_hor = direction_hor
        self.direction_ver = direction_ver
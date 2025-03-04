from math import pi, cos, sin

class Turtle:

    pos_x=0
    pos_y=0
    direction=0
    trail=[]
    
    def __init__(self, pos_x=0, pos_y=0, direction=0):
        self.pos_x = pos_x
        self.pos_y = pos_y
        self.direction = direction
        self.trail = []

    def turn(self,angle):
        self.direction += angle
        if (self.direction < 0): 
            self.direction += 360
        if (self.direction >= 360): 
            self.direction -= 360

    def move(self,distance, addPositionToTrail=True):
        self.pos_x += distance * cos(self.direction * 2 * pi / 360)
        self.pos_y += distance * sin(self.direction * 2 * pi / 360)
        if addPositionToTrail:
            self.addPositionToTrail()

    def addPositionToTrail(self):
        self.trail.append({'x': self.pos_x, 'y': self.pos_y})

    def clearTrail(self):
        self.trail=[]

    def setPosition(self, pos_x, pos_y):
        self.pos_x = pos_x
        self.pos_y = pos_y

    def setDirection(self, direction):
        self.direction = direction
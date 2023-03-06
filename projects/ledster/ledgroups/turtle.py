from cursor import *

class Turtle:

    cursor = Cursor(0)
    direction = 0
    
    def __init__(self, position, direction):
        self.cursor = Cursor(position)
        self.direction = direction

    def turn(self,angle):
        self.direction += angle
        if (self.direction < 0): 
            self.direction += 6
        if (self.direction > 5): 
            self.direction -= 6

    def move(self,distance):
        if (self.direction == 0):
            self.cursor.left(distance)
        elif (self.direction == 1):
            self.cursor.upleft(distance)
        elif (self.direction == 2):
            self.cursor.upright(distance)
        elif (self.direction == 3):
            self.cursor.right(distance)       
        elif (self.direction == 4):
            self.cursor.downright(distance)
        elif (self.direction == 5):
            self.cursor.downleft(distance)

    def petalccw(self):
        self.cursor.petalccw()

    def petalcw(self):
        self.cursor.petalcw()

    def printTrail(self):
        self.cursor.printTrail()

    def clearTrail(self):
        self.cursor.clearTrail()

    def setPosition(self, pos):
        self.cursor.position = pos

    def setDirection(self, direction):
        self.direction = direction
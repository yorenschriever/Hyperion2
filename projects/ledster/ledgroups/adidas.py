from turtle import *
from cursor import printList


t = Turtle(0,0)
for j in range (6):
    t.move(9)
    t.turn(-1)
t.printTrail()
t.clearTrail()

t = Turtle(19,0)
for j in range (6):
    t.move(8)
    t.turn(-1)
t.printTrail()
t.clearTrail()

t = Turtle(5,5)
for j in range (3):
    t.move(13)
    t.turn(-1)
    t.move(1)
    t.turn(-1)
t.printTrail()
t.clearTrail()

t = Turtle(15,5)
for j in range (3):
    t.move(12)
    t.turn(-2)
t.printTrail()
t.clearTrail()
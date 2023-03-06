from turtle import *
from cursor import printList

t = Turtle(0,0)
for i in range (6):
    t.move(9)
    t.turn(-1)
    t.printTrail()
    t.clearTrail()

for i in range (6):
    t.setPosition(135)
    t.clearTrail()
    t.setDirection(i)
    t.move(9)
    t.printTrail()

for i in range (6):
    start = 270 + i * 35
    for j in range(4):
        result = list(range(start + j * 9, start + (j+1) * 9 + 1 ))
        if (j==0):
            result[0] = [270,261,144,0,9,126][i]
        if (j==3):
            result[9] = [261,144,0,9,126,270][i]
        printList (result)
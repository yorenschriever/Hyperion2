from cursor import *
from turtle import *

starts = [0,9,126,270,261,144]


for (direction,start) in enumerate(starts):
    t = Turtle(start,0)
    t.turn(-direction)
    t.move(9)
    t.petalcw()
    t.printTrail()
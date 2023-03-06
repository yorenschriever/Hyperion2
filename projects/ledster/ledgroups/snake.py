from cursor import *
from turtle import *

# curs = Cursor(135)
# curs.left(9)
# curs.upright(9)
# curs.petalcw()
# curs.downright(9)
# curs.left(9)
# curs.downleft(9)
# curs.printTrail()

# curs = Cursor(162)
# curs.upright(1)
# curs.printTrail()

t = Turtle(135,0)
for i in range (6):
    t.move(9)
    t.turn(2)
    t.move(9)
    t.petalcw()
    t.turn(2)
    t.move(9)
    t.turn(2)
    t.move(9)
    t.turn(-1)
t.printTrail()
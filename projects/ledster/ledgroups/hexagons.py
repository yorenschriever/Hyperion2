from cursor import *

starts = [135, 116, 100, 81, 69, 50, 42, 23, 19, 0]

for i in range(1,10):
    curs = Cursor(starts[i])
    curs.left(i)
    curs.downleft(i)
    curs.downright(i)
    curs.right(i)
    curs.upright(i)
    curs.upleft(i-1)
    curs.printTrail()
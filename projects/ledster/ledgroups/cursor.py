from distutils.log import error

class Cursor:
    __turns = [9,20,32,45,59,74,90,107,125,144,162,179,195,210,224,237,249,260,270]
    position = 0
    trail=[]

    def __init__(self, position):
        self.position = position
        self.trail=[position]

    def left(self, distance):
        for i in range(distance):
            self.position = self.__leftof(self.position)
            self.trail.append(self.position)

    def right(self, distance):
        for i in range(distance):
            self.position = self.__rightof(self.position)
            self.trail.append(self.position)

    def upleft(self, distance):
        for i in range(distance):
            self.position = self.__upleftof(self.position)
            self.trail.append(self.position)

    def upright(self, distance):
        for i in range(distance):
            self.position = self.__uprightof(self.position)
            self.trail.append(self.position)

    def downleft(self, distance):
        for i in range(distance):
            self.position = self.__downleftof(self.position)
            self.trail.append(self.position)

    def downright(self, distance):
        for i in range(distance):
            self.position = self.__downrightof(self.position)
            self.trail.append(self.position)

    def petalcw(self):
        if self.position==0:
            self.trail.extend(range(375,340,-1))
            self.trail.append(144)
            self.position=144
        elif self.position==144:
            self.trail.extend(range(340,305,-1))
            self.trail.append(261)
            self.position=261
        elif self.position==261:
            self.trail.extend(range(305,270,-1))
            self.trail.append(270)
            self.position=270
        elif self.position==270:
            self.trail.extend(range(480,445,-1))
            self.trail.append(126)
            self.position=126
        elif self.position==126:
            self.trail.extend(range(445,410,-1))
            self.trail.append(9)
            self.position=9
        elif self.position==9:
            self.trail.extend(range(410,375,-1))
            self.trail.append(0)
            self.position=0
        else:
            error('not at corner')

    def petalccw(self):
        if self.position==0:
            self.trail.extend(range(376,411))
            self.trail.append(9)
            self.position=9
        elif self.position==9:
            self.trail.extend(range(411,446))
            self.trail.append(126)
            self.position=126
        elif self.position==126:
            self.trail.extend(range(446,481))
            self.trail.append(270)
            self.position=270
        elif self.position==270:
            self.trail.extend(range(271,306))
            self.trail.append(261)
            self.position=261
        elif self.position==261:
            self.trail.extend(range(306,341))
            self.trail.append(144)
            self.position=144
        elif self.position==144:
            self.trail.extend(range(341,375))
            self.trail.append(0)
            self.position=0
        else:
            error('not at corner')

    def printTrail(self):
        printList(self.trail)

    def clearTrail(self):
        self.trail=[self.position]

    def __getrow(self, pix):
        return next(i for [i,x] in enumerate(self.__turns) if pix <= x)

    def __leftmostpixel(self, row):
        if row % 2 == 0:
            return self.__turns[row]
        return self.__turns[row-1]+1

    def __rightmostpixel(self, row):
        if row == 0:
            return 0
        if row % 2 == 1:
            return self.__turns[row]
        return self.__turns[row-1]+1

    def __horizontalof(self, pix, direction):
        row = self.__getrow(pix)
        if (row % 2 == direction):
            if self.__turns[row]==pix:
                return -1
            return pix+1
        else:
            if self.__turns[row-1]==pix-1:
                return -1
            return pix-1

    def __leftof(self,pix):
        return self.__horizontalof(pix,0)

    def __rightof(self,pix): 
        return self.__horizontalof(pix,1)

    def __downleftof(self,pix):
        row = self.__getrow(pix)
        rowstart = self.__leftmostpixel(row)
        downrowstart = self.__leftmostpixel(row+1)
        offset = 0 if row < 9 else 1
        if row == 18:
            return -1
        if (offset == 1 and pix == rowstart):
            return -2
        if (row %2 == 0):
            return downrowstart + (rowstart - pix) - offset
        else:
            return downrowstart - (pix-rowstart) + offset

    def __downrightof(self,pix):
        row = self.__getrow(pix)
        rowstart = self.__leftmostpixel(row)
        rowend = self.__rightmostpixel(row)
        downrowstart = self.__leftmostpixel(row+1)
        offset = 1 if row < 9 else 0
        if row == 18:
            return -1
        if (offset == 0 and pix == rowend):
            return -2
        if (row %2 == 0):
            return downrowstart + (rowstart - pix) + offset
        else:
            return downrowstart - (pix-rowstart) - offset

    def __upleftof(self,pix):
        row = self.__getrow(pix)
        rowstart = self.__leftmostpixel(row)
        uprowstart = self.__leftmostpixel(row-1)
        offset = 1 if row < 10 else 0
        if row == 0:
            return -1
        if (offset == 1 and pix == rowstart):
            return -2
        if (row %2 == 0):
            return uprowstart + (rowstart - pix) - offset
        else:
            return uprowstart - (pix-rowstart) + offset

    def __uprightof(self,pix):
        row = self.__getrow(pix)
        rowstart = self.__leftmostpixel(row)
        uprowstart = self.__leftmostpixel(row-1)
        rowend = self.__rightmostpixel(row)
        offset = 0 if row < 10 else 1
        if row == 0:
            return -1
        if (offset == 0 and pix == rowend):
            return -2
        if (row %2 == 0):
            return uprowstart + (rowstart - pix) + offset
        else:
            return uprowstart - (pix-rowstart) - offset


def printList(lst):
    print ("{",', '.join(map(lambda i:str(i), lst)), "}, ")
#sizes are in mm
width = 715
height = 82+25+82+25+500+25+82
strokeWidth = 82

ledsPerMeter = 60
ledSpacing = 1000 / ledsPerMeter 

# the lists that we will put the positions in
leds = []
bulbs = []

#### F vertical

def fVertical():
    numBulbs = 10
    distance = 500-strokeWidth
    bulbSpacing = distance / (numBulbs-1)
    numLeds = round(distance / ledSpacing)+1
    xStart = 0 + strokeWidth + 25 + strokeWidth/2
    yStart = 0 + strokeWidth + 25 + strokeWidth/2
    for i in range(numBulbs):
        bulbs.append({'x': xStart, 'y': yStart + i*bulbSpacing})
    for i in range(numLeds):
        leds.append({'x': xStart, 'y': yStart + i*ledSpacing})

fVertical()

#### F Horizontal top

def fHorizontal(yStart):
    numBulbs = 2
    distance = 185 - strokeWidth
    # take into account the bulb of the vertical row when calculating the bulb spacing
    bulbSpacing = distance / (numBulbs -1 + 1)
    #same for the leds, but inversed
    numLeds = round(distance / ledSpacing) +1 - 1
    xStart = 0 + strokeWidth + 25 + strokeWidth/2
    for i in range(numBulbs):
        bulbs.append({'x': xStart + (i+1)*bulbSpacing, 'y': yStart})
    for i in range(numLeds):
        leds.append({'x': xStart + (i+1)*ledSpacing, 'y': yStart})

fHorizontalYStart = 0 + strokeWidth + 25 + strokeWidth/2
fHorizontal(fHorizontalYStart)

#### F Horizontal bottom

fHorizontal(fHorizontalYStart + 250 + strokeWidth)

#### M Horizontal

def mHorizontal():
    numBulbs = 5
    distance = 290-strokeWidth
    bulbSpacing = distance / (numBulbs - 1)
    numLeds = round(distance / ledSpacing) + 1 + 1
    xStart = 0 + strokeWidth + 25 + 185  + 25 + strokeWidth/2
    yStart = 0 + strokeWidth + 25 + strokeWidth + 25 + strokeWidth/2
    for i in range(numBulbs):
        bulbs.append({'x': xStart + i * bulbSpacing, 'y': yStart })
    for i in range(numLeds):
        leds.append({'x': xStart + i * ledSpacing, 'y': yStart })

mHorizontal()

#### M Vertical 1

def mVertical(xStart):
    numBulbs = 9
    distance = 500 - strokeWidth
    # take into account the bulb of the horizontal row when calculating the bulb spacing
    bulbSpacing = distance / (numBulbs - 1 + 1)
    #same for the leds, but inversed
    numLeds = round(distance / ledSpacing) +1 - 1
    yStart = 0 + strokeWidth + 25 + strokeWidth + 25 + strokeWidth/2
    for i in range(numBulbs):
        bulbs.append({'x': xStart , 'y': yStart + (i+1)*bulbSpacing})
    for i in range(numLeds):
        leds.append({'x': xStart , 'y': yStart + (i+1)*ledSpacing})

mVerticalXStart = 0 + strokeWidth + 25 + 185  + 25 + strokeWidth/2
mVertical(mVerticalXStart)

#### M Vertical 2

mVertical(mVerticalXStart + strokeWidth + 25)

#### M Vertical 3

mVertical(mVerticalXStart + (strokeWidth + 25) * 2)

#### Border

def border():
    distanceHorizontal = width -strokeWidth
    distanceVertical = height - strokeWidth
    ledsHorizontal = round(distanceHorizontal / ledSpacing) +1 -1
    ledsVertical = round(distanceVertical / ledSpacing) +1 -1
    for i in range(ledsHorizontal):
        leds.append({'x': strokeWidth/2 + i*ledSpacing , 'y': strokeWidth/2})
    for i in range(ledsVertical):
        leds.append({'x': width - strokeWidth/2 , 'y': strokeWidth/2 + i * ledSpacing})
    for i in range(ledsHorizontal):
        leds.append({'x': width - (strokeWidth/2 + i*ledSpacing) , 'y': height-strokeWidth/2})
    for i in range(ledsVertical):
        leds.append({'x': strokeWidth/2 , 'y': height- (strokeWidth/2 + i * ledSpacing)})

border()

# write to file

scale = 2./height
xOffset = -1. + (height-width)/2 * scale
yOffset = -1.

f = open("bulbMap.hpp", "w")
f.write("PixelMap bulbMap = {\n")
for point in bulbs:
    f.write("    {.x = " + str(point['x'] * scale + xOffset) + ", .y = " + str(point['y']*scale + yOffset) + "},\n")
f.write("};\n\n")
f.close()

f = open("ledMap.hpp", "w")
f.write("PixelMap ledMap = {\n")
for point in leds:
    f.write("    {.x = " + str(point['x'] * scale + xOffset) + ", .y = " + str(point['y']*scale + yOffset) + "},\n")
f.write("};\n\n")
f.close()
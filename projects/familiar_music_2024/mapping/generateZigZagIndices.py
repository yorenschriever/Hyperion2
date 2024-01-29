# ====== zigzag index map
# maps the indices to compensate the zigzag ledbar orientation

numBars = 2*4*6
numBarsPerRing = numBars/2
mappedIndices=[]
startIndex=0

def fillAsc(startIndex):
    mappedIndices.extend(range(startIndex, startIndex+60) )
    return startIndex + 60

def fillDesc(startIndex):
    mappedIndices.extend(range(startIndex+59,startIndex-1,-1) )
    return startIndex + 60

#bovenste ring
for i in range(int(numBarsPerRing/2)):
    startIndex = fillAsc(startIndex)
    startIndex = fillDesc(startIndex)    

#onderste ring
for i in range(int(numBarsPerRing/2)):
    startIndex = fillAsc(startIndex)
    startIndex = fillDesc(startIndex)    

print(len(mappedIndices))

f = open("zigzagIndices.hpp", "w")
f.write("#pragma once\n")
f.write("#include <stdint.h>\n")
f.write("uint16_t zigzagIndices[] = {\n")
for index in mappedIndices:
    f.write(str(index)+",\n")
f.write("};\n\n")
f.close()
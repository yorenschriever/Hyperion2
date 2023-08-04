import clickPoints from "./wing.json" assert { type: "json" };
import fs from "fs"

const minX = Math.min(...clickPoints.map(i => i.x))
const maxX = Math.max(...clickPoints.map(i => i.x))
const scale = (maxX - minX)*0.8

const minY = Math.min(...clickPoints.map(i => i.y))
const maxY = Math.max(...clickPoints.map(i => i.y))

const scaled = clickPoints.map(p => ({
    x: 1-((p.x - minX) / scale) + 0.4,
    y: (p.y - minY) / scale,
}))

//console.log({scaled})

let content = 'PixelMap wingMap = {\n'

for (let mirror = -1; mirror<2; mirror += 2)
{
    for (let i = 0; i < scaled.length / 2; i++) {
        let p1 = scaled[i];
        let p2 = scaled[clickPoints.length - 1 - i];
        
        if (i%2==0)
        {
            const tmp = p1;
            p1=p2;
            p2=tmp;
        }


        for (let j = 0; j < 60; j++) {
            const x = (p1.x + (p2.x - p1.x) * j/60) * mirror;
            const y = (p1.y + (p2.y - p1.y) * j/60) ;
            content += `    {.x = ${x}, .y = ${y}},\n`
        }
    }
}
content += ('};\n')

console.log (content);
try {
    fs.writeFileSync('wingMap.hpp', content);
    // file written successfully
  } catch (err) {
    console.error(err);
  }
import { createSocket } from '../common/useSocket.js'

export class Websocket {
    buffers;
    gl;
    buffer;

    constructor(gl, buffers, scene) {
        this.buffers = buffers;
        this.gl = gl;

        //TODO this is too big, but how big should it actually be? 
        this.buffer = new Uint8Array(buffers.verticesCount * 3)

        let ledOffset = 0;

        scene.forEach(scenePart => {
            const ledsInScenePart = scenePart.positions.length;
            
            const bufferOffset = ledOffset
            ledOffset += ledsInScenePart

            createSocket(scenePart.port, async data => {
                const ab = await data.arrayBuffer()
                const view = new Uint8Array(ab)

                const ledsReceived = ab.byteLength / 3;
                
                //TODO magic const, better calculate
                const indicesPerLed = 15;

                for (let i = 0; i < ledsReceived * indicesPerLed; i++) {
                    const ledIndex = Math.floor(i / indicesPerLed);
                    const bufferIndex = 3 * i + 3* indicesPerLed * bufferOffset
                    this.buffer[bufferIndex + 0] = view[3 * ledIndex + 0]
                    this.buffer[bufferIndex + 1] = view[3 * ledIndex + 1]
                    this.buffer[bufferIndex + 2] = view[3 * ledIndex + 2]
                }  
            }, false, 500, null, null)
        })

        window.requestAnimationFrame(() => this.writeBuffer());
    }

    writeBuffer() {
        this.gl.bindBuffer(this.gl.ARRAY_BUFFER, this.buffers.color);
        this.gl.bufferData(this.gl.ARRAY_BUFFER, this.buffer, this.gl.STATIC_DRAW);

        window.requestAnimationFrame(() => this.writeBuffer());
    }
}
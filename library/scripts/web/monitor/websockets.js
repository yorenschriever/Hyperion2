import { Socket } from '../common/socket.js'

export class Websockets {
    buffers;
    gl;
    buffer;

    runtimeSessionId=null;

    constructor(gl, buffers, scene) {
        this.buffers = buffers;
        this.gl = gl;

        //TODO this is too big, but how big should it actually be? 
        this.bytesbuffer = buffers.ledBuffers.map(b => new Uint8Array(b.verticesCount * 3));

        scene.forEach((scenePart,index) => {

            new Socket(scenePart.path, data => {
                if (!(data instanceof ArrayBuffer)){
                    const json = JSON.parse(data)
                    if (json.type=='runtimeSessionId')
                    {
                        const newBuildId = json.value
                        if (!this.runtimeSessionId) this.runtimeSessionId = newBuildId;
                        else if (this.runtimeSessionId != newBuildId) {
                            delete window.scene
                            window.onBuildIdChange?.();
                            window.location.reload();
                        }
                    }
                }
                const view = new Uint8Array(data)

                const ledsReceived = data.byteLength / 3;
                
                //TODO magic const, better calculate
                const indicesPerLed = 15;

                for (let i = 0; i < ledsReceived * indicesPerLed; i++) {
                    const ledIndex = Math.floor(i / indicesPerLed);
                    const bufferIndex = 3 * i 
                    this.bytesbuffer[index][bufferIndex + 0] = view[3 * ledIndex + 0]
                    this.bytesbuffer[index][bufferIndex + 1] = view[3 * ledIndex + 1]
                    this.bytesbuffer[index][bufferIndex + 2] = view[3 * ledIndex + 2]
                }  
            })
        })

        window.requestAnimationFrame(() => this.writeBuffer());
    }

    writeBuffer() {
        this.bytesbuffer.forEach((buf, index) => {
            this.gl.bindBuffer(this.gl.ARRAY_BUFFER, this.buffers.ledBuffers[index].color);
            this.gl.bufferData(this.gl.ARRAY_BUFFER, buf, this.gl.STATIC_DRAW);
        })
        window.requestAnimationFrame(() => this.writeBuffer());
    }
}
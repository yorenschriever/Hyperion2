export class Websocket {
    buffers;
    gl;
    buffer;
    sockets = []

    constructor(gl, buffers, scene) {
        this.buffers = buffers;

        //TODO this is too big, but how big should it actually be? 
        this.buffer = new Uint8Array(buffers.verticesCount * 3)

        let ledOffset = 0;

        scene.forEach(scenePart => {
            const ledsInScenePart = scenePart.positions.length;
            
            const bufferOffset = ledOffset
            ledOffset += ledsInScenePart

            const socket = new WebSocket(`wss://localhost:${scenePart.port}`);
            this.sockets.push(socket);
            socket.onmessage = async msg => {

                const ab = await msg.data.arrayBuffer()
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

                //TODO Only write to buffer every animationframe
                gl.bindBuffer(gl.ARRAY_BUFFER, this.buffers.color);
                gl.bufferData(gl.ARRAY_BUFFER, this.buffer, gl.STATIC_DRAW);
               
            }

        })
    }
}
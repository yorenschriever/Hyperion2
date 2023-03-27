export  class Websocket{
    buffers;
    gl;
    time=0;
    buffer;

    constructor(gl, buffers){
        this.buffers = buffers;
        const colors = Array(buffers.verticesCount).fill([0,0,0,255]).flat();
        this.buffer = new Uint8Array(colors)

        setInterval(()=>{

            this.time += 1;
            let h = this.hue(this.time % 255);

            for(let i=0;i<buffers.verticesCount;i++){
                this.buffer[4*i+0] = h[0]
                this.buffer[4*i+1] = h[1]
                this.buffer[4*i+2] = h[2]
            }
            
            gl.bindBuffer(gl.ARRAY_BUFFER, this.buffers.color);
            gl.bufferData(gl.ARRAY_BUFFER, this.buffer, gl.STATIC_DRAW);
        
        },1000/60);
    }


    hue(phase)
    {
        let WheelPos = 255 - phase;
        if (WheelPos < 85)
        {
            return [(255 - WheelPos * 3), 0, (WheelPos * 3)];
        }
        else if (WheelPos < 170)
        {
            WheelPos -= 85;
            return [0, (WheelPos * 3), (255 - WheelPos * 3)];
        }
        else
        {
            WheelPos -= 170;
            return [(WheelPos * 3), (255 - WheelPos * 3), 0];
        }
    }
}
import { html, useRef } from '/common/preact-standalone.js';
import { useSocket } from '/common/socket.js'

export const MonitorWindowsApp = () => {
    const canvasRefs = [useRef(null),useRef(null),useRef(null),useRef(null)];

    const handleData = (data) => {
        if (!(data instanceof ArrayBuffer))
            return;

        const data2 = new Uint8Array(data)

        const dataToPct = (d, i) =>(d[i+1]*256 + d[i])*100/(256*256-1);
        for(let i=0;i<4;i++){
            const canvas = canvasRefs[i].current;
            const ctx = canvas.getContext("2d");
            ctx.clearRect(0, 0, canvas.width, canvas.height);

            ctx.fillStyle = '#000';
            
            ctx.beginPath();
            ctx.rect(0, 0, canvas.width, dataToPct(data2,4*i));
            ctx.fill();

            ctx.beginPath();
            ctx.rect(0, canvas.height-dataToPct(data2,4*i+2), canvas.width, dataToPct(data2,4*i+2));
            ctx.fill();

        }
    };

    const socket = useSocket("/ws/windows", handleData);

    return html`<div class="windows">${
        canvasRefs.map(i => html`<div class="window">
            <canvas ref=${i}  width="100" height="100"></canvas>
        </div>`)
    }</div>`
}
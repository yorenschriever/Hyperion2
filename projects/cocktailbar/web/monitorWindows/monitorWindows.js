import { html } from '/common/preact-standalone.js';
import { useSocket } from '/common/socket.js'

export const MonitorWindowsApp = () => {
    const handleData = (data) => {
        if (!(data instanceof ArrayBuffer))
            return;

        const data2 = new Uint8Array(data)

        // console.log(data2);

        const dataToPct = (d, i) =>(d[i+1]*256 + d[i])*100/(256*256-1);
        // const dataToPct = (d, i) =>(d[i+1])*100/0x255;

        for(let i=0;i<4;i++){
            document.getElementById(`shutterTop${i}`   ).style.height = dataToPct(data2,4*i) + '%';
            document.getElementById(`shutterBottom${i}`).style.height = dataToPct(data2,4*i+2) + '%';
        }
    };

    const socket = useSocket("/ws/windows", handleData);

    return html`<div class="windows"><${Window} id=0/><${Window} id=1/><${Window} id=2/><${Window} id=3/></div>`
}

export const Window = ({id}) => {
    return html`<div class="window">
        <div class="shutterTop" id="shutterTop${id}"></div>
        <div class="shutterBottom" id="shutterBottom${id}"></div>
    </div>`
}

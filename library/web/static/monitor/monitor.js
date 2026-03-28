import { html, useState, useEffect, useRef} from '../common/preact-standalone.js'
import { main as initWebGLMonitor } from './webgl-monitor.js';
import { Socket } from '../common/socket.js'

export const MonitorApp = () => {

    useEffect(async () => {
        const pixelMaps = await fetch("/monitor/mapping.json")
        const pixelJson = await pixelMaps.json();

        const instanceParam = new URLSearchParams(document.location.search).get("instance")?.split(",");
        const filtered = 
            instanceParam ?
            pixelJson.filter(m => instanceParam.includes(m.instance)) :
            pixelJson;

        const groups = Object.groupBy(filtered, m => `${m.instance}-${m.type}`);
        setPixelMaps(groups);

    }, [])

    const [pixelMaps, setPixelMaps] = useState([]);

    return Object.entries(pixelMaps).map(([_groupName, maps]) => html`
        <div class="monitor">
            <${Monitor} scenes=${maps} />
            <span class="monitor-name">${maps[0].instance}</span>
        </div>`)
}

const Monitor = ({ scenes }) => {
    const canvasRef = useRef(null);
    useEffect(() => {
        if (!canvasRef.current) return;

        let runtimeSessionId;
        const createPixelSource = (scenePart, setBuffer) => {

            new Socket(scenePart.path, data => {
            if (!(data instanceof ArrayBuffer)){
                const json = JSON.parse(data)
                if (json.type=='runtimeSessionId')
                {
                    const newBuildId = json.value
                    if (!runtimeSessionId) runtimeSessionId = newBuildId;
                    else if (runtimeSessionId != newBuildId) {
                        delete window.scene
                        window.onBuildIdChange?.();
                        window.location.reload();
                    }
                }
                return;
            }
            setBuffer(new Uint8Array(data));
            })
        };

        initWebGLMonitor(scenes, canvasRef.current, createPixelSource);
    }, [scenes]);

    return html`<canvas ref=${canvasRef} width="640" height="480"></canvas>`;
}
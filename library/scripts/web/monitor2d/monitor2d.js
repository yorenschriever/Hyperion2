import { html, useRef, useEffect } from '../common/preact-standalone.js'
import { Socket } from '../common/socket.js'

export const Monitor2dApp = (props) => {
    const canvas = useRef(null);
    const sceneRef = useRef(null);
    const r = 0.01;

    const runtimeSessionId = useRef();

    useEffect(() => {

        const clearCanvas = (c, ctx) => {
            ctx.clearRect(0, 0, c.width, c.height);
            ctx.rect(0, 0, c.width, c.height);
            ctx.fillStyle = '#191919';
            ctx.fill();
        }

        const drawCircle = (c, ctx, x, y, r, color) => {
            const scale = c.height / 2;
            const centerX = c.width / 2 + x * scale;
            const centerY = c.height / 2 + y * scale;

            ctx.beginPath();
            ctx.arc(centerX, centerY, r * scale, 0, 2 * Math.PI, false);
            ctx.fillStyle = color;
            ctx.fill();
        }

        const drawCanvas = () => {

            const ctx = canvas.current.getContext("2d");

            clearCanvas(canvas.current, ctx);

            const scene = sceneRef.current;
            scene?.forEach(scenePart => {
                scenePart.positions.forEach((position, i) => {
                    const color = 
                        (scenePart.colors && (i+1)*3 <= scenePart.colors.length) ? 
                            `rgb(${scenePart.colors[i * 3]},${scenePart.colors[i * 3 + 1]},${scenePart.colors[i * 3 + 2]})` : 
                            `rgb(0,0,0)`
                    drawCircle(canvas.current, ctx, position.x, position.y, r, color)
                })
            })

            window.requestAnimationFrame(() => drawCanvas());
        }

        window.requestAnimationFrame(() => drawCanvas());

        window.onresize = () => {
            canvas.current.width  = window.innerWidth;
            canvas.current.height = window.innerHeight;
        }
        
        window.onresize()
        
        let createdSockets;

        const getScene = () => {
            if (window.scene)
                return Promise.resolve(window.scene)
            return fetch("./mapping.json").then(i=>i.json())
        }

        const handleData = (data, scenePart) => {
            if (data instanceof ArrayBuffer){
                scenePart.colors = new Uint8Array(data)
                return;
            }
            
            //if a new build id comes in, the mapping could have changed, reload everything
            const json = JSON.parse(data)
            if (json.type=='runtimeSessionId')
            {
                const newBuildId = json.value
                if (!runtimeSessionId.current) runtimeSessionId.current = newBuildId;
                else if (runtimeSessionId.current != newBuildId) {
                    window.onBuildIdChange?.();
                    window.location.reload();
                }
            }
        }

        getScene().then(scene => {
            sceneRef.current = scene;
            createdSockets = sceneRef.current.map(scenePart => new Socket(scenePart.path, data => handleData(data, scenePart)))
        });

        return () => createdSockets.forEach(socket => socket.close());
    }, [])

    return html`<canvas ref=${canvas} width="600" height="400"/>`
}


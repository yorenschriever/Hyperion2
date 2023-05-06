import { html, useRef, useEffect } from '../common/preact-standalone.js'
import { useMonitorSockets } from '../common/useSocket.js'
import scene from "./mapping.json" assert { type: "json" };

export const Monitor2dApp = (props) => {
    const canvas = useRef(null);
    const r = 0.01;

    useMonitorSockets(scene, async (scenePart, data) => scenePart.colors = new Uint8Array(await data.arrayBuffer()));

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

            scene.forEach(scenePart => {
                if (scenePart.colors) {
                    scenePart.positions.forEach((position, i) => {
                        const color = `rgb(${scenePart.colors[i * 3]},${scenePart.colors[i * 3 + 1]},${scenePart.colors[i * 3 + 2]})`
                        drawCircle(canvas.current, ctx, position.x, position.y, r, color)
                    })
                }
            })

            window.requestAnimationFrame(() => drawCanvas());
        }

        window.requestAnimationFrame(() => drawCanvas());

        window.onresize = () => {
            canvas.current.width  = window.innerWidth;
            canvas.current.height = window.innerHeight;
        }
        
        window.onresize()
    }, [])

    return html`<canvas ref=${canvas} width="600" height="400"/>`
}


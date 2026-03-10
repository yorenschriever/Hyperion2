import { html, useState, createContext, useContext, useCallback, useEffect, useRef } from '../common/preact-standalone.js'
import { useSocket } from '../common/socket.js'
import { AnalogGauge } from './gauge.js';

const stale = (lastUpdate) => Date.now() - lastUpdate > 2500;

const useAnalytics = () => {

    // Update the analytics every second, also when no data comes in.
    // Also we don't want to update on every message, to avoid too many re-renders.
    // So this will the only state. The incoming dat will be stored in a ref.
    const [time, setTime] = useState(Date.now());
    useEffect(() => {
        const interval = setInterval(() => setTime(Date.now()), 1000);
        return () => clearInterval(interval);
    }, []);

    const state = useRef({});
    
    const [socketState, setSocketState] = useState(WebSocket.CLOSED);

    const [send] = useSocket("/ws/analytics", msg => {
        msg = JSON.parse(msg);

        const current = state.current[msg.name] || {};
        const next = { ...current, name: msg.name };
        if (msg.source === "local") {
            next.numLights = msg.numLights;
            next.localFps = msg.fps;
            next.lastLocalUpdate = Date.now();
        }
        if (msg.source === "remote") {
            next.remoteFps = msg.fps;
            next.lastRemoteUpdate = Date.now();
        }

        state.current[msg.name] = next;
    }, setSocketState);

    const valid = Object.values(state.current).filter(analytics => !stale(analytics.lastLocalUpdate));
    let minFps = valid.reduce((min, analytics) => Math.min(min, analytics.localFps), Infinity);
    if (minFps === Infinity) minFps = 0;

    const totalLights = valid.reduce((sum, analytics) => sum + analytics.numLights, 0);

    return { state: state.current, minFps, totalLights, socketState };
}

export const AnalyticsIcon = () => {
    const { minFps, socketState } = useAnalytics();
    const className = `fps-icon ${socketState === WebSocket.OPEN ? 'connected' : 'disconnected'}`;
    return html`<${AnalogGauge} value="${minFps}" min="0" max="120" className="${className}" />`;
};

export const AnalyticsApp = () => {
    const { state, minFps, totalLights } = useAnalytics();

    return html`
        <div class="analytics">
        
        <${AnalogGauge} value="${minFps}" min="0" max="120" label="FPS" values="13" className="fps" />

        <p>Total lights: ${totalLights}</p>

        <table>
            <thead>
                <tr>
                    <th>Source</th>
                    <th>Number of Lights</th>
                    <th>FPS</th>
                    <th>FPS remote</th>
                </tr>
            </thead>
            <tbody>
                ${Object.values(state).map(analytics => html`
                    <tr key=${analytics.name}>
                        <td>${analytics.name}</td>
                        <td>${analytics.numLights}</td>
                        <td><${Fps} fps=${analytics.localFps} lastUpdate=${analytics.lastLocalUpdate} /></td>
                        <td><${Fps} fps=${analytics.remoteFps} lastUpdate=${analytics.lastRemoteUpdate} /></td>
                    </tr>
                `)}
            </tbody>
        </table>  
        </div>
    `;
}

const Fps = ({ fps, lastUpdate }) => {
    if (stale(lastUpdate))
         return html`<span className="fps-stale">.</span>`;

    return html`${fps}`;
}
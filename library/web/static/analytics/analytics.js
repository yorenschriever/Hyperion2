import { html, useState, createContext, useContext, useCallback, useEffect, useRef } from '../common/preact-standalone.js'
import { useSocket } from '../common/socket.js'
import {AnalogGauge} from './gauge.js';

const useAnalytics = () => {
    const [state, setState] = useState({
        analytics: {}
    });
    const [socketState, setSocketState] = useState(WebSocket.CLOSED);

    const [send] = useSocket("/ws/analytics", msg => {
        setState(prevState => {
            const analytics = JSON.parse(msg);
            return {
                ...prevState,
                analytics: {
                    ...prevState.analytics,
                    [analytics.name]: analytics
                }
            };
        });
    }, setSocketState, setState);

    let minFps = Object.values(state.analytics).reduce((min, analytics) => Math.min(min, analytics.fps), Infinity);   
    if (minFps === Infinity) minFps = 0;

    const totalLights = Object.values(state.analytics).reduce((sum, analytics) => sum + analytics.numLights, 0);

    return { state, minFps, totalLights, socketState };
}

export const AnalyticsIcon = () => {
    const { minFps, socketState } = useAnalytics();
    const className = `fps-icon ${socketState === WebSocket.OPEN ? 'connected' : 'disconnected'}`;
    return html`<${AnalogGauge} value="${minFps}" min="0" max="120" className="${className}" />`;
};

export const AnalyticsApp = () => {
    const { state, minFps, totalLights } = useAnalytics();

    const className = (ip) => ip!='' ? 'connected' : 'disconnected';

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
                </tr>
            </thead>
            <tbody>
                ${Object.values(state.analytics).map(analytics => html`
                    <tr class="${className(analytics.ip)}" key=${analytics.name}>
                        <td>${analytics.name}</td>
                        <td>${analytics.numLights}</td>
                        <td>${analytics.fps}</td>
                    </tr>
                `)}
            </tbody>
        </table>  
        </div>
    `;
}

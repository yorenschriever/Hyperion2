import { html, useState, useRef, useEffect, createContext, useContext, useCallback } from '/common/preact-standalone.js'
import { MidiBridge } from "/midi/midiBridge.js"

export const Toolbar = () => {

    return html`
        <${MidiBridgeButton}/>
        <${Button} icon=${MetronomeIcon}/>
        <${Button} icon=${GaugeIcon}/>

        <${WakeLockButton}/>
        <${FullscreenButton}/>
    `;
}

const ensureSafeEnvironment = () => {
    const loc = window.location;
    if (loc.protocol == "https:")
        return;
    
    alert('You need to have a secure connection to use this feature. You will switch now.')
    
    const port = window.location.port || 80;
    const newUrl = `https://${loc.hostname}:${port}${loc.pathname}`

    window.location.replace(newUrl);
}

const MidiBridgeButton = () => {
    const [state, setState] = useState(false);
    const midiBridge = useRef();

    const toggleState = () =>
        setState(st => {
            const newstate = !st;

            if (newstate) {
                ensureSafeEnvironment();
                if (["localhost","127.0.0.1"].includes(window.location.hostname)){
                    if (!confirm("Your browser is running on the same host as the hyperion core. Are you sure you need the midi bridge?"))
                        return false;
                }
                midiBridge.current = new MidiBridge();
            } else {
                midiBridge.current?.destroy();
                delete midiBridge.current;
            }

            return newstate;
        });

    let className = state ? "active" : "inactive"
    if (midiBridge.current?.error) className = "error";

    return html`<a onclick=${toggleState} class=${className}><${MidiIcon}/></a>`;
}

const WakeLockButton = () => {
    const [state, setState] = useState(false);
    const [error, setError] = useState(false);
    const wakeLock = useRef(null);
    const reapplyWakeLock = useRef(false);

    const setLock = async () => {
        ensureSafeEnvironment();
        try {
            console.log('set lock')
            wakeLock.current = await navigator.wakeLock.request("screen");
            setState(true);
            reapplyWakeLock.current=true;
            wakeLock.current.addEventListener("release", (evt) => {
                console.log('remove lock', evt)
                wakeLock.current = null;
                setState(false);
            });
        } catch (err) {
            console.error(err)
            setError(true);
        }
    }

    const removeLock = async () => {
        await wakeLock.current.release()
        reapplyWakeLock.current=false;
    }

    const toggleState = () => (wakeLock.current ? removeLock: setLock)();

    useEffect(() => {
        //re apply the wake lock if the window visibility changed
        document.addEventListener("visibilitychange", async () => {
            if (reapplyWakeLock.current && document.visibilityState === "visible") 
                setLock();       
        });
    }, [])

    let className = state ? "active" : "inactive"
    if (error) className = "error";

    return html`<a onclick=${toggleState} class=${className}><${CoffeeIcon}/></a>`;
}

const FullscreenButton = () => {
    const [state, setState] = useState(false);

    const toggleFullScreen = () => {
        if (!document.fullscreenElement) {
            document.documentElement.requestFullscreen();
            setState(true);
        } else if (document.exitFullscreen) {
            document.exitFullscreen();
            setState(false);
        }
    }

    return html`<a onclick=${toggleFullScreen} class=${state ? "active" : "inactive"}><${FullscreenIcon}/></a>`;
}

const Button = ({ icon, defaultState = false }) => {

    const [state, setState] = useState(defaultState);

    const toggleState = () => {
        console.log('set state')
        setState(st => !st);
    }

    return html`<a onclick=${toggleState} class="disabled"><${icon}/></a>`
}

const FullscreenIcon = () => html`<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 20 20">
    <path fill="currentcolor" fill-rule="evenodd" d="M1 1v6h2V3h4V1H1zm2 12H1v6h6v-2H3v-4zm14 4h-4v2h6v-6h-2v4zm0-16h-4v2h4v4h2V1h-2z"/>
</svg>`

const MidiIcon = () => html`<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1000 1000" >
    <g transform="translate(0.000000,502.000000) scale(0.100000,-0.100000)">
      <path fill="currentcolor" d="M 4561,4807.5 C 3633.8,4725.1 2741,4370.7 1972.8,3778.7 1599.2,3491.3 1116.5,2953 834.8,2510.5 714.1,2322.8 480.4,1841.9 392.3,1602.4 168.2,997.1 64.7,255.7 110.7,-388 c 24.9,-364 61.3,-580.5 147.5,-925.3 88.1,-341 174.3,-572.8 354.4,-936.8 252.9,-513.4 532.6,-904.2 940.6,-1306.6 542.2,-534.5 1107.3,-887 1856.4,-1155.2 l 320.9163,-71.1959 8.5137,1126.7764 c 0,0 1708.8459,-3.8104 2483.3798,11.752 l -4.174,-1129.1008 c 0,0 114.7642,-28.3317 448.1642,88.5683 285.5,101.5 775.9,346.7 1034.5,519.2 559.4,371.7 1040.3,856.4 1404.2,1408.1 153.3,233.7 402.3,731.8 501.9,1003.8 222.2,607.3 329.5,1358.3 281.6,1998.1 -90,1251 -607.3,2354.5 -1505.8,3218.5 -1017.1,979 -2413.7,1469.5 -3821.8,1344.9 z m 1019.2,-411.8 c 988.5,-116.9 1923.4,-588.1 2649.5,-1331.5 706.9,-728 1132.2,-1618.8 1252.9,-2632.3 32.6,-260.5 26.8,-804.6 -9.6,-1092 -203,-1570.8 -1229.9,-2931 -2685.9,-3555.6 -214.6,-92 -226.1,-95.8 -251,-61.3 l 9.0628,985.3203 -3126.1101,-17.3162 7.6473,-996.7041 -147.5,59.4 c -365.9,149.4 -768.2,385.1 -1101.6,651.4 C 1279.3,-2880.4 677.8,-1811.4 526.4,-660 490,-389.9 484.3,179.1 514.9,432 c 99.6,812.3 390.8,1546 871.7,2189.7 189.7,254.8 649.4,718.4 892.7,900.4 385.1,287.4 793.1,505.8 1212.7,653.3 697.4,243.2 1329.6,310.3 2088.2,220.3 z"/>
      <path fill="currentcolor" d="M4712.4,3771.1c-283.5-99.6-488.5-325.7-565.1-622.6C3992,2554.6,4520.8,1974.1,5130,2069.9c756.7,118.8,1011.5,1072.8,415.7,1557.5c-160.9,128.4-283.5,170.5-517.3,180.1C4865.6,3813.3,4817.7,3807.5,4712.4,3771.1z M5222,3376.5c90-46,193.5-155.2,228-241.4c82.4-199.2,42.1-388.9-111.1-538.3c-139.9-136-308.4-178.2-496.2-120.7c-86.2,24.9-233.7,147.5-274,226.1c-139.8,270.1-13.4,595.8,270.1,701.2C4938.4,3439.7,5122.3,3428.2,5222,3376.5z"/>
      <path fill="currentcolor" d="M2652.9,2918.6c-323.8-115-526.8-348.7-595.8-682c-84.3-398.5,153.3-829.5,534.5-975.1c241.4-92,482.8-80.4,716.5,34.5c178.2,86.2,310.4,220.3,402.3,406.1c63.2,126.4,72.8,162.8,78.5,319.9c17.3,385.1-168.6,697.3-507.7,858.3c-116.9,53.7-151.3,61.3-331.4,65.1C2794.7,2951.2,2727.6,2943.5,2652.9,2918.6z M3130,2520.1c302.7-145.6,371.7-549.8,136-787.4c-176.3-174.3-471.3-185.8-664.8-26.8c-38.3,32.6-92,101.5-118.8,157.1c-128.4,256.7-3.8,576.6,266.3,676.3C2848.3,2575.7,3034.2,2566.1,3130,2520.1z" />
      <path fill="currentcolor" d="M6810.1,2918.6c-147.5-53.6-258.6-126.4-367.8-239.5c-176.3-187.7-245.2-379.3-231.8-657.1c5.7-157.1,15.3-193.5,78.5-321.8c157.1-318,438.7-496.2,789.3-498.1c490.4-3.8,881.2,387,877.4,877.4c-1.9,352.5-180.1,632.2-500,789.3c-136,69-157.1,72.8-346.8,78.5C6951.9,2951.2,6884.9,2943.5,6810.1,2918.6z M7243.1,2539.3c429.1-147.5,429.1-772.1,0-919.6c-321.8-109.2-653.3,122.6-653.3,459.8C6589.8,2412.8,6923.2,2648.5,7243.1,2539.3z"/>
      <path fill="currentcolor" d="M1877.1,861.1c-141.8-26.8-327.6-130.3-442.5-245.2c-329.5-329.5-341-871.7-26.8-1206.9c402.3-431,1072.8-373.6,1390.8,116.9c99.6,155.2,139.8,316.1,128.3,526.8c-5.7,147.5-17.2,191.6-78.5,319.9C2672.1,742.3,2281.3,939.6,1877.1,861.1z M2269.8,433.9c160.9-74.7,256.7-212.7,272-398.5c19.2-193.5-90-385-270.1-480.8c-109.2-57.5-316.1-57.5-425.3,0c-367.8,195.4-367.8,701.2,1.9,885.1C1959.4,495.2,2145.3,493.3,2269.8,433.9z"/>
      <path fill="currentcolor" d="M7735.4,851.5C7061.1,688.7,6835-177.2,7344.6-646.6c293.1-270.1,733.7-312.3,1063.3-101.6c477,306.5,551.7,967.5,157.1,1364C8342.7,838.1,8034.3,924.3,7735.4,851.5z M8128.2,451.1C8358.1,355.3,8480.7,89,8406-156.2c-28.7-95.8-149.4-233.7-252.9-289.3c-109.2-57.5-316.1-57.5-425.3,0c-364,191.6-364,710.7,1.9,879.3C7842.7,487.5,8022.8,495.2,8128.2,451.1z"/>
    </g>
 </svg>`

const MetronomeIcon = () => html`<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 314 314">
    <path fill="currentcolor" d="M281.853,46.631c-0.39-2.035-1.66-3.794-3.47-4.802l-11.257-6.271l5.297-9.511c1.881-3.378,0.668-7.641-2.709-9.521
	c-3.38-1.881-7.641-0.667-9.521,2.709l-5.297,9.511l-11.257-6.269c-1.81-1.008-3.975-1.162-5.908-0.422
	c-1.936,0.74-3.442,2.3-4.117,4.259l-13.806,40.109c-1.103,3.207,0.25,6.743,3.213,8.394l4.741,2.642l-23.115,41.501L184.592,17.11
	C182.703,7.516,173.563,0,163.783,0h-56.052c-9.778,0-18.918,7.516-20.809,17.11L46.102,224.425
	c-0.002,0.012-0.004,0.023-0.006,0.035l-13.729,69.725c-0.998,5.067,0.205,10.132,3.3,13.895C38.76,311.842,43.496,314,48.66,314
	h174.195c5.163,0,9.898-2.158,12.993-5.92c3.095-3.763,4.298-8.827,3.301-13.896l-13.732-69.741
	c-0.001-0.002-0.001-0.004-0.001-0.004l-16.584-84.225l31.16-55.944l4.742,2.642c1.072,0.598,2.243,0.885,3.402,0.885
	c2.046,0,4.056-0.896,5.425-2.571l26.836-32.853C281.708,50.769,282.241,48.665,281.853,46.631z M100.659,19.816
	c0.6-3.044,3.971-5.816,7.072-5.816h56.052c3.103,0,6.473,2.771,7.072,5.815l23.249,118.069L164,191.933
	c-6.17-2.645-13.121-4.627-21.242-5.363v-14.92h10.96c3.866,0,7-3.134,7-7s-3.134-7-7-7h-10.96v-41.211h10.96c3.866,0,7-3.134,7-7
	c0-3.866-3.134-7-7-7h-10.96v-41.21h10.96c3.866,0,7-3.134,7-7s-3.134-7-7-7h-10.96V34.938c0-3.866-3.134-7-7-7
	c-3.866,0-7,3.134-7,7v12.291h-10.96c-3.866,0-7,3.134-7,7s3.134,7,7,7h10.96v41.21h-10.96c-3.866,0-7,3.134-7,7
	c0,3.866,3.134,7,7,7h10.96v41.211h-10.96c-3.866,0-7,3.134-7,7s3.134,7,7,7h10.96v14.92c-19.447,1.763-32.189,10.688-43.564,18.666
	c-7.894,5.536-14.975,10.493-23.52,12.573L100.659,19.816z M225.036,299.188c-0.432,0.523-1.206,0.813-2.181,0.813H48.66
	c-0.976,0-1.75-0.289-2.181-0.814c-0.432-0.524-0.565-1.34-0.377-2.297l12.675-64.372c14.147-1.593,24.455-8.807,34.455-15.82
	c12.059-8.457,23.448-16.444,42.525-16.444c19.077,0,30.466,7.987,42.524,16.444c10.001,7.014,20.308,14.228,34.456,15.82
	l12.675,64.371C225.602,297.846,225.468,298.662,225.036,299.188z M209.842,217.809c-8.545-2.08-15.627-7.037-23.521-12.574
	c-3.175-2.227-6.456-4.527-9.959-6.726l21.928-39.367L209.842,217.809z M246.427,71.829l-11.474-6.392l9.177-26.662l20.136,11.216
	L246.427,71.829z"/>
</svg>`

const GaugeIcon = () => html`<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 122.88 106.3">
    <path fill="currentcolor" d="M58.15,52.98l28.6-18.22c0.2-0.15,0.48-0.12,0.65,0.06l2.76,2.94c0.17,0.18,0.18,0.47,0.02,0.66L68.51,63.6 c-3.08,3.31-6.37,3.96-9.02,3.1c-1.32-0.43-2.47-1.22-3.35-2.25c-0.88-1.02-1.49-2.27-1.74-3.61c-0.49-2.67,0.49-5.66,3.73-7.85 L58.15,52.98L58.15,52.98z M19.33,106.17c-3.05-2.87-5.8-6.05-8.21-9.48c-2.39-3.4-4.44-7.06-6.11-10.91 C3.38,82,2.12,78.02,1.26,73.88C0.44,69.86,0,65.7,0,61.44c0-8.32,1.66-16.25,4.65-23.49C7.77,30.43,12.33,23.66,18,18 c5.66-5.66,12.43-10.23,19.95-13.34C45.19,1.66,53.12,0,61.44,0c8.3,0,16.21,1.66,23.43,4.66c7.52,3.12,14.28,7.7,19.95,13.37 c5.68,5.68,10.26,12.46,13.38,19.97c3.01,7.24,4.68,15.16,4.68,23.44c0,4.05-0.4,8.01-1.16,11.85c-0.78,3.94-1.95,7.75-3.46,11.4 c-1.54,3.71-3.43,7.25-5.64,10.55c-2.23,3.34-4.78,6.45-7.6,9.3c-0.19,0.19-0.51,0.19-0.7,0l-3.07-3.06 c-0.06-0.02-0.12-0.06-0.17-0.11l-8.56-8.56c-0.19-0.19-0.19-0.51,0-0.7l4.49-4.49c0.19-0.19,0.51-0.19,0.7,0l6.61,6.61 c1.4-1.82,2.69-3.72,3.85-5.7c1.25-2.12,2.35-4.34,3.3-6.63c1.28-3.1,2.29-6.35,2.97-9.71c0.64-3.12,1-6.35,1.07-9.64h-9.11 c-0.27,0-0.5-0.22-0.5-0.5V55.7c0-0.27,0.22-0.5,0.5-0.5h8.76c-0.68-5.85-2.31-11.43-4.72-16.58c-2.49-5.31-5.82-10.15-9.82-14.37 l-5.86,5.86c-0.19,0.19-0.51,0.19-0.7,0l-4.49-4.49c-0.19-0.19-0.19-0.51,0-0.7l5.65-5.65c-4.44-3.57-9.45-6.46-14.87-8.5 C75.1,8.8,69.47,7.62,63.6,7.39v8.03c0,0.27-0.22,0.5-0.5,0.5h-6.36c-0.27,0-0.5-0.22-0.5-0.5V7.59 c-5.83,0.55-11.4,2.04-16.54,4.29c-5.31,2.33-10.17,5.49-14.42,9.3l5.87,5.87c0.19,0.19,0.19,0.51,0,0.7l-4.49,4.49 c-0.19,0.19-0.51,0.19-0.7,0l-5.8-5.8c-3.73,4.4-6.78,9.41-8.96,14.86C9.1,46.6,7.79,52.29,7.44,58.23h9.03 c0.27,0,0.5,0.22,0.5,0.5v6.36c0,0.27-0.22,0.5-0.5,0.5H7.5c0.22,2.94,0.68,5.8,1.35,8.58c0.72,3.01,1.7,5.92,2.91,8.72 c1.05,2.43,2.27,4.76,3.64,6.98c1.29,2.09,2.72,4.09,4.28,5.97l7.24-7.24c0.19-0.19,0.51-0.19,0.7,0l4.49,4.49 c0.19,0.19,0.19,0.51,0,0.7c-4.14,4.14-8.09,8.11-12.09,12.36C19.84,106.35,19.53,106.36,19.33,106.17L19.33,106.17z"/>
</svg>`

const CoffeeIcon = () => html`<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1024 1024">
    <path fill="currentcolor" d="M390.1 398.1l0.4-0.5c1.3 1.7 2.7 3.3 4.2 4.9 13 12.2 33.4 11.6 45.6-1.4s11.6-33.4-1.4-45.6c-1.7-1.5-3.3-2.8-5.1-4l0.4-0.5c-25.9-24.4-27.4-65.3-3-91.3 48.8-52 46.2-133.7-5.8-182.5l-0.4 0.5c-1.3-1.7-2.7-3.3-4.2-4.9-13-12.2-33.4-11.6-45.6 1.4s-11.6 33.4 1.4 45.6c1.7 1.5 3.3 2.8 5.1 4l-0.5 0.5c26.1 24.4 27.4 65.3 3 91.3-48.7 52-46.1 133.7 5.9 182.5zM854.3 482.1c-28.6 0-55.2 8.5-77.5 23.1v-36c0-14.3-11.6-25.8-25.8-25.8h-12.8c-2.3 0-4.4 0.4-6.4 0.9-2.1-0.5-4.2-0.9-6.4-0.9H79.5c-2.2 0-4.4 0.4-6.4 0.9-2.1-0.5-4.2-0.9-6.4-0.9H53.5c-14.3 0-25.8 11.6-25.8 25.8v154.9c0 3.2 0.6 6.3 1.8 9.2C47.1 822.7 207.2 960 402.3 960c162 0 299.7-94.7 351.8-235.1 25.7 25.6 61 41.4 100.1 41.4 78.5 0 142.1-63.6 142.1-142.1 0.1-78.5-63.5-142.1-142-142.1zM712.4 611.4h-0.4c-6.6 164.8-142.6 284.1-309.6 284.1S99.3 776.2 92.8 611.4h-0.3V508.1h619.9v103.3z m141.9 90.3c-42.8 0-77.5-34.7-77.5-77.5s34.7-77.5 77.5-77.5 77.5 34.7 77.5 77.5-34.7 77.5-77.5 77.5zM558 398.1l0.4-0.5c1.3 1.7 2.7 3.3 4.2 4.9 13 12.2 33.4 11.6 45.6-1.4s11.6-33.4-1.4-45.6c-1.7-1.5-3.3-2.8-5.1-4l0.4-0.5c-25.9-24.4-27.4-65.3-3-91.3 48.8-52 46.2-133.7-5.8-182.5l-0.4 0.5c-1.3-1.7-2.7-3.3-4.2-4.9-13-12.2-33.4-11.6-45.6 1.4s-11.6 33.4 1.4 45.6c1.7 1.5 3.3 2.8 5.1 4l-0.5 0.5c26.1 24.4 27.4 65.3 3 91.3-48.8 52-46.1 133.7 5.9 182.5zM209.2 398.1l0.4-0.5c1.3 1.7 2.7 3.3 4.2 4.9 13 12.2 33.4 11.6 45.6-1.4s11.6-33.4-1.4-45.6c-1.7-1.5-3.3-2.8-5.1-4l0.4-0.5c-25.9-24.4-27.4-65.3-3-91.3 48.8-52 46.2-133.7-5.8-182.5l-0.4 0.5c-1.3-1.7-2.7-3.3-4.2-4.9-13-12.2-33.4-11.6-45.6 1.4s-11.6 33.4 1.4 45.6c1.7 1.5 3.3 2.8 5.1 4l-0.5 0.5c26.1 24.4 27.4 65.3 3 91.3-48.7 52-46.1 133.7 5.9 182.5z"/>
</svg>`
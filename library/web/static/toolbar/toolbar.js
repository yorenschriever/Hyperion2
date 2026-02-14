import { html, useState, useRef, useEffect, createContext, useContext, useCallback } from '/common/preact-standalone.js'
import { MidiBridge } from "/midi/midiBridge.js"

export const Toolbar = () => {

    return html`
        <${MidiBridgeButton}/>
        <${WakeLockButton}/>
        <${MonitorButton}/> 
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

                //this must be called in response to a user action, so we can't ask for permission in the MidiBridge constructor
                navigator.requestMIDIAccess().then( () =>  midiBridge.current = new MidiBridge());
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

const MonitorButton = () => {
    const [state, setState] = useState(true);

    const handleMonitorToggle = () => { 
        window.postMessage({type: "toggleMonitor", state: !state}, "*");
        setState(st => !st);
    }

    return html`<a onclick=${handleMonitorToggle} class=${state ? "active" : "inactive"}><${MonitorIcon}/></a>`;
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

const CoffeeIcon = () => html`<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1024 1024">
    <path fill="currentcolor" d="M390.1 398.1l0.4-0.5c1.3 1.7 2.7 3.3 4.2 4.9 13 12.2 33.4 11.6 45.6-1.4s11.6-33.4-1.4-45.6c-1.7-1.5-3.3-2.8-5.1-4l0.4-0.5c-25.9-24.4-27.4-65.3-3-91.3 48.8-52 46.2-133.7-5.8-182.5l-0.4 0.5c-1.3-1.7-2.7-3.3-4.2-4.9-13-12.2-33.4-11.6-45.6 1.4s-11.6 33.4 1.4 45.6c1.7 1.5 3.3 2.8 5.1 4l-0.5 0.5c26.1 24.4 27.4 65.3 3 91.3-48.7 52-46.1 133.7 5.9 182.5zM854.3 482.1c-28.6 0-55.2 8.5-77.5 23.1v-36c0-14.3-11.6-25.8-25.8-25.8h-12.8c-2.3 0-4.4 0.4-6.4 0.9-2.1-0.5-4.2-0.9-6.4-0.9H79.5c-2.2 0-4.4 0.4-6.4 0.9-2.1-0.5-4.2-0.9-6.4-0.9H53.5c-14.3 0-25.8 11.6-25.8 25.8v154.9c0 3.2 0.6 6.3 1.8 9.2C47.1 822.7 207.2 960 402.3 960c162 0 299.7-94.7 351.8-235.1 25.7 25.6 61 41.4 100.1 41.4 78.5 0 142.1-63.6 142.1-142.1 0.1-78.5-63.5-142.1-142-142.1zM712.4 611.4h-0.4c-6.6 164.8-142.6 284.1-309.6 284.1S99.3 776.2 92.8 611.4h-0.3V508.1h619.9v103.3z m141.9 90.3c-42.8 0-77.5-34.7-77.5-77.5s34.7-77.5 77.5-77.5 77.5 34.7 77.5 77.5-34.7 77.5-77.5 77.5zM558 398.1l0.4-0.5c1.3 1.7 2.7 3.3 4.2 4.9 13 12.2 33.4 11.6 45.6-1.4s11.6-33.4-1.4-45.6c-1.7-1.5-3.3-2.8-5.1-4l0.4-0.5c-25.9-24.4-27.4-65.3-3-91.3 48.8-52 46.2-133.7-5.8-182.5l-0.4 0.5c-1.3-1.7-2.7-3.3-4.2-4.9-13-12.2-33.4-11.6-45.6 1.4s-11.6 33.4 1.4 45.6c1.7 1.5 3.3 2.8 5.1 4l-0.5 0.5c26.1 24.4 27.4 65.3 3 91.3-48.8 52-46.1 133.7 5.9 182.5zM209.2 398.1l0.4-0.5c1.3 1.7 2.7 3.3 4.2 4.9 13 12.2 33.4 11.6 45.6-1.4s11.6-33.4-1.4-45.6c-1.7-1.5-3.3-2.8-5.1-4l0.4-0.5c-25.9-24.4-27.4-65.3-3-91.3 48.8-52 46.2-133.7-5.8-182.5l-0.4 0.5c-1.3-1.7-2.7-3.3-4.2-4.9-13-12.2-33.4-11.6-45.6 1.4s-11.6 33.4 1.4 45.6c1.7 1.5 3.3 2.8 5.1 4l-0.5 0.5c26.1 24.4 27.4 65.3 3 91.3-48.7 52-46.1 133.7 5.9 182.5z"/>
</svg>`

const MonitorIcon = () => html`
<svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="2 2 20 20">
  <path fill="currentcolor" d="M6.5 21v-1.825L7.675 18H3.5c-0.4 0 -0.75 -0.15 -1.05 -0.45 -0.3 -0.3 -0.45 -0.65 -0.45 -1.05V4.5c0 -0.4 0.15 -0.75 0.45 -1.05C2.75 3.15 3.1 3 3.5 3h17c0.4 0 0.75 0.15 1.05 0.45 0.3 0.3 0.45 0.65 0.45 1.05v12c0 0.4 -0.15 0.75 -0.45 1.05 -0.3 0.3 -0.65 0.45 -1.05 0.45h-4.2l1.2 1.175V21H6.5Zm-3 -4.5h17V4.5H3.5v12Z" stroke-width="0.5"></path>
</svg>`

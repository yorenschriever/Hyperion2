import { html, useState, createContext, useContext, useCallback , useEffect} from '../common/preact-standalone.js'
import { useSocket } from '../common/socket.js'

const set = (obj, path, value) => {
    if (!path || path == "") return value;

    const steps = path.split(".");
    const step = steps[0];
    const remainingSteps = steps.slice(1).join(".");

    if (Array.isArray(obj)) {
        const index = parseInt(step);
        const result = [...obj];
        result[index] = set(obj[index], remainingSteps, value);
        return result;
    } else if (typeof (obj) === "object") {
        const result = { ...obj };
        result[step] = set(obj[step], remainingSteps, value);
        return result;
    } else {
        console.error('unknown obj', obj, typeof (obj))
    }
}

const defaultColumn = {
    slots: [],
    dim: 255,
    name: ""
}

const defaultSlot = {
    active: false,
    name: ""
}

const initialState = {
    buildId: null,
    columns: [],
    masterDim: 255,
    params: { }
}

const SendMessage = createContext();

export const ControllerApp = () => {
    const [state, setState] = useState(initialState);
    const [socketState, setSocketState] = useState(WebSocket.CLOSED)

    const resizeController = useCallback((columnIndex, slotIndex) => {
        setState(state => {
            if (columnIndex === undefined || state.columns.length > columnIndex)
                return state;

            return set(state, `columns.${columnIndex}`, defaultColumn)
        })
        setState(state => {
            if (slotIndex === undefined || state.columns[columnIndex].slots.length > slotIndex)
                return state;

            return set(state, `columns.${columnIndex}.slots.${slotIndex}`, defaultSlot)
        })
    },[setState])

    const [send] = useSocket("/ws/controller", msg => {
        msg = JSON.parse(msg)
        resizeController(msg.columnIndex, msg.slotIndex)

        if (msg.type == "onHubSlotActiveChange") {
            setState(state => set(state, `columns.${msg.columnIndex}.slots.${msg.slotIndex}.active`, Boolean(msg.active)))
        } else if (msg.type == "onHubSlotNameChange") {
            setState(state => set(state, `columns.${msg.columnIndex}.slots.${msg.slotIndex}.name`, msg.name))
        } else if (msg.type == "onHubColumnDimChange") {
            setState(state => set(state, `columns.${msg.columnIndex}.dim`, msg.dim))
        } else if (msg.type == "onHubMasterDimChange") {
            setState(state => set(state, `masterDim`, msg.masterDim))
        } else if (msg.type == "onHubParamChange") {
            setState(state => set(state, `params.${msg.param}`, msg.value))
        } else if (msg.type == "onHubColumnNameChange") {
            setState(state => set(state, `columns.${msg.columnIndex}.name`, msg.name))
        } else if (msg.type == "buildId") {
            setState(state => {
                if (!state.buildId)
                    //no build id was present yet, store it
                    return {...state,buildId: msg.value}
                
                if (msg.value == state.buildId)
                    //build is unchanged, continue with the state we had
                    return state;
                
                //build id is changed. try to call onBuildIdChange(). This function is present if we are in an iframe
                //and it will reload the entire iframe container. This will trigger a re-evaluation whether
                //we should show a 2d or 3d monitor.
                //if we are not in an iframe, it is enough to reset out internal state. that is wat the next line does
                window.onBuildIdChange?.();
                return {...initialState, buildId: msg.value}
            })
        }
    }, setSocketState)

    return html`
    <${SendMessage.Provider} value=${send}>
        <${SocketState} socketState=${socketState}/>
        <${Controller} state=${state} />
        <${Tempo}/>
    </${SendMessage.Provider}>`;
}

const SocketState = ({ socketState }) => {
    if (socketState == WebSocket.OPEN) return
    return html`<div class="controllerState">Reconnecting ...</div>`
}

const Controller = ({ state }) => {
    return html`
    <div class="controller">

        <div class="params columns">
            ${Object.entries(state.params).map(([name, value]) => html`<${ParamFader} name=${name} value=${value}/>`)}
        </div>

        <div class="activation columns">
            ${state.columns.map((column, columnIndex) => html`<${Column} column=${column} columnIndex=${columnIndex}/>`)}
            <div class="column">
                <div class="columnHeader">Master</div>
                <${MasterDimFader} value=${state.masterDim}/> 
            </div> 
        </div>
    

    </div>
`;
}

const Column = ({ column, columnIndex }) => html`
    <div class="column">
        <div class="columnHeader">${column.name}</div>
        <${DimFader} column=${column} columnIndex=${columnIndex}/>   
        <div class="slots">
            ${column.slots.map((slot, slotIndex) => html`<${Slot} slot=${slot} columnIndex=${columnIndex} slotIndex=${slotIndex}/>`)}
        </div>
    </div>
    `;

const Slot = ({ slot, columnIndex, slotIndex }) => {
    const sender = useContext(SendMessage);

    const handlePressed = () => {
        sender(`{"type":"buttonPressed", "columnIndex":${columnIndex}, "slotIndex": ${slotIndex}}`)
    }

    const handleReleased = () => {
        sender(`{"type":"buttonReleased", "columnIndex":${columnIndex}, "slotIndex": ${slotIndex}}`)
    }

    return html`
    <div 
        class="slot ${slot.active ? "active" : ""}" 
        onmousedown=${handlePressed}
        onmouseup=${handleReleased}
        ontouchstart=${(event) => { handlePressed(); event.preventDefault(); }}
        ontouchend=${(event) => { handleReleased(); event.preventDefault(); }}
        >
        ${slot.name}
    </div>`;
}

const DimFader = ({ column, columnIndex }) => {
    const sender = useContext(SendMessage);

    const handleChange = (e) => {
        sender(`{"type":"columnDimChange", "columnIndex":${columnIndex}, "dim": ${e.target.value}}`)
    }

    return html`
        <input type="range" min="0" max="255" value=${column.dim} class="fader" oninput=${handleChange}/>
    `
}

const MasterDimFader = ({ value }) => {
    const sender = useContext(SendMessage);

    const handleChange = (e) => {
        sender(`{"type":"masterDimChange", "masterDim": ${e.target.value}}`)
    }

    return html`
        <input type="range" min="0" max="255" value=${value} class="fader" oninput=${handleChange}/>
    `
}

const ParamFader = ({ name, value }) => {
    const sender = useContext(SendMessage);

    const handleChange = (e) => {
        sender(`{"type":"paramChange", \"param\": \"${name}\", "value": ${e.target.value}}`)
    }

    return html`
        <div class="faderContainer">
            <div class="columnHeader">${name}</div>
            <input type="range" min="0" max="255" value=${value} class="fader param" oninput=${handleChange}/>
        </div>
    `
}

const Tempo = () => {
    const [source, setSource] = useState("[none]");
    useSocket("/ws/tempo", msg => {
        msg = JSON.parse(msg)
        //console.log(msg);

        const element = document.getElementById("tempo"); 
        element.classList.remove("beat"); 
        element.classList.remove("beat1"); 
        
        // trigger a DOM reflow 
        void element.offsetWidth; 
        
        element.classList.add((msg.beatNr % 4==0)?"beat1":"beat");
        setSource(msg.sourceName);
    });

    return html`<div class="tempo" id="tempo">Tempo source: ${source}</div>`
}
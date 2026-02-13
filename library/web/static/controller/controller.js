import { html, useState, createContext, useContext, useCallback , useEffect} from '../common/preact-standalone.js'
import { useSocket } from '../common/socket.js'
import { useLongPress} from "./useLongPress.js"
import { set } from '../common/set.js';

const defaultColumn = {
    slots: [],
    dim: 255,
    name: ""
}

const defaultSlot = {
    active: 0,
    name: ""
}

const defaultParams = {
    params: {},
    name: ""
};

const initialState = {
    runtimeSessionId: null,
    columns: [],
    masterDim: 255,
    paramsSet: []
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

    const resizeParams = useCallback((paramsSlotIndex) => {
        setState(state => {
            if (paramsSlotIndex === undefined || state.paramsSet.length > paramsSlotIndex)
                return state;

            const newstate = set(state, `paramsSet.${paramsSlotIndex}`, defaultParams)
            console.log({newstate})
            return newstate
        })

    },[setState])

    const [send] = useSocket("/ws/controller", msg => {
        msg = JSON.parse(msg)
        resizeController(msg.columnIndex, msg.slotIndex)
        resizeParams(msg.paramsSlotIndex)

        if (msg.type == "onHubSlotActiveChange") {
            setState(state => set(state, `columns.${msg.columnIndex}.slots.${msg.slotIndex}.active`, msg.active))
        } else if (msg.type == "onHubSlotNameChange") {
            setState(state => set(state, `columns.${msg.columnIndex}.slots.${msg.slotIndex}.name`, msg.name))
        } else if (msg.type == "onHubColumnDimChange") {
            setState(state => set(state, `columns.${msg.columnIndex}.dim`, msg.dim))
        } else if (msg.type == "onHubMasterDimChange") {
            setState(state => set(state, `masterDim`, msg.masterDim))
        } else if (msg.type == "onHubColumnNameChange") {
            setState(state => set(state, `columns.${msg.columnIndex}.name`, msg.name))
        } else if (msg.type == "onHubParamChange") {
            setState(state => set(state, `paramsSet.${msg.paramsSlotIndex}.params.${msg.param}`, msg.value))
        } else if (msg.type == "onHubParamsNameChange") {
            setState(state => set(state, `paramsSet.${msg.paramsSlotIndex}.name`, msg.name))
        } else if (msg.type == "runtimeSessionId") {
            setState(state => {
                if (!state.runtimeSessionId)
                    //no runtimeSessionId was present yet, store it
                    return {...state,runtimeSessionId: msg.value}
                
                if (msg.value == state.runtimeSessionId)
                    //runtimeSessionId is unchanged, continue with the state we had
                    return state;
                
                //runtimeSessionId is changed. try to call onBuildIdChange(). This function is present if we are in an iframe
                //and it will reload the entire iframe container. This will trigger a re-evaluation whether
                //we should show a 2d or 3d monitor.
                //if we are not in an iframe, it is enough to reset out internal state. that is wat the next line does
                window.onBuildIdChange?.();
                return {...initialState, runtimeSessionId: msg.value}
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

        <div class="paramsSet">
            ${state.paramsSet.map((params, paramsSlotIndex) => html`<${Params} params=${params} paramsSlotIndex=${paramsSlotIndex}/>`)}
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

const Params = ({ params, paramsSlotIndex }) => html`
    <div class="paramsHeader">${params.name}</div>
    <div class="params columns">
        ${Object.entries(params.params).map(([name, value]) => html`<${ParamFader} name=${name} value=${value} paramsSlotIndex=${paramsSlotIndex}/>`)}
    </div>
    `;

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

    const handleSequencerPressed = (event) => {
        event.stopPropagation();
        window.postMessage({
            type: "addSequence",
            columnIndex,
            slotIndex
        }, "*");
        window.setTimeout(() => {
            window.scrollTo({ top: 0, behavior: 'smooth' });
        }, 100);
    }

    let className = "slot";
    if (slot.active & 1) className += " active";
    if (slot.active & 2) className += " active-flash";
    if (slot.active & 4) className += " active-sequence";

    return html`
    <div 
        class="${className}" 
        onmousedown=${handlePressed}
        onmouseup=${handleReleased}
        ontouchstart=${(event) => { handlePressed(); event.preventDefault(); }}
        ontouchend=${(event) => { handleReleased(); event.preventDefault(); }}
        >
        ${slot.name}
        <button class="sequencer" onmousedown=${handleSequencerPressed}></button>
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

const ParamFader = ({ name, value, paramsSlotIndex }) => {
    const sender = useContext(SendMessage);

    const handleChange = (e) => {
        sender(`{"type":"paramChange", "paramsSlotIndex":${paramsSlotIndex}, "param": "${name}", "value": ${e.target.value}}`)
    }

    return html`
        <div class="faderContainer">
            <div class="columnHeader">${name}</div>
            <input type="range" min="0" max="255" value=${value} class="fader param" oninput=${handleChange}/>
        </div>
    `
}

const Tempo = () => {
    const [source, setSource] = useState("None");
    const [send] = useSocket("/ws/tempo", msg => {
        msg = JSON.parse(msg)
        //console.log(msg);

        const element = document.getElementById("tempo"); 
        element.classList.remove("beat"); 
        element.classList.remove("beat1"); 
        
        // trigger a DOM reflow 
        void element.offsetWidth; 
        
        if (msg.beatNr !== -1)
            element.classList.add((msg.beatNr % 4==0)?"beat1":"beat");
        setSource(msg.sourceName);
    });

    const onLongPress = () => {
        send(`{"type":"stop"}`)
    };

    const onClick = () => {
        send(`{"type":"tap"}`)
    }

    const longPressEvent = useLongPress(onLongPress, onClick, {delay:500});

    return html`<div class="tempo" id="tempo" ...${longPressEvent}>Tempo source: ${source}</div>`
}
import { html, useState, useEffect, createContext, useRef, useContext } from './preact-standalone.js'

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
    dim:255,
    name: "default column name"
}

const defaultSlot = {
    active: false,
    name: "default slot name"
}

const initialState = {
    columns: [],
    masterDim:255
}

const SendMessage = createContext();

export const ControllerApp = (props) => {
    const [state, setState] = useState(initialState);
    const [socketState, setSocketState] = useState(WebSocket.CLOSED);
    const socketRef = useRef();

    useEffect(() => {

        const resizeController = (columnIndex, slotIndex) => {
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
        }

        const createSocket = () => {
            const socket = new WebSocket(`wss://${location.host}:9800`);
            setSocketState(socket.readyState);

            socket.onmessage = wsmsg => {
                const msg = JSON.parse(wsmsg.data);

                resizeController(msg.columnIndex, msg.slotIndex)

                if (msg.type == "onHubSlotActiveChange") {
                    setState(state => set(state, `columns.${msg.columnIndex}.slots.${msg.slotIndex}.active`, Boolean(msg.active)))
                } else if (msg.type == "onHubSlotNameChange") {
                    setState(state => set(state, `columns.${msg.columnIndex}.slots.${msg.slotIndex}.name`, msg.name))
                } else if (msg.type=="onHubColumnDimChange"){
                    setState(state => set(state, `columns.${msg.columnIndex}.dim`, msg.dim))
                } else if (msg.type == "onHubMasterDimChange"){
                    setState(state => set(state, `masterDim`, msg.dim))
                }
            }

            socket.onclose = (e) => {
                setSocketState(socket.readyState);
                console.log('Socket is closed. Reconnect will be attempted in 1 second.', e.reason);
                setTimeout(() => {
                    createSocket()
                }, 1000);
            };

            socket.onerror = (err) => {
                setSocketState(socket.readyState);
                console.error('Socket encountered error: ', err.message, 'Closing socket');
                socket.close();
            };

            socket.onopen = () => {
                setSocketState(socket.readyState);
            }

            socketRef.current = socket;

        }

        createSocket();
        return () => socketRef.current.close();
    }, [])

    const send = (msg) => {
        //console.log('sending', msg)
        socketRef.current.send(msg)
    }

    return html`
    <${SendMessage.Provider} value=${send}>
        <${SocketState} socketState=${socketState}/>
        <${Controller} state=${state} />
    </${SendMessage.Provider}>`;
}

const SocketState = ({ socketState }) => {
    if (socketState == WebSocket.OPEN) return
    return html`<div class="controllerState">Reconnecting ...</div>`
}

const Controller = ({ state }) => {
    return html`
    <div class="controller">
        ${state.columns.map((column, columnIndex) => html`<${Column} column=${column} columnIndex=${columnIndex}/>`)}
        <${MasterDimFader} value=${state.masterDim}/>   
    </div>`;
}

const Column = ({ column, columnIndex }) => html`
    <div class="column">
    <${DimFader} column=${column} columnIndex=${columnIndex}/>    
    ${column.slots.map((slot, slotIndex) => html`<${Slot} slot=${slot} columnIndex=${columnIndex} slotIndex=${slotIndex}/>`)}
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

const DimFader = ({column, columnIndex}) => {
    const sender = useContext(SendMessage);

    const handleChange = (e) => {
        sender(`{"type":"columnDimChange", "columnIndex":${columnIndex}, "dim": ${e.target.value}}`)
    }

    return html`
        <input type="range" min="1" max="255" value=${column.dim} class="fader" oninput=${handleChange}/>
    `
}

const MasterDimFader = ({value}) => {
    const sender = useContext(SendMessage);

    const handleChange = (e) => {
        sender(`{"type":"masterDimChange", "masterDim": ${e.target.value}}`)
    }

    return html`
        <input type="range" min="1" max="255" value=${value} class="fader" oninput=${handleChange}/>
    `
}

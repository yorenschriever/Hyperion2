import { html, useState, createContext, useContext, useCallback, useEffect, useRef } from '../common/preact-standalone.js'
import { useSocket } from '../common/socket.js'

const stepCount = 64; //TODO

const initialState = {
    stepNr: 0,
    sequences: []
}

export const SequencerApp = () => {
    const [state, setState] = useState(initialState);
    const [socketState, setSocketState] = useState(WebSocket.CLOSED);

    const [send] = useSocket("/ws/sequencer", msg => {
        msg = JSON.parse(msg)

        if (msg.type == "stepNr") 
            setState(state => ({...state, stepNr: msg.stepNr}))
        if (msg.type == "details"){
            setState(state => ({
                ...state,
                sequences: [...state.sequences.filter(s => s.index !== msg.index), {
                    index: msg.index,
                    slotName: msg.slotName,
                    colName: msg.colName,
                    steps: Array(stepCount).fill(false)
                }]
            }))
        }
        if (msg.type == "remove") {
            setState(state => ({
                ...state,
                sequences: state.sequences.filter(s => s.index !== msg.index)
            }))
        }
        if (msg.type == "status") {
            setState(state => ({
                ...state,
                sequences: state.sequences.map(s => s.index === msg.index ? 
                    {...s, steps: msg.steps.split("").map(s => s === '1')} : 
                    s)
            }))
       }
        
        // } else if (msg.type == "runtimeSessionId") {
        //     setState(state => {
        //         if (!state.runtimeSessionId)
        //             //no runtimeSessionId was present yet, store it
        //             return {...state,runtimeSessionId: msg.value}
                
        //         if (msg.value == state.runtimeSessionId)
        //             //runtimeSessionId is unchanged, continue with the state we had
        //             return state;
                
        //         //runtimeSessionId is changed. try to call onBuildIdChange(). This function is present if we are in an iframe
        //         //and it will reload the entire iframe container. This will trigger a re-evaluation whether
        //         //we should show a 2d or 3d monitor.
        //         //if we are not in an iframe, it is enough to reset out internal state. that is wat the next line does
        //         window.onBuildIdChange?.();
        //         return {...initialState, runtimeSessionId: msg.value}
        //     })
        // }
    }, setSocketState);

    useEffect(() => {
        const listener = (event) => {
            if (event.data?.type === "addSequence") {
                console.log("Adding sequence", event.data);
                send(JSON.stringify({
                    type: "add",
                    columnIndex: event.data.columnIndex,
                    slotIndex: event.data.slotIndex
                }));
            }
        };
        window.addEventListener("message", listener);
        return () => {
            window.removeEventListener("message", listener);
        }
    }, [])

    if (!state?.sequences.length)
         return undefined;

    return html`
    <div class="sequencer">
        ${state.sequences.map((sequence) => html`<${SequenceTrack} sequence=${sequence} send=${send} stepNr=${state.stepNr}/> `)}    
    </div>
    `;
}

const SequenceTrack = ({ sequence, send, stepNr }) => {
    const [draggedSteps, setDraggedSteps] = useState(Array(stepCount).fill(false));
    const [dragAction, setDragAction] = useState('select'); // 'select' or 'deselect'

    const trackRef = useRef(null);

    const handleDrag = useCallback((startX, endX) => {
        const minX = Math.min(startX, endX);
        const maxX = Math.max(startX, endX);

        const highlightedSteps = Array(stepCount).fill(false);
        for (let i = 0; i < stepCount; i++) {
            const stepRef = trackRef.current.children[3].children[i];
            const stepRect = stepRef.getBoundingClientRect();
            if (stepRect.right >= minX && stepRect.left <= maxX) {
                highlightedSteps[i] = true;
            }
        }

        setDraggedSteps(highlightedSteps);
    }, []);

    const handleDragEnd = useCallback((startX, endX, dragStepAction) => {
        const minX = Math.min(startX, endX);
        const maxX = Math.max(startX, endX);

        let startStep=0, endStep=0;
        trackRef.current.children[3].childNodes.forEach((stepRef, stepIndex) => {
            const stepRect = stepRef.getBoundingClientRect();

            //TODO sometimes the first rect is not selected
            if (stepRect.right >= minX && stepRect.left <= maxX) {
                if (startStep === 0) startStep = stepIndex;
                endStep = stepIndex;
            }
        });

        send(JSON.stringify({
            type: "setSteps",
            sequenceIndex:sequence.index,
            startStep,
            endStep,
            active: dragStepAction?0:1
        }))

        setDraggedSteps(Array(stepCount).fill(false));
    }, []);

    useEffect(() => {
        let startX=0, endX=0, dragging=false, dragStepAction=false;

        const handleMouseOver = (e) => {
            if (!dragging) return;
            endX = e.clientX;
            handleDrag(startX, endX);
        };

        const handleMouseDown = (e) => {
            dragging = true;
            startX = e.clientX;
            endX = startX;
            dragStepAction = e.target.dataset.active === 'true'; 
            setDragAction(dragStepAction ? 'deselect' : 'select');
        };

        const handleMouseUp = () => {
            if (!dragging) return;
            dragging = false;
            handleDragEnd(startX, endX, dragStepAction);
        };

        trackRef.current.addEventListener('mousedown', handleMouseDown);
        document.addEventListener('mouseup', handleMouseUp);
        document.addEventListener('mousemove', handleMouseOver);

        trackRef.current.addEventListener('touchstart', handleMouseDown);
        document.addEventListener('touchend', handleMouseUp);
        document.addEventListener('touchmove', handleMouseOver);
        return () => {
            trackRef.current.removeEventListener('mousedown', handleMouseDown);
            document.removeEventListener('mouseup', handleMouseUp);
            document.removeEventListener('mousemove', handleMouseOver);
            
            trackRef.current.removeEventListener('touchstart', handleMouseDown);
            document.removeEventListener('touchend', handleMouseUp);
            document.removeEventListener('touchmove', handleMouseOver);
        };
    }, []);

    const removeTrack = () => {
        send(JSON.stringify({
            type: "remove",
            index: sequence.index
        }))
    }

    return html`
    <div 
        class="track"
        ref=${trackRef}
    >
        <button class="remove-track" onClick=${removeTrack}>x</button>
        <div class="track-name">${sequence.colName}</div>    
        <div class="track-name">${sequence.slotName}</div>
        <div class="steps" data-drag-action=${dragAction}>
            ${sequence.steps.map((step, stepIndex) => html`
                <div 
                    class="step" 
                    data-current=${stepNr === stepIndex}
                    data-active=${step}
                    data-dragging=${draggedSteps[stepIndex]}
                ></div>
            `)}
        </div>
    </div>
    `;
}
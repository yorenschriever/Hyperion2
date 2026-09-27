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
                    enabled: msg.enabled,
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
                    {...s, enabled: msg.enabled, steps: msg.steps.split("").map(s => s === '1')} : 
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

    // toIndex/position describe where the row was hovered *before* removal, moveSequence resolves the final splice index
    const moveSequence = useCallback((fromIndex, toIndex, position) => {
        if (fromIndex === toIndex) return;
        setState(state => {
            const sequences = [...state.sequences];
            const [moved] = sequences.splice(fromIndex, 1);
            const shiftedTarget = toIndex > fromIndex ? toIndex - 1 : toIndex;
            const insertAt = position === 'after' ? shiftedTarget + 1 : shiftedTarget;
            sequences.splice(insertAt, 0, moved);
            return { ...state, sequences };
        });
    }, []);

    const [dragInfo, setDragInfo] = useState({ from: null, over: null, position: null });

    const handleRowDragStart = useCallback((index) => {
        setDragInfo({ from: index, over: null, position: null });
    }, []);

    const handleRowDragOver = useCallback((index, position) => {
        setDragInfo(info => (info.from === null || info.from === index) ? info : { ...info, over: index, position });
    }, []);

    const handleRowDragEnd = useCallback(() => {
        setDragInfo({ from: null, over: null, position: null });
    }, []);

    if (!state?.sequences.length)
         return undefined;

    return html`
    <div class="sequencer">
        ${state.sequences.map((sequence, index) => html`<${SequenceTrack} 
            key=${sequence.index} 
            index=${index} 
            sequence=${sequence} 
            send=${send} 
            stepNr=${state.stepNr} 
            moveSequence=${moveSequence}
            dragInfo=${dragInfo}
            onRowDragStart=${handleRowDragStart}
            onRowDragOver=${handleRowDragOver}
            onRowDragEnd=${handleRowDragEnd}
        /> `)}    
    </div>
    `;
}

const SequenceTrack = ({ sequence, send, stepNr, index, moveSequence, dragInfo, onRowDragStart, onRowDragOver, onRowDragEnd }) => {
    const [draggedSteps, setDraggedSteps] = useState(Array(stepCount).fill(false));
    const [dragAction, setDragAction] = useState('select'); // 'select' or 'deselect'

    const trackRef = useRef(null);
    const stepsRef = useRef(null);

    const handleDrag = useCallback((startX, endX) => {
        const minX = Math.min(startX, endX);
        const maxX = Math.max(startX, endX);

        const highlightedSteps = Array(stepCount).fill(false);
        for (let i = 0; i < stepCount; i++) {
            const stepRef = stepsRef.current.children[i];
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

        let startStep=-1, endStep=-1;
        stepsRef.current.childNodes.forEach((stepRef, stepIndex) => {
            const stepRect = stepRef.getBoundingClientRect();

            if (stepRect.right >= minX && stepRect.left <= maxX) {
                if (startStep === -1) startStep = stepIndex;
                endStep = stepIndex;
            }
        });

        if (startStep === -1 || endStep === -1) return;

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

        const getClientX = (e) => {
            return e.touches ? e.touches[0].clientX : e.clientX;
        };

        const handleMouseOver = (e) => {
            if (!dragging) return;
            endX = getClientX(e);
            handleDrag(startX, endX);
        };

        const handleMouseDown = (e) => {
            dragging = true;
            startX = getClientX(e);
            endX = startX;
            dragStepAction = e.target.dataset.active === 'true'; 
            setDragAction(dragStepAction ? 'deselect' : 'select');
        };

        const handleMouseUp = () => {
            if (!dragging) return;
            dragging = false;
            handleDragEnd(startX, endX, dragStepAction);
        };

        stepsRef.current.addEventListener('mousedown', handleMouseDown);
        document.addEventListener('mouseup', handleMouseUp);
        document.addEventListener('mousemove', handleMouseOver);

        stepsRef.current.addEventListener('touchstart', handleMouseDown);
        document.addEventListener('touchend', handleMouseUp);
        document.addEventListener('touchmove', handleMouseOver);
        return () => {
            stepsRef.current.removeEventListener('mousedown', handleMouseDown);
            document.removeEventListener('mouseup', handleMouseUp);
            document.removeEventListener('mousemove', handleMouseOver);
            
            stepsRef.current.removeEventListener('touchstart', handleMouseDown);
            document.removeEventListener('touchend', handleMouseUp);
            document.removeEventListener('touchmove', handleMouseOver);
        };
    }, []);

    // native drag-and-drop is only armed while the handle is held down, so the rest of the row stays interactive
    const [armed, setArmed] = useState(false);

    const handleHandleMouseDown = useCallback(() => {
        setArmed(true);
    }, []);

    const handleHandleMouseUp = useCallback(() => {
        setArmed(false);
    }, []);

    const handleDragStart = useCallback((e) => {
        e.dataTransfer.effectAllowed = 'move';
        e.dataTransfer.setData('text/plain', String(index));
        onRowDragStart(index);
    }, [index, onRowDragStart]);

    const handleTrackDragOver = useCallback((e) => {
        e.preventDefault();
        e.dataTransfer.dropEffect = 'move';
        const rect = trackRef.current.getBoundingClientRect();
        const position = (e.clientY - rect.top) < rect.height / 2 ? 'before' : 'after';
        onRowDragOver(index, position);
    }, [index, onRowDragOver]);

    const handleTrackDrop = useCallback((e) => {
        e.preventDefault();
        if (dragInfo.from !== null) moveSequence(dragInfo.from, index, dragInfo.position);
        onRowDragEnd();
    }, [index, dragInfo, moveSequence, onRowDragEnd]);

    const handleDragEndNative = useCallback(() => {
        setArmed(false);
        onRowDragEnd();
    }, [onRowDragEnd]);

    const isDragging = dragInfo.from === index;
    const dropPosition = (dragInfo.over === index && dragInfo.from !== index) ? dragInfo.position : null;

    const removeTrack = () => {
        send(JSON.stringify({
            type: "remove",
            index: sequence.index
        }))
    }

    const handleToggle = useCallback(() => {
        send(JSON.stringify({
            type: "enable",
            index: sequence.index,
            value: !sequence.enabled
        }))
    }, [sequence.index, sequence.enabled]);

    const hotKeys="1234567890QWERTYUIOPASDFGHJKLZXCVBNM";;

    useEffect(() => {
        const callback = (event) => (event.key == hotKeys[index].toLocaleLowerCase()) && handleToggle();
        window.addEventListener('keypress', callback);
        return () => {
            window.removeEventListener('keypress', callback);
        };  
    }, [handleToggle, index]);

    return html`
    <div 
        class="track"
        ref=${trackRef}
        draggable=${armed}
        data-dragging=${isDragging}
        data-drop-position=${dropPosition}
        onDragStart=${handleDragStart}
        onDragOver=${handleTrackDragOver}
        onDrop=${handleTrackDrop}
        onDragEnd=${handleDragEndNative}
    >
        <div 
            class="drag-handle" 
            onMouseDown=${handleHandleMouseDown}
            onMouseUp=${handleHandleMouseUp}
            onTouchStart=${handleHandleMouseDown}
            onTouchEnd=${handleHandleMouseUp}
        >⠿</div>
        <button class="remove-track" onClick=${removeTrack}>x</button>
        <div class="track-name">${sequence.colName}</div>    
        <div class="track-name">${sequence.slotName}</div>
        <${Toggle} onClick=${handleToggle} checked=${sequence.enabled} label="${hotKeys[index]}"/>
        <div class="steps ${sequence.enabled ? 'enabled' : 'disabled'}" data-drag-action=${dragAction} ref=${stepsRef}>
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

const Toggle = ({ onClick, checked, label }) => {
    return html`<label class="switch">
            <input type="checkbox" onClick=${onClick} checked=${checked}/>
            <span class="slider round"></span>
            <span class="toggle-label">${label}</span>
    </label>`;
}
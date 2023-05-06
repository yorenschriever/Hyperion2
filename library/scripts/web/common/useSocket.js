
import { useState, useEffect, useRef } from './preact-standalone.js'

const createSocket = (port, onMessage, json, backoff, setSocketState, setSocketRef) => {
    const socket = new WebSocket(`wss://${location.hostname}:${port}`);
    setSocketState?.(socket.readyState);

    socket.onmessage = wsmsg => {
        backoff = 500;
        if (json) {
            const msg = JSON.parse(wsmsg.data);
            onMessage(msg);
        } else {
            onMessage(wsmsg.data)
        }
    }

    socket.onclose = (e) => {
        console.log(`Socket is closed. Reconnect will be attempted in ${backoff} milliseconds.`, e.reason);
        setTimeout(() => {
            createSocket(port, onMessage,json, Math.min(backoff * 1.5,15000),setSocketState,setSocketRef)
        }, backoff);
    };
    
    socket.onerror = (err) => {
        setSocketState?.(socket.readyState);
        console.error('Socket encountered error: ', err.message, 'Closing socket');
        socket.close();
    };

    socket.onopen = () => {
        setSocketState?.(socket.readyState);
    }

    setSocketRef?.(socket);

}

export const useSocket = (port, onMessage, json=true) => 
{
    const [socketState, setSocketState] = useState(WebSocket.CLOSED);
    const socketRef = useRef();

    useEffect(() => {
        createSocket(port, onMessage, json, 500, setSocketState, newSocketRef => socketRef.current = newSocketRef);
        return () => socketRef.current.close();
    }, [])

    return [
        socketState,
        (msg) => {
            //console.log('sending', msg)
            socketRef.current.send(msg)
        }
    ]
}

export const useMonitorSockets = (scene, onMessage) => 
{
    useEffect(() => {
        const createdSockets = []
        scene.forEach(scenePart => {
            createdSockets.push(
                createSocket(
                    scenePart.port, 
                    data => onMessage(scenePart, data), 
                    false, 
                    500, 
                    null,
                    null));
        });
        
        return () => createdSockets.forEach(socket => socketRef.current.close());
    }, [])
}
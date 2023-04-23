
import { useState, useEffect, useRef } from './preact-standalone.js'

export const useSocket = (port, onMessage) => 
{
    const [socketState, setSocketState] = useState(WebSocket.CLOSED);
    const socketRef = useRef();

    useEffect(() => {
        let backoff = 500
        const createSocket = () => {
            const socket = new WebSocket(`wss://${location.host}:${port}`);
            setSocketState(socket.readyState);

            socket.onmessage = wsmsg => {
                backoff = 500;
                const msg = JSON.parse(wsmsg.data);
                onMessage(msg);
            }

            socket.onclose = (e) => {
                backoff = Math.min(backoff * 1.5,15000)
                console.log(`Socket is closed. Reconnect will be attempted in ${backoff} milliseconds.`, e.reason);
                setTimeout(() => {
                    createSocket()
                }, backoff);
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

    return [
        socketState,
        (msg) => {
            //console.log('sending', msg)
            socketRef.current.send(msg)
        }
    ]
}
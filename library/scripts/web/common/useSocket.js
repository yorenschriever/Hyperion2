
import { useEffect, useRef } from './preact-standalone.js'

class Socket {

    socket;
    port;
    onMessage;
    onStatusChange;

    reconnectInterval;
    visibilityCallback;

    constructor(port, onMessage, onStatusChange)
    {
        this.port = port;
        this.onMessage = onMessage;
        this.onStatusChange = onStatusChange;
        
        //this.reconnectInterval = window.setInterval(() => this.fixSocket(), 2500);
        this.visibilityCallback = this.onVisibilityChange.bind(this);
        document.addEventListener("visibilitychange", this.visibilityCallback);
        this.fixSocket();
    }

    fixSocket() {
        if (this.socket && this.socket.readyState != WebSocket.CLOSED)
            return; //everything is fine

        if (document.hidden)
            return; //don't create new socket when page is not in view

        this.socket = new WebSocket(`wss://${location.hostname}:${this.port}`)

        this.socket.onmessage = wsmsg => this.onMessage(wsmsg.data);

        this.socket.onclose = (e) => {
            this.onStatusChange?.(this.socket.readyState);
            console.log(`Socket is closed. `); //Reconnect will be attempted in ${backoff} milliseconds.`, e.reason);
        };
        
        this.socket.onerror = (err) => {
            console.error('Socket encountered error: ', err, 'Closing socket');
            this.onStatusChange?.(this.socket.readyState);
            socket.close();

            //if you enable this, also cancel it in the close() method
            //window.setTimeout(() => this.fixSocket(), 500);
        };
    
        this.socket.onopen = () => {
            console.log(`socket is opened on port ${this.port}`)
            this.onStatusChange?.(this.socket.readyState);
        }
    }

    onVisibilityChange() {
        const oldState = document.hidden;
        //the event fires before the state is updated.
        const willBeVisible = !oldState;

        if (willBeVisible){
            console.log('Document becoming visible. Creating socket');
            window.setTimeout(() => this.fixSocket(), 1);
        } else {
            console.log('Document hidden. Closing socket');
            this.socket?.close();
        }
    }

    close()
    {
        document.removeEventListener("visibilitychange", this.visibilityCallback);
        window.clearInterval(this.reconnectInterval);
        this.socket?.close();
    }

    send(msg)
    {
        this.socket?.send(msg)
    }

}

export const createSocket = (port, onMessage) => {
    new Socket(port, onMessage)
}

export const useSocket = (port, onMessage, setSocketState) => 
{
    const socketRef = useRef();

    useEffect(() => {
        socketRef.current = new Socket(port, onMessage, setSocketState);
        return () => socketRef.current.close();
    }, []);

    return [
        (msg) => socketRef.current?.send(msg)
    ]
}

export const useMonitorSockets = (scene, onMessage) => 
{
    useEffect(() => {
        const createdSockets = scene.map(scenePart => new Socket(scenePart.port, data => onMessage(scenePart, data)));
        return () => createdSockets.forEach(socket => socket.close());
    }, [])
}
import { useCallback, useRef, useState } from '../common/preact-standalone.js';

export const useLongPress = (
    onLongPress,
    onClick,
    { delay = 300 } = {}
) => {
    const [longPressTriggered, setLongPressTriggered] = useState(false);
    const timeout = useRef();

    const start = useCallback(
        event => {
            event.preventDefault();
            timeout.current = setTimeout(() => {
                onLongPress(event);
                setLongPressTriggered(true);
            }, delay);
        },
        [onLongPress, delay]
    );

    const clear = useCallback(
        (event, shouldTriggerClick = true) => {
            timeout.current && clearTimeout(timeout.current);
            shouldTriggerClick && !longPressTriggered && onClick();
            setLongPressTriggered(false);
            event.preventDefault();
        },
        [onClick, longPressTriggered]
    );

    return {
        onmousedown: e => start(e),
        ontouchstart: e => start(e),
        onmouseup: e => clear(e),
        ontouchend: e => clear(e)
    };
};

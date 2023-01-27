import { App } from '../mod.ts';

const port = 9001;

interface Counter {
    count: number
}

App().ws<Counter>('/*', {
    open(ws) {
        ws.count = 0;
        console.log('open', ws.count);
    },
    message(ws) {
        ws.count += 1;
        console.log('message', ws.count)
    },
    close(ws) {
        console.log('close', ws.count);
    },
}).listen(port, (token) => {
    if (token) {
        console.log('Listening to port ' + port);
        // const ws = new WebSocket("ws://127.0.0.1:9001");
        // ws.onopen = console.log;
        // ws.onerror = console.error;
        // ws.onclose = console.log;
        // console.log(ws.readyState, WebSocket.CONNECTING);
    } else {
        console.log('Failed to listen to port ' + port);
    }
});
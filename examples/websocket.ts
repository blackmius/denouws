import { App } from '../mod.ts';

const port = 9001;

interface Counter {
    count: number
}

App().ws<Counter>('*', {
    open(ws) {
        ws.count = 0;
        console.log('open', ws.count);
    },
    message(ws) {
        ws.count += 1;
        console.log('message', ws.count)
        console.log(ws.send('pong'));
    },
    close(ws) {
        console.log('close', ws.count);
    },
}).listen(port, (token) => {
    if (token) {
        console.log('Listening to port ' + port);
        // new WebSocket("ws://127.0.0.1:9001");
    } else {
        console.log('Failed to listen to port ' + port);
    }
});
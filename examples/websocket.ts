import { App } from '../mod.ts';

const port = 9001;

App().ws('/*', {
    open() {
        console.log('open')
    }
}).listen(port, (token) => {
    if (token) {
        console.log('Listening to port ' + port);
    } else {
        console.log('Failed to listen to port ' + port);
    }
});
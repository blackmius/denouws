import { App } from '../mod.ts';

const port = 9001;

App().any('/*', (res) => {
    res.end('Hello world from deno uws!');
}).listen(port, (token) => {
    if (token) {
        console.log('Listening to port ' + port);
    } else {
        console.log('Failed to listen to port ' + port);
    }
});
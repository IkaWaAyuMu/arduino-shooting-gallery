import * as http from "http";
import { connection, server } from "websocket";

const httpServer : http.Server = http.createServer((req, res) => {
    res.writeHead(404);
    res.end();
});
httpServer.listen(8000, () => console.log("Server initiated and is listening on port 8000"));

const wsServer : server = new server({ httpServer, autoAcceptConnections: false });

wsServer.on('request', (req) => {
    const connection: connection = req.accept('', req.origin);
    console.log((new Date()) + ': Connection accepted.');
    connection.on('message', function(message) {
        if (message.type === 'utf8') {
            console.log('Received Message: ' + message.utf8Data);
            connection.sendUTF(message.utf8Data);
        }
        else if (message.type === 'binary') {
            console.log('Received Binary Message of ' + message.binaryData.length + ' bytes');
            connection.sendBytes(message.binaryData);
        }
    });
    connection.on('close', function(reasonCode, description) {
        console.log((new Date()) + ' Peer ' + connection.remoteAddress + ' disconnected.');
    });
})
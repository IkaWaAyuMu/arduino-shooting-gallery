import * as http from "http";
import { connection, server } from "websocket";
// Game setup
let score: number = 0;

// server setup
function sleep(ms: number) { return new Promise((resolve) => { setTimeout(resolve, ms); }); }

const httpServer: http.Server = http.createServer((req, res) => {
    res.writeHead(404);
    res.end();
});
httpServer.listen(8000, () => console.log("Server initiated and is listening on port 8000"));

const wsServer: server = new server({ httpServer, autoAcceptConnections: false });

const targetClients: connection[] = [];
const gunClients: connection[] = [];
const webClients: connection[] = [];

wsServer.on('request', (req) => {
    const connection: connection = req.accept('', req.origin);
    console.log((new Date()) + `: Connection accepted from [ ${connection.remoteAddress} ].`);
    connection.sendUTF("TYPE");
    // Type specification
    connection.on('message', (message) => {
        if (message.type === "utf8" && message.utf8Data === "TARGET")  {
            console.log(`Add ${connection.remoteAddress} to target`);
            targetClients.push(connection);
        }
        else if (message.type === "utf8" && message.utf8Data === "GUN") {
            console.log(`Add ${connection.remoteAddress} to gun`);
            gunClients.push(connection);
        }
        else if (message.type === "utf8" && message.utf8Data === "WEB") {
            console.log(`Add ${connection.remoteAddress} to web`);
            webClients.push(connection);
        }
    });

    //Update from TARGET
    connection.on('message', (message) => {
        if (!targetClients.includes(connection)) return;
        console.log(`${new Date()} + Recieve target (${connection.remoteAddress}) message : ${(message.type === "utf8" ? message.utf8Data : "NOT_UTF8")}`)

        // Update score to web
        if (message.type === "utf8" && message.utf8Data === "SCORE") {
            score++
            webClients.forEach(client => {client.send(JSON.stringify({score}));});
        }
    })

    //Update from WEB
    connection.on('message', (message) => {
        if (!webClients.includes(connection)) return;
        console.log(`${new Date()} + Recieve web (${connection.remoteAddress}) message : ${(message.type === "utf8" ? message.utf8Data : "NOT_UTF8")}`)
        // Message sequence to target.
        if (message.type === "utf8" && message.utf8Data === "START") targetClients.forEach(client => { client.send("START"); });
        else if (message.type === "utf8" && message.utf8Data === "STOP") targetClients.forEach(client => { client.send("STOP"); });
    })
})

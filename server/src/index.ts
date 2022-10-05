import * as http from "http";
import { connection, server } from "websocket";
// Game setup
let score: number = 0;

// server setup
function sleep(ms: number) {
  return new Promise((resolve) => {
    setTimeout(resolve, ms);
  });
}

const httpServer: http.Server = http.createServer((req, res) => {
  res.writeHead(404);
  res.end();
});
httpServer.listen(8000, () =>
  console.log("Server initiated and is listening on port 8000")
);

const wsServer: server = new server({
  httpServer,
  autoAcceptConnections: false,
});

let targetClients: connection[] = [];
let gunClients: connection[] = [];
let webClients: connection[] = [];

wsServer.on("request", (req) => {
  const connection: connection = req.accept("", req.origin);
  console.log(
    new Date() + `: Connection accepted from [ ${connection.remoteAddress} ].`
  );
  connection.sendUTF("TYPE");
  // Type specification
  connection.on("message", (message) => {
    if (message.type === "utf8" && message.utf8Data === "TARGET") {
      console.log(`Add ${connection.remoteAddress} to target`);
      targetClients.push(connection);
      connection.sendUTF("TARGET ACCEPTED");
    } else if (message.type === "utf8" && message.utf8Data === "GUN") {
      console.log(`Add ${connection.remoteAddress} to gun`);
      gunClients.push(connection);
      connection.sendUTF("GUN ACCEPTED");
    } else if (message.type === "utf8" && message.utf8Data === "WEB") {
      console.log(`Add ${connection.remoteAddress} to web`);
      webClients.push(connection);
      connection.sendUTF("WEB ACCEPTED");
    }
  });

  //Update from TARGET
  connection.on("message", (message) => {
    if (!targetClients.includes(connection)) return;
    console.log(
      `${new Date()}: Recieve target (${connection.remoteAddress}) message : ${
        message.type === "utf8" ? message.utf8Data : "NOT_UTF8"
      }`
    );

    // Update score to web
    if (message.type === "utf8" && message.utf8Data === "SCORE") {
      score++;
      webClients.forEach((client) => {
        client.send(JSON.stringify({ score }));
      });
    }
  });

  //Update from WEB
  connection.on("message", (message) => {
    if (!webClients.includes(connection)) return;
    console.log(
      `${new Date()}: Recieve web (${connection.remoteAddress}) message : ${
        message.type === "utf8" ? message.utf8Data : "NOT_UTF8"
      }`
    );
    // Message sequence to target.
    if (message.type === "utf8" && message.utf8Data === "START")
      targetClients.forEach((client) => {
        client.send("START");
      });
    else if (message.type === "utf8" && message.utf8Data === "STOP")
      targetClients.forEach((client) => {
        client.send("STOP");
      });
  });

  connection.on("close", (reason, description) => {
    if (targetClients.includes(connection)) {
      targetClients = targetClients.filter((client: connection) => {
        client !== connection;
      });
      console.log(
        `${new Date()}: Target (${
          connection.remoteAddress
        }) closed the connection. ${reason}: ${description}`
      );
    } else if (gunClients.includes(connection)) {
      gunClients = gunClients.filter((client: connection) => {
        client !== connection;
      });
      console.log(
        `${new Date()}: Gun (${
          connection.remoteAddress
        }) closed the connection. ${reason}: ${description}`
      );
    } else if (webClients.includes(connection)) {
      webClients = webClients.filter((client: connection) => {
        client !== connection;
      });
      console.log(
        `${new Date()}: Web (${
          connection.remoteAddress
        }) closed the connection. ${reason}: ${description}`
      );
    } else
      console.log(
        `${new Date()}: Undefined client (${
          connection.remoteAddress
        }) closed the connection. ${reason}: ${description}`
      );
  });
});

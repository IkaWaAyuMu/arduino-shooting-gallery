# arduino-shooting-gallery

(This is part of [ENTANEER FRESTIVAL #1: CINEMANEER](https://www.facebook.com/cinemaneer65) booth)

This repository contains 4 part.

- [E-11](E-11/): Gun model. Thanks to mh319 for [models](https://www.thingiverse.com/thing:2415489)
- [gun](gun/): Program for guns.
- [target](target/): Program for target.
- [server](server/): Websocket protocol to communicate with [gun](gun/),[target](target/) and [server](target/).

## Setup instructions

1. Config the network at `DEFINE.h` in [gun](gun/) and [target](target/)
2. Start the server `cd ./server/ && pnpm dev`
3. Start the devices.
4. Connect to `2.`'s server using websocket. (example. [PieSocket Websocket](https://chrome.google.com/webstore/detail/piesocket-websocket-teste/oilioclnckkoijghdniegedkbocfpnip))

## Control Instructions

1. From 4. above, send "WEB" after server ask for TYPE to gain control (The ser will answer "WEB ACCEPTED")
2. Send "START" to start the game.
3. Send "STOP" to stop the game.

*Each score gain will send to WEB clients in format {score: n} which n is current score. Score reset each "START".*

## Play Instructions

1. Aim at the green light.
2. Shoot to gain 1 score.
3. Repeat.

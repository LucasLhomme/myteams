# myTeams — Client/Server messaging app in C++

A **Slack / TeamSpeak**-like messaging application, built as a team project at Epitech.
A network server handles multiple simultaneous clients that can join teams, chat in
channels, open discussion threads and exchange private messages, in real time.

## ✨ Features

- 🔐 User authentication (login / logout, unique UUID)
- 👥 Teams, channels and discussion threads
- 💬 Channel messages and **private messages** in real time
- 🔔 Team subscription system (subscribe / unsubscribe)
- 📡 Server handling **multiple simultaneous clients** (sockets, multiplexing)
- 📄 Communication protocol **documented in an RFC** ([`RFC/rfc-teams-protocol.txt`](RFC/rfc-teams-protocol.txt))

## 🛠️ Tech stack

- **Language**: C++
- **Networking**: sockets, multiplexing (non-blocking I/O)
- **Architecture**: client / server, object-oriented
- **Build**: Makefile

## 🚀 Build & run

```bash
make                      # build the server and the client

./myteams_server <port>   # start the server
./myteams_cli <ip> <port> # launch a client
```

Some client commands: `/login`, `/users`, `/send`, `/messages`,
`/create`, `/subscribe`, `/use`, `/info`, `/help`.

## 🧠 What I learned

Designing a real network architecture in C++ (OOP, handling concurrent connections)
and, above all, the importance of **clearly specifying a protocol (RFC)** before
writing a single line of code.

---

*Project built as part of the Epitech curriculum (NWP-400 module).*

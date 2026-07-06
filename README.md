# myTeams — Messagerie client/serveur en C++

Application de messagerie type **Slack / TeamSpeak**, développée en équipe à Epitech.
Un serveur réseau gère plusieurs clients simultanés qui peuvent rejoindre des équipes,
discuter dans des canaux, ouvrir des fils de discussion et s'échanger des messages privés,
en temps réel.

## ✨ Fonctionnalités

- 🔐 Authentification des utilisateurs (login / logout, UUID unique)
- 👥 Équipes, canaux et fils de discussion (threads)
- 💬 Messages de canal et **messages privés** en temps réel
- 🔔 Système d'abonnement aux équipes (subscribe / unsubscribe)
- 📡 Serveur gérant **plusieurs clients simultanés** (sockets, multiplexage)
- 📄 Protocole de communication **documenté dans une RFC** ([`RFC/rfc-teams-protocol.txt`](RFC/rfc-teams-protocol.txt))

## 🛠️ Stack technique

- **Langage** : C++
- **Réseau** : sockets, multiplexage (I/O non bloquantes)
- **Architecture** : client / serveur, orientée objet
- **Build** : Makefile

## 🚀 Compilation & lancement

```bash
make                      # compile le serveur et le client

./myteams_server <port>   # démarre le serveur
./myteams_cli <ip> <port> # lance un client
```

Quelques commandes côté client : `/login`, `/users`, `/send`, `/messages`,
`/create`, `/subscribe`, `/use`, `/info`, `/help`.

## 🧠 Ce que ce projet m'a apporté

Concevoir une véritable architecture réseau en C++ (POO, gestion de connexions
concurrentes) et surtout l'importance de **spécifier un protocole clairement (RFC)**
avant même d'écrire une ligne de code.

---

*Projet réalisé dans le cadre du cursus Epitech (module NWP-400).*

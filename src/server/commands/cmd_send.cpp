/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** cmd_send
*/

#include "server/commands/cmd_send.hpp"
#include "server/server.hpp"
#include "server/team.hpp"
#include <cstring>
#include <ctime>
#include <stdexcept>
#include <uuid/uuid.h>

extern "C" {
#include "../../../libs/myteams/logging_server.h"
}

void CmdSend::execute(Server &server, int client_socket, const std::vector<std::string> &args)
{
    if (args.size() < 3) {
        server.send_response(client_socket, "Usage: /send [user_uuid] [message_body]");
        return;
    }
    try {
        check_user_exists(server, args[1]);
    } catch (const std::runtime_error& e) {
        server.send_response(client_socket, e.what());
        return;
    }

    std::string sender_uuid;
    std::string sender_name = "Unknown";
    auto &socket_to_uuid = server.get_socket_to_uuid();
    auto it = socket_to_uuid.find(client_socket);
    if (it != socket_to_uuid.end())
        sender_uuid = it->second;

    auto &socket_to_name = server.get_socket_to_name();
    auto name_it = socket_to_name.find(client_socket);
    if (name_it != socket_to_name.end())
        sender_name = name_it->second;

    int reciver_socket = -1;
    for (const auto &pair : socket_to_uuid) {
        if (pair.second == args[1]) {
            reciver_socket = pair.first;
            break;
        }
    }
    if (reciver_socket == -1) {
        server.send_response(client_socket, "Recipient user is not online");
        return;
    } else if (reciver_socket == client_socket) {
        server.send_response(client_socket, "Cannot send message to yourself");
        return;
    }
    std::string message = args[2];
    for (size_t j = 3; j < args.size(); j++)
        message += " " + args[j];
    PrivateMessage *pm = new PrivateMessage;
    memset(pm, 0, sizeof(*pm));
    uuid_parse(sender_uuid.c_str(), pm->sender_uuid);
    uuid_t receiver_uuid_t;
    uuid_parse(args[1].c_str(), receiver_uuid_t);
    uuid_copy(pm->receiver_uuid, receiver_uuid_t);
    std::strncpy(pm->body, message.c_str(), MAX_BODY_LENGTH - 1);
    pm->body[MAX_BODY_LENGTH - 1] = '\0';
    pm->timestamp = time(nullptr);
    server.get_private_messages().push_back(pm);
    server.send_response(reciver_socket, "EVENT\tMESSAGE_RECEIVED\t" + sender_uuid + "\t" + message);
    server_event_private_message_sended(sender_uuid.c_str(), args[1].c_str(), message.c_str());
    server.send_response(client_socket, "Message sent to " + args[1] + ": " + message);
    server.save_state();
}
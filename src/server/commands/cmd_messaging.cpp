/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** messaging commands
*/

#include "server/server.hpp"
#include <stdexcept>
#include <uuid/uuid.h>

extern "C" {
#include "../../../libs/myteams/logging_server.h"
}

static void check_user_exists(const std::vector<User *> &users, const std::string &uuid_str) {
    for (const auto &user : users) {
        char u_str[MAX_UUID_LENGTH];
        uuid_unparse(user->uuid, u_str);
        if (std::string(u_str) == uuid_str) {
            return;
        }
    }
    throw std::runtime_error("ERROR_UNKNOWN_USER:" + uuid_str);
}

void Server::cmd_send(int client_socket, const std::vector<std::string> &args)
{
    if (args.size() < 3) {
        send_response(client_socket, "Usage: /send [user_uuid] [message_body]");
        return;
    }
    try {
        check_user_exists(_users, args[1]);
    } catch (const std::runtime_error& e) {
        send_response(client_socket, e.what());
        return;
    }

    std::string sender_uuid;
    std::string sender_name = "Unknown";
    auto it = _socket_to_uuid.find(client_socket);
    if (it != _socket_to_uuid.end()) {
        sender_uuid = it->second;
    }
    auto name_it = _socket_to_name.find(client_socket);
    if (name_it != _socket_to_name.end()) {
        sender_name = name_it->second;
    }
    int reciver_socket = -1;
    for (const auto &pair : _socket_to_uuid) {
        if (pair.second == args[1]) {
            reciver_socket = pair.first;
            break;
        }
    }
    if (reciver_socket == -1) {
        send_response(client_socket, "Recipient user is not online");
        return;
    } else if (reciver_socket == client_socket) {
        send_response(client_socket, "Cannot send message to yourself");
        return;
    }
    std::string message = args[2];
    for (size_t i = 3; i < args.size(); ++i) {
        message += " " + args[i];
    }

    send_response(reciver_socket, "Private message from " + sender_name + " (" + sender_uuid + "): " + message);
    server_event_private_message_sended(sender_uuid.c_str(), args[1].c_str(), message.c_str());
    std::string response = "Message sent to " + args[1] + ": " + message;


    send_response(client_socket, response);
}

void Server::cmd_messages(int client_socket, const std::vector<std::string> &args)
{
    if (args.size() < 2) {
        send_response(client_socket, "Usage: /messages [user_uuid]");
        return;
    }
    try {
        check_user_exists(_users, args[1]);
    } catch (const std::runtime_error& e) {
        send_response(client_socket, e.what());
        return;
    }
    send_response(client_socket, "cmd_messages");
}

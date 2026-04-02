/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** messaging commands
*/

#include "server/server.hpp"

extern "C" {
#include "../../../libs/myteams/logging_server.h"
}

void Server::cmd_send(int client_socket, const std::vector<std::string> &args)
{
    if (args.size() < 3) {
        send_response(client_socket, "Usage: /send [user_uuid] [message_body]");
        return;
    }
    send_response(client_socket, "cmd_send");
}

void Server::cmd_messages(int client_socket, const std::vector<std::string> &args)
{
    if (args.size() < 2) {
        send_response(client_socket, "Usage: /messages [user_uuid]");
        return;
    }
    send_response(client_socket, "cmd_messages");
}

/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** login command
*/

#include "server/server.hpp"

extern "C" {
#include "../../../libs/myteams/logging_server.h"
}

void Server::cmd_login(int client_socket, const std::vector<std::string> &args)
{
    if (args.size() < 2) {
        send_response(client_socket, "Usage: /login [user_name]");
        return;
    }
    // TODO: Real login logic with UUID and state management
    _socket_to_name[client_socket] = args[1];
    server_event_user_logged_in(args[1].c_str());
    send_response(client_socket, "cmd_login");
}

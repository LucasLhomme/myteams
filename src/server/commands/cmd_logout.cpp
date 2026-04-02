/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** logout command
*/

#include "server/server.hpp"

extern "C" {
#include "../../../libs/myteams/logging_server.h"
}

void Server::cmd_logout(int client_socket, const std::vector<std::string> &args)
{
    (void)args;
    auto it = _socket_to_name.find(client_socket);
    if (it != _socket_to_name.end()) {
        server_event_user_logged_out(it->second.c_str());
        _socket_to_name.erase(it);
    }
    send_response(client_socket, "Goodbye!");
    remove_client(client_socket);
}

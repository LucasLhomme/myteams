/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** user management commands
*/

#include "server/server.hpp"

void Server::cmd_users(int client_socket, const std::vector<std::string> &args)
{
    (void)args;
    send_response(client_socket, "cmd_users");
}

void Server::cmd_user(int client_socket, const std::vector<std::string> &args)
{
    if (args.size() < 2) {
        send_response(client_socket, "Usage: /user [user_uuid]");
        return;
    }
    send_response(client_socket, "cmd_user");
}

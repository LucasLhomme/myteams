/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** subscription commands
*/

#include "server/server.hpp"

extern "C" {
#include "../../../libs/myteams/logging_server.h"
}

void Server::cmd_subscribe(int client_socket, const std::vector<std::string> &args)
{
    if (args.size() < 2) {
        send_response(client_socket, "Usage: /subscribe [team_uuid]");
        return;
    }
    send_response(client_socket, "cmd_subscribe");
}

void Server::cmd_subscribed(int client_socket, const std::vector<std::string> &args)
{
    (void)args;
    send_response(client_socket, "cmd_subscribed");
}

void Server::cmd_unsubscribe(int client_socket, const std::vector<std::string> &args)
{
    if (args.size() < 2) {
        send_response(client_socket, "Usage: /unsubscribe [team_uuid]");
        return;
    }
    send_response(client_socket, "cmd_unsubscribe");
}

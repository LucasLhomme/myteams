/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** subscription commands
*/

#include "server/commands/cmd_subscribe.hpp"
#include "server/commands/cmd_subscribed.hpp"
#include "server/commands/cmd_unsubscribe.hpp"
#include "server/server.hpp"

extern "C" {
#include "../../../libs/myteams/logging_server.h"
}

void CmdSubscribe::execute(Server &server, int client_socket, const std::vector<std::string> &args)
{
    if (args.size() < 2) {
        server.send_response(client_socket, "Usage: /subscribe [team_uuid]");
        return;
    }
    server.send_response(client_socket, "cmd_subscribe");
}

void CmdSubscribed::execute(Server &server, int client_socket, const std::vector<std::string> &args)
{
    (void)args;
    server.send_response(client_socket, "cmd_subscribed");
}

void CmdUnsubscribe::execute(Server &server, int client_socket, const std::vector<std::string> &args)
{
    if (args.size() < 2) {
        server.send_response(client_socket, "Usage: /unsubscribe [team_uuid]");
        return;
    }
    server.send_response(client_socket, "cmd_unsubscribe");
}

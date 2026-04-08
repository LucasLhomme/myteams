/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** cmd_use
*/

#include "server/commands/cmd_use.hpp"
#include "server/server.hpp"

void CmdUse::execute(Server &server, int client_socket, const std::vector<std::string> &args)
{
    server.send_response(client_socket, "Use command is not implemented yet");
}
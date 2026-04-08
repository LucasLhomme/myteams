/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** cmd_create
*/

#include "server/commands/cmd_create.hpp"
#include "server/server.hpp"

void CmdCreate::execute(Server &server, int client_socket, const std::vector<std::string> &args)
{
    server.send_response(client_socket, "Create command is not implemented yet");
}
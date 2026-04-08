/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** cmd_info
*/

#include "server/commands/cmd_info.hpp"
#include "server/server.hpp"

void CmdInfo::execute(Server &server, int client_socket, const std::vector<std::string> &args)
{
    server.send_response(client_socket, "Info command is not implemented yet");
}
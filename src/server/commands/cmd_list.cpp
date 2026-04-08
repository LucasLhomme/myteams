/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** cmd_list
*/

#include "server/commands/cmd_list.hpp"
#include "server/server.hpp"

void CmdList::execute(Server &server, int client_socket, const std::vector<std::string> &args)
{
    server.send_response(client_socket, "List command is not implemented yet");
}
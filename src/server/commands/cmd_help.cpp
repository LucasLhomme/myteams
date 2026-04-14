/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** help command
*/

#include "server/commands/cmd_help.hpp"
#include "server/server.hpp"
#include <iostream>

void CmdHelp::execute(Server &server, int client_socket, const std::vector<std::string> &args)
{
    try {
    (void)args;
    server.send_response(client_socket, "Available commands: /login, /logout, /users, /user, /send, /messages, /subscribe, /subscribed, /unsubscribe, /help");

    } catch (const std::exception &e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
    }}

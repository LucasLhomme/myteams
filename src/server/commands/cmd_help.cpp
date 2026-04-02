/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** help command
*/

#include "server/server.hpp"

void Server::cmd_help(int client_socket, const std::vector<std::string> &args)
{
    (void)args;
    send_response(client_socket, "Available commands: /login, /logout, /users, /user, /send, /messages, /subscribe, /subscribed, /unsubscribe, /help");
}

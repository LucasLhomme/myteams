/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** user management commands
*/

#include "server/server.hpp"

std::vector<std::string> Server::get_all_usernames() {
    std::vector<std::string> usernames;
    for (const auto& user : _users) {
        usernames.push_back(user->username);
    }
    return usernames;
}

void Server::cmd_users(int client_socket, const std::vector<std::string> &args)
{
    (void)args;
    std::vector<std::string> users_list = get_all_usernames();
    std::string response;
    for (const auto& name : users_list) {
        response += name + "\n";
    }
    if (response.empty())
        response = "No users found.";
    send_response(client_socket, response);
}

void Server::cmd_user(int client_socket, const std::vector<std::string> &args)
{
    if (args.size() < 2) {
        send_response(client_socket, "Usage: /user [user_uuid]");
        return;
    }
    send_response(client_socket, "cmd_user");
}

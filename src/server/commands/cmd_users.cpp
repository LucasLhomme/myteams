/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** user management commands
*/

#include "server/server.hpp"
#include <iostream>

std::vector<std::string> Server::get_all_usernames() {
    std::vector<std::string> usernames;
    for (const auto& user : _users) {
        usernames.push_back(user->username);
    }
    return usernames;
}

void Server::GetUserInfo(int client_socket, const std::string &uuid_str) {
    for (const auto &user : _users) {
        char u_str[MAX_UUID_LENGTH];
        uuid_unparse(user->uuid, u_str);
        if (std::string(u_str) == uuid_str) {
            std::string response = "User Info:\n";
            response += "UUID: " + std::string(u_str) + "\n";
            response += "Username: " + std::string(user->username) + "\n";
            response += "Status: " + std::string(user->is_online ? "Online" : "Offline") + "\n";
            send_response(client_socket, response);
            return;
        }
    }
    send_response(client_socket, "ERROR_UNKNOWN_USER:" + uuid_str);
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
    std::cout << response;
    send_response(client_socket, response);
}

void Server::cmd_user(int client_socket, const std::vector<std::string> &args)
{
    if (args.size() < 2) {
        send_response(client_socket, "Usage: /user [user_uuid]");
        return;
    }
    GetUserInfo(client_socket, args[1]);
}

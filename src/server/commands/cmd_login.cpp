/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** login command
*/

#include "server/server.hpp"
#include <cstring>
extern "C" {
#include "../../../libs/myteams/logging_server.h"
}

void Server::cmd_login(int client_socket, const std::vector<std::string> &args)
{
    if (args.size() < 2) {
        send_response(client_socket, "Usage: /login [user_name]");
        return;
    }

    std::string username = args[1];
    if (username.length() > MAX_NAME_LENGTH) {
        username = username.substr(0, MAX_NAME_LENGTH);
    }

    User *user = nullptr;
    for (auto &tmp : _users) {
        if (std::string(tmp->username) == username) {
            user = tmp;
            break;
        }
    }

    if (!user) {
        user = new User;
        uuid_generate(user->uuid);
        std::strncpy(user->username, username.c_str(), MAX_NAME_LENGTH);
        user->is_online = true;
        _users.push_back(user);
        
        char u_str[MAX_UUID_LENGTH];
        uuid_unparse(user->uuid, u_str);
        server_event_user_created(u_str, user->username);
    }

    user->is_online = true;
    char uuid_str[MAX_UUID_LENGTH];
    uuid_unparse(user->uuid, uuid_str);
    
    _socket_to_name[client_socket] = user->username;
    _socket_to_uuid[client_socket] = uuid_str;

    server_event_user_logged_in(uuid_str);
    send_response(client_socket, "Login success: " + std::string(uuid_str));
}

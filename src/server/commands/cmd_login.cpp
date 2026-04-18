#include <iostream>
/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** login command
*/

#include "server/commands/cmd_login.hpp"
#include "server/server.hpp"
#include <cstring>
extern "C" {
#include "../../../libs/myteams/logging_server.h"

}

void CmdLogin::execute(Server &server, int client_socket, const std::vector<std::string> &args)
{
    try {
    if (args.size() < 2) {
        server.send_response(client_socket, "Usage: /login [user_name]");
        return;
    }

    std::string username = args[1];
    if (username.length() >= MAX_NAME_LENGTH) {
        username = username.substr(0, MAX_NAME_LENGTH - 1);
    }

    User *user = nullptr;
    for (auto &tmp : server.get_users()) {
        if (std::string(tmp->username) == username) {
            user = tmp;
            break;
        }
    }

    if (!user) {
        user = new User;
        uuid_generate(user->uuid);
        std::strncpy(user->username, username.c_str(), MAX_NAME_LENGTH);
            user->username[MAX_NAME_LENGTH - 1] = '\0';
        user->is_online = true;
        server.get_users().push_back(user);
        
        char u_str[MAX_UUID_LENGTH];
        uuid_unparse(user->uuid, u_str);
        server_event_user_created(u_str, user->username);
    }

    user->is_online = true;
    char uuid_str[MAX_UUID_LENGTH];
    uuid_unparse(user->uuid, uuid_str);
    
    server.get_socket_to_name()[client_socket] = user->username;
    server.get_socket_to_uuid()[client_socket] = uuid_str;

    server_event_user_logged_in(uuid_str);
    server.broadcast_event("EVENT\tLOGGED_IN\t" + std::string(uuid_str) + "\t" + user->username, client_socket);
    server.send_response(client_socket, "Login success: " + std::string(uuid_str));
    server.save_state();

    } catch (const std::exception &e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
    }}

#include <iostream>
/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** logout command
*/

#include "server/commands/cmd_logout.hpp"
#include "server/server.hpp"

extern "C" {
#include "../../../libs/myteams/logging_server.h"

}

void CmdLogout::execute(Server &server, int client_socket, [[maybe_unused]] const std::vector<std::string> &args) {
    try {
    auto it = server.get_socket_to_uuid().find(client_socket);
    std::string uuid_str = "";
    std::string username_str = "";

    if (it != server.get_socket_to_uuid().end()) {
        uuid_str = it->second;
        for (auto &user : server.get_users()) {
            char u_str[MAX_UUID_LENGTH];
            uuid_unparse(user->uuid, u_str);
            if (std::string(u_str) == uuid_str) {
                username_str = user->username;
                user->is_online = false;
                break;
            }
        }
        server_event_user_logged_out(uuid_str.c_str());
        server.broadcast_event("EVENT\tLOGGED_OUT\t" + uuid_str + "\t" + username_str, client_socket);
        server.send_response(client_socket, "USER_LOGGED_OUT " + uuid_str + " " + username_str);
        
        server.get_socket_to_uuid().erase(it);
        server.get_socket_to_name().erase(client_socket);
        server.get_use_team_context().erase(client_socket);
        server.get_use_channel_context().erase(client_socket);
        server.get_use_thread_context().erase(client_socket);
        server.save_state();
    } else {
        server.send_response(client_socket, "USER_LOGGED_OUT UNKNOWN UNKNOWN");
    }

    } catch (const std::exception &e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
    }}

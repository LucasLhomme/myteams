/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** user management commands
*/

#include "server/commands/cmd_users.hpp"
#include "server/commands/cmd_user.hpp"
#include "server/server.hpp"
#include <iostream>
#include <sstream>

static std::string uuid_to_string(const uuid_t uuid) {
    char uuid_str[MAX_UUID_LENGTH];
    uuid_unparse(uuid, uuid_str);
    return std::string(uuid_str);
}

void CmdUsers::execute(Server &server, int client_socket, const std::vector<std::string> &args) {
    try {
        (void)args;
        const auto& users = server.get_users();
        std::ostringstream response;
        for (const auto& user : users) {
             response << "USER\t" << uuid_to_string(user->uuid) << "\t" << user->username << "\t"
                      << (user->is_online ? 1 : 0) << "\n";
        }
        if (response.str().empty())
            server.send_response(client_socket, "No users found.");
        else
            server.send_response(client_socket, response.str());
    } catch (const std::exception &e) {
        std::cout << "[ERROR] CmdUsers::execute: " << e.what() << std::endl;
    }
}

void CmdUser::execute(Server &server, int client_socket, const std::vector<std::string> &args) {
    try {
        if (args.size() < 2) {
            server.send_response(client_socket, "Usage: /user [user_uuid]");
            return;
        }
        const auto& users = server.get_users();
        for (const auto& user : users) {
            if (uuid_to_string(user->uuid) == args[1]) {
                std::ostringstream response;
                response << "USER\t" << uuid_to_string(user->uuid) << "\t" << user->username << "\t"
                         << (user->is_online ? 1 : 0);
                server.send_response(client_socket, response.str());
                return;
            }
        }
        server.send_response(client_socket, "ERROR_UNKNOWN_USER:" + args[1]);
    } catch (const std::exception &e) {
        std::cout << "[ERROR] CmdUser::execute: " << e.what() << std::endl;
    }
}

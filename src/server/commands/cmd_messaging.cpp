/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** messaging commands
*/

#include "server/commands/cmd_messages.hpp"
#include "server/server.hpp"
#include <stdexcept>
#include <uuid/uuid.h>

extern "C" {
#include "../../../libs/myteams/logging_server.h"
}

void CmdMessages::execute(Server &server, int client_socket, const std::vector<std::string> &args)
{
    if (args.size() < 2) {
        server.send_response(client_socket, "Usage: /messages [user_uuid]");
        return;
    }
    try {
        check_user_exists(server, args[1]);
    } catch (const std::runtime_error& e) {
        server.send_response(client_socket, e.what());
        return;
    }
    server.send_response(client_socket, "cmd_messages");
}

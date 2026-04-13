/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** messaging commands
*/

#include "server/commands/cmd_messages.hpp"
#include "server/server.hpp"
#include "server/team.hpp"
#include <stdexcept>
#include <cstring>
#include <ctime>
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
    auto &socket_to_uuid = server.get_socket_to_uuid();
    auto it = socket_to_uuid.find(client_socket);
    if (it == socket_to_uuid.end()) {
        server.send_response(client_socket, "You must be logged in to read messages");
        return;
    }
    const std::string &my_uuid = it->second;

    uuid_t my_uuid_t;
    uuid_t other_uuid_t;
    if (uuid_parse(my_uuid.c_str(), my_uuid_t) != 0 ||
        uuid_parse(args[1].c_str(), other_uuid_t) != 0) {
        server.send_response(client_socket, "Invalid UUID");
        return;
    }

    bool found = false;
    for (const auto &pm : server.get_private_messages()) {
        if (!pm)
            continue;
        bool sent_by_me = (uuid_compare(pm->sender_uuid,   my_uuid_t)    == 0 &&
                           uuid_compare(pm->receiver_uuid, other_uuid_t) == 0);
        bool sent_to_me = (uuid_compare(pm->sender_uuid,   other_uuid_t) == 0 &&
                           uuid_compare(pm->receiver_uuid, my_uuid_t)    == 0);
        if (!sent_by_me && !sent_to_me)
            continue;

        found = true;
        char sender_buf[MAX_UUID_LENGTH];
        char receiver_buf[MAX_UUID_LENGTH];
        uuid_unparse(pm->sender_uuid,   sender_buf);
        uuid_unparse(pm->receiver_uuid, receiver_buf);
        std::string line = std::string(sender_buf) + " -> " + receiver_buf
                        + " [" + std::to_string((long long)pm->timestamp) + "]: "
                        + pm->body;
        server.send_response(client_socket, line);
    }
    if (!found)
        server.send_response(client_socket, "No messages found with this user.");
}

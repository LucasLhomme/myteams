/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** Shared utils for commands
*/

#include "server/commands/ACommand.hpp"
#include <uuid/uuid.h>
#include <stdexcept>

void ACommand::check_user_exists(const Server &server, const std::string &uuid_str) const {
    for (const auto &user : server.get_users()) {
        char u_str[MAX_UUID_LENGTH];
        uuid_unparse(user->uuid, u_str);
        if (std::string(u_str) == uuid_str) {
            return;
        }
    }
    throw std::runtime_error("ERROR_UNKNOWN_USER:" + uuid_str);
}
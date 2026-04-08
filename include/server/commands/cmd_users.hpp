/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** cmd_users header declaration
*/

#ifndef CMD_USERS_HPP_
#define CMD_USERS_HPP_

#include "ACommand.hpp"

class CmdUsers : public ACommand {
public:
    CmdUsers() = default;
    ~CmdUsers() override = default;

    void execute(Server &server, int client_socket, const std::vector<std::string> &args) override;
};

#endif /* !CMD_USERS_HPP_ */
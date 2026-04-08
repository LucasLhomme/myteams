/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** cmd_user header declaration
*/

#ifndef CMD_USER_HPP_
#define CMD_USER_HPP_

#include "ACommand.hpp"

class CmdUser : public ACommand {
public:
    CmdUser() = default;
    ~CmdUser() override = default;

    void execute(Server &server, int client_socket, const std::vector<std::string> &args) override;
};

#endif /* !CMD_USER_HPP_ */
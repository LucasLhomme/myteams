/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** cmd_login header declaration
*/

#ifndef CMD_LOGIN_HPP_
#define CMD_LOGIN_HPP_

#include "ACommand.hpp"

class CmdLogin : public ACommand {
public:
    CmdLogin() = default;
    ~CmdLogin() override = default;

    void execute(Server &server, int client_socket, const std::vector<std::string> &args) override;
};

#endif /* !CMD_LOGIN_HPP_ */
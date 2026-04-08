/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** cmd_logout header declaration
*/

#ifndef CMD_LOGOUT_HPP_
#define CMD_LOGOUT_HPP_

#include "ACommand.hpp"

class CmdLogout : public ACommand {
public:
    CmdLogout() = default;
    ~CmdLogout() override = default;

    void execute(Server &server, int client_socket, const std::vector<std::string> &args) override;
};

#endif /* !CMD_LOGOUT_HPP_ */
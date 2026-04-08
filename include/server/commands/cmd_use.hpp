/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** cmd_use header declaration
*/

#ifndef CMD_USE_HPP_
#define CMD_USE_HPP_

#include "ACommand.hpp"

class CmdUse : public ACommand {
public:
    CmdUse() = default;
    ~CmdUse() override = default;

    void execute(Server &server, int client_socket, const std::vector<std::string> &args) override;
};

#endif /* !CMD_USE_HPP_ */
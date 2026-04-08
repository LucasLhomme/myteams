/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** cmd_info header declaration
*/

#ifndef CMD_INFO_HPP_
#define CMD_INFO_HPP_

#include "ACommand.hpp"

class CmdInfo : public ACommand {
public:
    CmdInfo() = default;
    ~CmdInfo() override = default;

    void execute(Server &server, int client_socket, const std::vector<std::string> &args) override;
};

#endif /* !CMD_INFO_HPP_ */
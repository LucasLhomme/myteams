/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** cmd_list header declaration
*/

#ifndef CMD_LIST_HPP_
#define CMD_LIST_HPP_

#include "ACommand.hpp"

class CmdList : public ACommand {
public:
    CmdList() = default;
    ~CmdList() override = default;

    void execute(Server &server, int client_socket, const std::vector<std::string> &args) override;
};

#endif /* !CMD_LIST_HPP_ */
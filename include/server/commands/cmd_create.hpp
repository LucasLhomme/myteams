/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** cmd_create header declaration
*/

#ifndef CMD_CREATE_HPP_
#define CMD_CREATE_HPP_

#include "ACommand.hpp"

class CmdCreate : public ACommand {
public:
    CmdCreate() = default;
    ~CmdCreate() override = default;

    void execute(Server &server, int client_socket, const std::vector<std::string> &args) override;
};

#endif /* !CMD_CREATE_HPP_ */
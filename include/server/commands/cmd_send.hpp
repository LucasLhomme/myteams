/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** cmd_send header declaration
*/

#ifndef CMD_SEND_HPP_
#define CMD_SEND_HPP_

#include "ACommand.hpp"

class CmdSend : public ACommand {
public:
    CmdSend() = default;
    ~CmdSend() override = default;

    void execute(Server &server, int client_socket, const std::vector<std::string> &args) override;
};

#endif /* !CMD_SEND_HPP_ */
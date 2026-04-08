/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** cmd_messages header declaration
*/

#ifndef CMD_MESSAGES_HPP_
#define CMD_MESSAGES_HPP_

#include "ACommand.hpp"

class CmdMessages : public ACommand {
public:
    CmdMessages() = default;
    ~CmdMessages() override = default;

    void execute(Server &server, int client_socket, const std::vector<std::string> &args) override;
};

#endif /* !CMD_MESSAGES_HPP_ */
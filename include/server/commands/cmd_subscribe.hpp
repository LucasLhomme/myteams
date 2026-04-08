/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** cmd_subscribe header declaration
*/

#ifndef CMD_SUBSCRIBE_HPP_
#define CMD_SUBSCRIBE_HPP_

#include "ACommand.hpp"

class CmdSubscribe : public ACommand {
public:
    CmdSubscribe() = default;
    ~CmdSubscribe() override = default;

    void execute(Server &server, int client_socket, const std::vector<std::string> &args) override;
};

#endif /* !CMD_SUBSCRIBE_HPP_ */

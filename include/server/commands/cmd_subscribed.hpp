/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** cmd_subscribed header declaration
*/

#ifndef CMD_SUBSCRIBED_HPP_
#define CMD_SUBSCRIBED_HPP_

#include "ACommand.hpp"

class CmdSubscribed : public ACommand {
public:
    CmdSubscribed() = default;
    ~CmdSubscribed() override = default;

    void execute(Server &server, int client_socket, const std::vector<std::string> &args) override;
};

#endif /* !CMD_SUBSCRIBED_HPP_ */

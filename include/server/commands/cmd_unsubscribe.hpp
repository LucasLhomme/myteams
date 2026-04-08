/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** cmd_unsubscribe header declaration
*/

#ifndef CMD_UNSUBSCRIBE_HPP_
#define CMD_UNSUBSCRIBE_HPP_

#include "ACommand.hpp"

class CmdUnsubscribe : public ACommand {
public:
    CmdUnsubscribe() = default;
    ~CmdUnsubscribe() override = default;

    void execute(Server &server, int client_socket, const std::vector<std::string> &args) override;
};

#endif /* !CMD_UNSUBSCRIBE_HPP_ */

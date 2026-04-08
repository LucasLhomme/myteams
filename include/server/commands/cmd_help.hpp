/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** cmd_help header declaration
*/

#ifndef CMD_HELP_HPP_
#define CMD_HELP_HPP_

#include "ACommand.hpp"

class CmdHelp : public ACommand {
public:
    CmdHelp() = default;
    ~CmdHelp() override = default;

    void execute(Server &server, int client_socket, const std::vector<std::string> &args) override;
};

#endif /* !CMD_HELP_HPP_ */

/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** help
*/

#ifndef HELP_COMMAND_HPP_
#define HELP_COMMAND_HPP_

#include "../commands.hpp"

class HelpCommand : public ICommand {
    public:
        void execute(const std::string& input, Cli::Socket& socket, Cli::Context& context) override;
        bool requiresAuth() const override { return false; }
        std::string getName() const override { return "help"; }
};

#endif /* !HELP_COMMAND_HPP_ */
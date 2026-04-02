/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** info
*/

#ifndef INFO_COMMAND_HPP_
#define INFO_COMMAND_HPP_

#include "../commands.hpp"

class InfoCommand : public ICommand {
    public:
        void execute(const std::string& input, Cli::Socket& socket) override;
        bool requiresAuth() const override { return true; }
        std::string getName() const override { return "info"; }
};

#endif /* !INFO_COMMAND_HPP_ */
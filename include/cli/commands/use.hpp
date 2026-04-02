/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** use
*/

#ifndef USE_COMMAND_HPP_
#define USE_COMMAND_HPP_

#include "../commands.hpp"

class UseCommand : public ICommand {
    public:
        void execute(const std::string& input, Cli::Socket& socket) override;
        bool requiresAuth() const override { return true; }
        std::string getName() const override { return "use"; }
};

#endif /* !USE_COMMAND_HPP_ */
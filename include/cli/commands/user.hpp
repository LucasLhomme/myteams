/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** user
*/

#ifndef USER_COMMAND_HPP_
#define USER_COMMAND_HPP_

#include "../commands.hpp"

class UserCommand : public ICommand {
    public:
        void execute(const std::string& input, Cli::Socket& socket, Cli::Context& context) override;
        bool requiresAuth() const override { return true; }
        std::string getName() const override { return "user"; }
};

#endif /* !USER_COMMAND_HPP_ */
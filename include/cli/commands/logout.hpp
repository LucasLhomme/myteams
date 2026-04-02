/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** logout
*/

#ifndef LOGOUT_COMMAND_HPP_
#define LOGOUT_COMMAND_HPP_

#include "../commands.hpp"

class LogoutCommand : public ICommand {
    public:
        void execute(const std::string& input, Cli::Socket& socket) override;
        bool requiresAuth() const override { return true; }
        std::string getName() const override { return "logout"; }
};

#endif /* !LOGOUT_COMMAND_HPP_ */
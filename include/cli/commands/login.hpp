/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** login
*/

#ifndef LOGIN_COMMAND_HPP_
#define LOGIN_COMMAND_HPP_

#include "../commands.hpp"

class LoginCommand : public ICommand {
    public:
        void execute() override;
        bool requiresAuth() const override { return false; }
        std::string getName() const override { return "login"; }
};

#endif /* !LOGIN_COMMAND_HPP_ */
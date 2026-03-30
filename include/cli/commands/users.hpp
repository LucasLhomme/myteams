/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** users
*/

#ifndef USERS_COMMAND_HPP_
#define USERS_COMMAND_HPP_

#include "../commands.hpp"

class UsersCommand : public ICommand {
    public:
        void execute() override;
        bool requiresAuth() const override { return true; }
        std::string getName() const override { return "users"; }
};

#endif /* !USERS_COMMAND_HPP_ */
/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** create
*/

#ifndef CREATE_COMMAND_HPP_
#define CREATE_COMMAND_HPP_

#include "../commands.hpp"

class CreateCommand : public ICommand {
    public:
        void execute() override;
        bool requiresAuth() const override { return true; }
        std::string getName() const override { return "create"; }
};

#endif /* !CREATE_COMMAND_HPP_ */
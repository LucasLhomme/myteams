/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** list
*/

#ifndef LIST_COMMAND_HPP_
#define LIST_COMMAND_HPP_

#include "../commands.hpp"

class ListCommand : public ICommand {
    public:
        void execute() override;
        bool requiresAuth() const override { return true; }
        std::string getName() const override { return "list"; }
};

#endif /* !LIST_COMMAND_HPP_ */
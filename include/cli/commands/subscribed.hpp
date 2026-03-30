/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** subscribed
*/

#ifndef SUBSCRIBED_COMMAND_HPP_
#define SUBSCRIBED_COMMAND_HPP_

#include "../commands.hpp"

class SubscribedCommand : public ICommand {
    public:
        void execute() override;
        bool requiresAuth() const override { return true; }
        std::string getName() const override { return "subscribed"; }
};

#endif /* !SUBSCRIBED_COMMAND_HPP_ */
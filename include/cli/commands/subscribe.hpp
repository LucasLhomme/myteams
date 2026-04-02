/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** subscribe
*/

#ifndef SUBSCRIBE_COMMAND_HPP_
#define SUBSCRIBE_COMMAND_HPP_

#include "../commands.hpp"

class SubscribeCommand : public ICommand {
    public:
        void execute(const std::string& input, Cli::Socket& socket) override;
        bool requiresAuth() const override { return true; }
        std::string getName() const override { return "subscribe"; }
};

#endif /* !SUBSCRIBE_COMMAND_HPP_ */
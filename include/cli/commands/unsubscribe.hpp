/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** unsubscribe
*/

#ifndef UNSUBSCRIBE_COMMAND_HPP_
#define UNSUBSCRIBE_COMMAND_HPP_

#include "../commands.hpp"

class UnsubscribeCommand : public ICommand {
    public:
        void execute(const std::string& input, Cli::Socket& socket, Cli::Context& context) override;
        bool requiresAuth() const override { return true; }
        std::string getName() const override { return "unsubscribe"; }
};

#endif /* !UNSUBSCRIBE_COMMAND_HPP_ */
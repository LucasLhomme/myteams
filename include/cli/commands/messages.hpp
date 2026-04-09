/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** messages
*/

#ifndef MESSAGES_COMMAND_HPP_
#define MESSAGES_COMMAND_HPP_

#include "../commands.hpp"

class MessagesCommand : public ICommand {
    public:
        void execute(const std::string& input, Cli::Socket& socket, Cli::Context& context) override;
        bool requiresAuth() const override { return true; }
        std::string getName() const override { return "messages"; }
};

#endif /* !MESSAGES_COMMAND_HPP_ */
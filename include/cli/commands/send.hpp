/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** send
*/

#ifndef SEND_COMMAND_HPP_
#define SEND_COMMAND_HPP_

#include "../commands.hpp"

class SendCommand : public ICommand {
    public:
        void execute(const std::string& input, Cli::Socket& socket, Cli::Context& context) override;
        bool requiresAuth() const override { return true; }
        std::string getName() const override { return "send"; }
};

#endif /* !SEND_COMMAND_HPP_ */
/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** commands
*/

#ifndef COMMANDS_HPP_
#define COMMANDS_HPP_

#include <string>
#include <unordered_map>
#include <memory>
#include "./socket.hpp"
#include "./context.hpp"

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void execute(const std::string& input, Cli::Socket& socket, Cli::Context& context) = 0;
    virtual bool requiresAuth() const = 0;
    virtual std::string getName() const = 0;
};

class CommandHandler {
private:
    std::unordered_map<std::string, std::unique_ptr<ICommand>> _commands;
public:
    CommandHandler();
    ~CommandHandler() = default;
    
    void process_command(const std::string& command, Cli::Socket& socket, Cli::Context& context);
    void execute_command(const std::string& command, const std::string& input, Cli::Socket& socket, Cli::Context& context);
};

#endif /* !COMMANDS_HPP_ */

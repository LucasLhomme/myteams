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
#include <vector>
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

    static bool handle_async_event(const std::string& line);
    static std::string receive_response(Cli::Socket& socket, int timeout_ms = 5000);
};

std::vector<std::string> split_args(const std::string& command);

#endif /* !COMMANDS_HPP_ */

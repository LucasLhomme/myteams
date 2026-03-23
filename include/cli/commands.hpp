/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** commands
*/

#ifndef COMMANDS_HPP_
#define COMMANDS_HPP_

#include <map>
#include <string>

class ICommands {
    private:
        // int 1 = command for everyone | int 0 = user auth command
        // std::string for the command name 
        std::map<int, std::string> _commands;
    public:
        ICommands() {
            _commands[1] = "help"; // Help command
            _commands[1] = "login"; // login command
            _commands[1] = "logout"; // disconnect user
            _commands[1] = "users"; // List all users
            _commands[1] = "user"; // Get user information
            _commands[1] = "send"; // Send a message
            _commands[1] = "messages"; // List messages
            _commands[1] = "subscribe"; // Subscribe to a team
            _commands[1] = "subscribed"; // List subscribed teams
            _commands[1] = "unsubscribe"; // Unsubscribe from a team
            _commands[1] = "use"; // Use a team
            _commands[1] = "create"; // Create a new team
            _commands[1] = "list"; // List teams
            _commands[1] = "info"; // Get team information
        }
        virtual ~ICommands() = default;
        virtual void process_command(const char *command) = 0;
        virtual void execute_command(const char *command) = 0;
    };

#endif /* !COMMANDS_HPP_ */

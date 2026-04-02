/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** commands
*/
#include "./cli/commands.hpp"
#include "./cli/commands/help.hpp"
#include "./cli/commands/login.hpp"
#include "./cli/commands/logout.hpp"
#include "./cli/commands/users.hpp"
#include "./cli/commands/user.hpp"
#include "./cli/commands/send.hpp"
#include "./cli/commands/messages.hpp"
#include "./cli/commands/subscribe.hpp"
#include "./cli/commands/subscribed.hpp"
#include "./cli/commands/unsubscribe.hpp"
#include "./cli/commands/use.hpp"
#include "./cli/commands/create.hpp"
#include "./cli/commands/list.hpp"
#include "./cli/commands/info.hpp"
#include <iostream>

CommandHandler::CommandHandler()
{
    _commands["help"] = std::make_unique<HelpCommand>();
    _commands["login"] = std::make_unique<LoginCommand>();
    _commands["logout"] = std::make_unique<LogoutCommand>();
    _commands["users"] = std::make_unique<UsersCommand>();
    _commands["user"] = std::make_unique<UserCommand>();
    _commands["send"] = std::make_unique<SendCommand>();
    _commands["messages"] = std::make_unique<MessagesCommand>();
    _commands["subscribe"] = std::make_unique<SubscribeCommand>();
    _commands["subscribed"] = std::make_unique<SubscribedCommand>();
    _commands["unsubscribe"] = std::make_unique<UnsubscribeCommand>();
    _commands["use"] = std::make_unique<UseCommand>();
    _commands["create"] = std::make_unique<CreateCommand>();
    _commands["list"] = std::make_unique<ListCommand>();
    _commands["info"] = std::make_unique<InfoCommand>();
}

void CommandHandler::execute_command(const std::string& command, const std::string& input, Cli::Socket& socket)
{
    auto it = _commands.find(command);
    if (it != _commands.end()) {
        // Ajout d'un \r\n à la fin de la commande pour le protocole
        std::string input_with_crlf = input;
        if (input_with_crlf.size() < 2 || input_with_crlf.substr(input_with_crlf.size()-2) != "\r\n")
            input_with_crlf += "\r\n";
        it->second->execute(input_with_crlf, socket);
    } else {
        std::cout << "Unknown command" << std::endl;
    }
}

void CommandHandler::process_command(const std::string& command, Cli::Socket& socket)
{
    for (const auto& [cmd_name, cmd_obj] : _commands) {
        if (command.find(cmd_name) != std::string::npos) {
            execute_command(cmd_name, command, socket);
            break;
        }
    }
}
/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** commands
*/
#include "../../include/cli/commands.hpp"
#include "../../include/cli/commands/help.hpp"
#include "../../include/cli/commands/login.hpp"
#include "../../include/cli/commands/logout.hpp"
#include "../../include/cli/commands/users.hpp"
#include "../../include/cli/commands/user.hpp"
#include "../../include/cli/commands/send.hpp"
#include "../../include/cli/commands/messages.hpp"
#include "../../include/cli/commands/subscribe.hpp"
#include "../../include/cli/commands/subscribed.hpp"
#include "../../include/cli/commands/unsubscribe.hpp"
#include "../../include/cli/commands/use.hpp"
#include "../../include/cli/commands/create.hpp"
#include "../../include/cli/commands/list.hpp"
#include "../../include/cli/commands/info.hpp"
#include <iostream>
#include <vector>
#include <sstream>

extern "C" {
#include "../../libs/myteams/logging_client.h"
}

static std::vector<std::string> split_tab_fields(const std::string &line)
{
    std::vector<std::string> fields;
    std::string field;
    std::istringstream stream(line);

    while (std::getline(stream, field, '\t')) {
        fields.push_back(field);
    }
    return fields;
}

bool CommandHandler::handle_async_event(const std::string &line)
{
    const std::vector<std::string> fields = split_tab_fields(line);

    if (fields.size() < 2 || fields[0] != "EVENT") {
        return false;
    }

    if (fields[1] == "LOGGED_IN" && fields.size() >= 4) {
        client_event_logged_in(fields[2].c_str(), fields[3].c_str());
        return true;
    }
    if (fields[1] == "LOGGED_OUT" && fields.size() >= 4) {
        client_event_logged_out(fields[2].c_str(), fields[3].c_str());
        return true;
    }
    if (fields[1] == "MESSAGE_RECEIVED" && fields.size() >= 4) {
        client_event_private_message_received(fields[2].c_str(), fields[3].c_str());
        return true;
    }
    if (fields[1] == "TEAM_CREATED" && fields.size() >= 5) {
        client_event_team_created(fields[2].c_str(), fields[3].c_str(), fields[4].c_str());
        return true;
    }
    if (fields[1] == "CHANNEL_CREATED" && fields.size() >= 5) {
        client_event_channel_created(fields[2].c_str(), fields[3].c_str(), fields[4].c_str());
        return true;
    }
    if (fields[1] == "THREAD_CREATED" && fields.size() >= 7) {
        const time_t timestamp = static_cast<time_t>(std::stoll(fields[4]));
        client_event_thread_created(fields[2].c_str(), fields[3].c_str(), timestamp,
            fields[5].c_str(), fields[6].c_str());
        return true;
    }
    if (fields[1] == "THREAD_REPLY_RECEIVED" && fields.size() >= 6) {
        client_event_thread_reply_received(fields[2].c_str(), fields[3].c_str(),
            fields[4].c_str(), fields[5].c_str());
        return true;
    }
    return false;
}

std::string CommandHandler::receive_response(Cli::Socket& socket, int timeout_ms)
{
    while (true) {
        std::string line = socket.receive_data(timeout_ms);
        if (line.empty())
            return "";
        if (!handle_async_event(line)) {
            return line;
        }
    }
}

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

void CommandHandler::execute_command(const std::string& command, const std::string& input, Cli::Socket& socket, Cli::Context& context)
{
    auto it = _commands.find(command);
    if (it != _commands.end()) {
        try {
            std::string input_with_crlf = input;
            if (input_with_crlf.size() < 2 || input_with_crlf.substr(input_with_crlf.size()-2) != "\r\n")
                input_with_crlf += "\r\n";
            it->second->execute(input_with_crlf, socket, context);
        } catch (const std::exception& e) {
            std::cout << "[ERROR] Exception in command '" << command << "': " << e.what() << std::endl;
        }
    } else {
        std::cout << "Unknown command" << std::endl;
    }
}

std::vector<std::string> split_args(const std::string& command) {
    std::vector<std::string> args;
    std::string current;
    bool in_quotes = false;

    for (char c : command) {
        if (c == '"') {
            in_quotes = !in_quotes;
        } else if (c == ' ' && !in_quotes) {
            if (!current.empty()) {
                args.push_back(current);
                current.clear();
            }
        } else if (c != '\r' && c != '\n') {
            current += c;
        }
    }
    if (!current.empty())
        args.push_back(current);
    return args;
}

    std::string extract_command_name(const std::string& arg) {
        if (!arg.empty() && arg[0] == '/')
            return arg.substr(1);
        return arg;
    }

    std::string recompose_args(const std::vector<std::string>& args) {
        std::string recomposed;
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0)
                recomposed += " ";
            if (args[i].find(' ') != std::string::npos)
                recomposed += '"' + args[i] + '"';
            else
                recomposed += args[i];
        }
        return recomposed;
    }


void CommandHandler::process_command(const std::string& command, Cli::Socket& socket, Cli::Context& context)
{
    std::vector<std::string> args = split_args(command);
    if (args.empty())
        return;

    std::string cmd_name = extract_command_name(args[0]);
    auto it = _commands.find(cmd_name);
    if (it != _commands.end()) {
        execute_command(cmd_name, command, socket, context);
    } else {
        std::cout << "Unknown command" << std::endl;
    }
}
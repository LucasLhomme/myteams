/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** commands registry and parser
*/

#include "server/server.hpp"
#include <regex>

void Server::init_commands()
{
    _command_map["/login"] = &Server::cmd_login;
    _command_map["/logout"] = &Server::cmd_logout;
    _command_map["/users"] = &Server::cmd_users;
    _command_map["/user"] = &Server::cmd_user;
    _command_map["/send"] = &Server::cmd_send;
    _command_map["/messages"] = &Server::cmd_messages;
    _command_map["/subscribe"] = &Server::cmd_subscribe;
    _command_map["/subscribed"] = &Server::cmd_subscribed;
    _command_map["/unsubscribe"] = &Server::cmd_unsubscribe;
    _command_map["/help"] = &Server::cmd_help;
    _command_map["exit"] = &Server::cmd_logout;
}

void Server::process_command(int client_socket, const std::string &command)
{
    if (command.empty()) return;

    std::vector<std::string> args;
    // this regex is for parsing arguments with quotes
    std::regex token_regex(R"(("[^"]*"|\S+))");
    auto it = std::sregex_iterator(command.begin(), command.end(), token_regex);
    auto end = std::sregex_iterator();

    for (; it != end; ++it) {
        std::string token = it->str();
        if (token.size() >= 2 && token.front() == '"' && token.back() == '"') {
            token = token.substr(1, token.size() - 2);
        }
        args.push_back(token);
    }
    if (args.empty())
        return;
    
    std::string cmd_name = args[0];
    auto map_it = _command_map.find(cmd_name);
    if (map_it != _command_map.end()) {
        (this->*(map_it->second))(client_socket, args);
    } else {
        send_response(client_socket, "Unknown command");
    }
}

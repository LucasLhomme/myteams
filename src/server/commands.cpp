/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** commands registry and parser
*/

#include "server/server.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <cctype>

static bool is_public_command(const std::string &command)
{
  return command == "/login" || command == "/help";
}

void Server::extract_and_process_commands(int client_socket) {
  auto &buffer = _client_buffers[client_socket];
  size_t pos;

  while ((pos = buffer.find("\r\n")) != std::string::npos || (pos = buffer.find("\n")) != std::string::npos) {
    size_t newline_len = (buffer[pos] == '\r') ? 2 : 1;
    std::string line = buffer.substr(0, pos);
    buffer.erase(0, pos + newline_len);

    if (line.empty()) continue;

    std::vector<std::string> args;
    std::string current;
    bool in_quotes = false;
    bool last_was_quoted = false;
    std::vector<bool> quoted_args;

    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];
        if (c == '"') {
            if (in_quotes) {
                args.push_back(current);
                quoted_args.push_back(true);
                current.clear();
                in_quotes = false;
            } else {
                if (!current.empty()) {
                    args.push_back(current);
                    quoted_args.push_back(false);
                    current.clear();
                }
                in_quotes = true;
            }
        } else if (std::isspace(c) && !in_quotes) {
            if (!current.empty()) {
                args.push_back(current);
                quoted_args.push_back(false);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    if (in_quotes) {
        send_response(client_socket, "ERROR: Missing closing quote");
        continue;
    }
    if (!current.empty()) {
        args.push_back(current);
        quoted_args.push_back(false);
    }

    if (args.empty()) continue;

    // Standard Protocol: ALL command arguments MUST be quoted
    bool all_quoted = true;
    for (size_t i = 1; i < args.size(); ++i) {
        if (!quoted_args[i]) {
            send_response(client_socket, "ERROR: All arguments must be quoted");
            all_quoted = false;
            break;
        }
    }
    if (!all_quoted) continue;

    std::cout << "Processing: " << args[0] << std::endl;
    
    const bool is_logged_in = (_socket_to_uuid.find(client_socket) != _socket_to_uuid.end());

    if (!is_logged_in && !is_public_command(args[0])) {
        send_response(client_socket, "ERROR: Unauthorized. You must be logged in.");
        continue;
    }
        
    auto map_it = _command_map.find(args[0]);

    try {
        auto generic_cmd = _command_factory.get_command(args[0]);
        if (generic_cmd) {
            generic_cmd->execute(*this, client_socket, args);
        } else if (map_it != _command_map.end()) {
          (this->*(map_it->second))(client_socket, args);
        } else {
          send_response(client_socket, "ERROR: Unknown command: " + args[0]);
        }
    } catch (const std::exception &e) {
        std::cout << "[ERROR] Exception during '" << args[0] << "': " << e.what() << std::endl;
        send_response(client_socket, "ERROR: Internal error");
    }
  }
}

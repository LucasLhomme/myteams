/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** messages
*/

#include "../../../include/cli/commands/messages.hpp"
#include <iostream>
#include <sstream>
#include <cctype>

extern "C" {
#include "../../../libs/myteams/logging_client.h"
}

static bool parse_messages_line(const std::string &line,
    std::string &sender_uuid, time_t &timestamp, std::string &message)
{
    const size_t arrow_pos = line.find(" -> ");
    const size_t open_bracket = line.find('[', arrow_pos == std::string::npos ? 0 : arrow_pos + 4);
    const size_t close_bracket = line.find(']', open_bracket == std::string::npos ? 0 : open_bracket + 1);
    const size_t colon_pos = line.find(": ", close_bracket == std::string::npos ? 0 : close_bracket + 1);

    if (arrow_pos != std::string::npos &&
        open_bracket != std::string::npos &&
        close_bracket != std::string::npos &&
        colon_pos != std::string::npos) {
        sender_uuid = line.substr(0, arrow_pos);
        const std::string timestamp_str = line.substr(open_bracket + 1, close_bracket - open_bracket - 1);

        if (sender_uuid.empty() || timestamp_str.empty()) {
            return false;
        }
        for (char c : timestamp_str) {
            if (!std::isdigit(static_cast<unsigned char>(c))) {
                return false;
            }
        }
        timestamp = static_cast<time_t>(std::stoll(timestamp_str));
        message = line.substr(colon_pos + 2);
        return true;
    }

    std::istringstream entry(line);
    std::string timestamp_str;

    entry >> sender_uuid >> timestamp_str;
    if (sender_uuid.empty() || timestamp_str.empty()) {
        return false;
    }
    for (char c : timestamp_str) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    timestamp = static_cast<time_t>(std::stoll(timestamp_str));
    std::getline(entry, message);
    const size_t start = message.find_first_not_of(' ');
    message = (start == std::string::npos) ? "" : message.substr(start);
    return true;
}

void MessagesCommand::execute(const std::string &input, Cli::Socket &socket,
                              [[maybe_unused]] Cli::Context &context) {
  try {
    const std::vector<std::string> args = split_args(input);

    if (args.size() != 2) {
      std::cout << "Usage: /messages [\"user_uuid\"]" << std::endl;
      return;
    }

    socket.send_data(input);
    while (true) {
      std::string line = CommandHandler::receive_response(socket, 500);
      if (line.empty())
        break;
      if (line == "\r" || line == "\n" || line == "")
        continue;

      if (line.find("ERROR_UNKNOWN_USER:") == 0) {
        std::string uuid = line.substr(19);
        uuid.erase(uuid.find_last_not_of(" \n\r\t") + 1);
        client_error_unknown_user(uuid.c_str());
        return;
      }

      if (line.find("ERROR") != std::string::npos) {
        if (line.find("Unauthorized") != std::string::npos) {
          client_error_unauthorized();
        } else {
          std::cout << line << std::endl;
        }
        return;
      }

      if (line.find("No message") != std::string::npos) {
        continue;
      }

        std::string sender_uuid;
        std::string message;
        time_t timestamp = 0;

        if (parse_messages_line(line, sender_uuid, timestamp, message)) {
            client_private_message_print_messages(sender_uuid.c_str(), timestamp, message.c_str());
        }
    }

    } catch (const std::exception &e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
    }}
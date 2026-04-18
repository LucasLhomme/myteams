/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** create
*/

#include "../../../include/cli/commands/create.hpp"
#include <iostream>
#include <sstream>
#include <ctime>

extern "C" {
#include "../../../libs/myteams/logging_client.h"
}

static std::string trim_copy(const std::string &value)
{
    const size_t first = value.find_first_not_of(" \n\r\t");
    if (first == std::string::npos) return "";
    const size_t last = value.find_last_not_of(" \n\r\t");
    return value.substr(first, last - first + 1);
}

void CreateCommand::execute(const std::string &input, Cli::Socket &socket,
                           Cli::Context &context) {
  try {
    const std::vector<std::string> args = split_args(input);

    if (context.get_team_uuid().empty()) {
      if (args.size() != 3) {
        std::cout << "Usage: /create [\"team_name\"] [\"team_description\"]"
                  << std::endl;
        return;
      }
    } else if (context.get_channel_uuid().empty()) {
      if (args.size() != 3) {
        std::cout << "Usage: /create [\"channel_name\"] [\"channel_description\"]"
                  << std::endl;
        return;
      }
    } else if (context.get_thread_uuid().empty()) {
      if (args.size() != 3) {
        std::cout << "Usage: /create [\"thread_title\"] [\"thread_message\"]"
                  << std::endl;
        return;
      }
    } else {
      if (args.size() != 2) {
        std::cout << "Usage: /create [\"comment_body\"]" << std::endl;
        return;
      }
    }

    socket.send_data(input);
    std::string response = socket.receive_data();

    bool is_error = (response.find("ERROR") != std::string::npos ||
                     response.find("Error") != std::string::npos);

    if (is_error) {
      if (response.find("UNKNOWN_TEAM") != std::string::npos) {
        size_t pos = response.find(":") + 1;
        client_error_unknown_team(trim_copy(response.substr(pos)).c_str());
      } else if (response.find("UNKNOWN_CHANNEL") != std::string::npos) {
        size_t pos = response.find(":") + 1;
        client_error_unknown_channel(trim_copy(response.substr(pos)).c_str());
      } else if (response.find("UNKNOWN_THREAD") != std::string::npos) {
        size_t pos = response.find(":") + 1;
        client_error_unknown_thread(trim_copy(response.substr(pos)).c_str());
      } else if (response.find("ALREADY_EXIST") != std::string::npos) {
        client_error_already_exist();
      } else if (response.find("Unauthorized") != std::string::npos ||
                 response.find("subscribe") != std::string::npos) {
        client_error_unauthorized();
      } else {
        std::cout << response << std::endl;
      }
      return;
    }

    if (args.size() < 2) return;
    const std::string &arg1 = args[1];
    const std::string arg2 = args.size() > 2 ? args[2] : std::string();

    if (response.find("Team created") != std::string::npos) {
        size_t pos = response.find(": ") + 2;
        client_print_team_created(trim_copy(response.substr(pos)).c_str(), arg1.c_str(), arg2.c_str());
    } else if (response.find("Channel created") != std::string::npos) {
        size_t pos = response.find(": ") + 2;
        client_print_channel_created(trim_copy(response.substr(pos)).c_str(), arg1.c_str(), arg2.c_str());
    } else if (response.find("Thread created") != std::string::npos) {
        size_t pos = response.find(": ") + 2;
        std::istringstream ds(response.substr(pos));
        std::string tu, uu, ts_str;
        ds >> tu >> uu >> ts_str;
        if (!tu.empty() && !uu.empty() && !ts_str.empty())
            client_print_thread_created(tu.c_str(), uu.c_str(), (time_t)std::stoll(ts_str), arg1.c_str(), arg2.c_str());
    } else if (response.find("Reply created") != std::string::npos) {
        size_t pos = response.find(": ") + 2;
        std::istringstream ds(response.substr(pos));
        std::string ru, uu, ts_str;
        ds >> ru >> uu >> ts_str;
        if (!ru.empty() && !uu.empty() && !ts_str.empty())
            client_print_reply_created(context.get_thread_uuid().c_str(), uu.c_str(), (time_t)std::stoll(ts_str), arg1.c_str());
    }

    } catch (const std::exception &e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
    }}
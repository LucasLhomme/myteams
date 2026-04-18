/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** subscribe
*/

#include "../../../include/cli/commands/subscribe.hpp"
#include <iostream>

extern "C" {
#include "../../../libs/myteams/logging_client.h"
}

static std::string trim_copy(const std::string &value)
{
    const size_t first = value.find_first_not_of(" \n\r\t");

    if (first == std::string::npos) {
        return "";
    }
    const size_t last = value.find_last_not_of(" \n\r\t");

    return value.substr(first, last - first + 1);
}

void SubscribeCommand::execute(const std::string &input, Cli::Socket &socket,
                                Cli::Context &context) {
  try {
    const std::vector<std::string> args = split_args(input);

    if (args.size() != 2) {
      std::cout << "Usage: /subscribe [\"team_uuid\"]" << std::endl;
      return;
    }

    socket.send_data(input);
    std::string response = socket.receive_data();

    if (response.find("ERROR_UNKNOWN_TEAM:") == 0) {
      std::string uuid =
          trim_copy(response.substr(std::string("ERROR_UNKNOWN_TEAM:").size()));
      client_error_unknown_team(uuid.c_str());
    } else if (response.find("ERROR") != std::string::npos) {
      std::cout << response << std::endl;
    } else {
        std::string clean_resp = response;
        if (clean_resp.find("Success ") == 0)
            clean_resp = clean_resp.substr(8);
        
        size_t space_pos = clean_resp.find(' ');
        if (space_pos != std::string::npos) {
            std::string user_uuid = clean_resp.substr(0, space_pos);
            std::string team_uuid = clean_resp.substr(space_pos + 1);
            team_uuid.erase(team_uuid.find_last_not_of(" \n\r\t") + 1);
            client_print_subscribed(user_uuid.c_str(), team_uuid.c_str());
        }
    }

    } catch (const std::exception &e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
    }}
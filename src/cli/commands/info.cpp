#include <iostream>
/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** info
*/

#include "../../../include/cli/commands/info.hpp"
#include <sstream>
#include <vector>

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

void InfoCommand::execute(const std::string &input, Cli::Socket &socket,
                         [[maybe_unused]] Cli::Context &context) {
  try {
    const std::vector<std::string> args = split_args(input);

    if (args.size() != 1) {
      std::cout << "Usage: /info" << std::endl;
      return;
    }

    (void)context;
    socket.send_data(input);
    std::string response = socket.receive_data();

    if (response.find("ERROR_UNKNOWN_TEAM:") == 0) {
        std::string uuid = trim_copy(response.substr(std::string("ERROR_UNKNOWN_TEAM:").size()));
        client_error_unknown_team(uuid.c_str());
        return;
    }
    if (response.find("ERROR_UNKNOWN_CHANNEL:") == 0) {
        std::string uuid = trim_copy(response.substr(std::string("ERROR_UNKNOWN_CHANNEL:").size()));
        client_error_unknown_channel(uuid.c_str());
        return;
    }
    if (response.find("ERROR_UNKNOWN_THREAD:") == 0) {
        std::string uuid = trim_copy(response.substr(std::string("ERROR_UNKNOWN_THREAD:").size()));
        client_error_unknown_thread(uuid.c_str());
        return;
    }
    if (response.find("ERROR") != std::string::npos) {
        client_error_unauthorized();
        return;
    }

    const std::vector<std::string> fields = split_tab_fields(response);
    if (fields.empty()) {
        return;
    }

    if (fields[0] == "USER" && fields.size() >= 4) {
        const int status = (fields[3] == "1" || fields[3] == "online") ? 1 : 0;
        client_print_user(fields[1].c_str(), fields[2].c_str(), status);
    } else if (fields[0] == "TEAM" && fields.size() >= 4) {
        client_print_team(fields[1].c_str(), fields[2].c_str(), fields[3].c_str());
    } else if (fields[0] == "CHANNEL" && fields.size() >= 4) {
        client_print_channel(fields[1].c_str(), fields[2].c_str(), fields[3].c_str());
    } else if (fields[0] == "THREAD" && fields.size() >= 6) {
        const time_t timestamp = static_cast<time_t>(std::stoll(fields[3]));
        client_print_thread(fields[1].c_str(), fields[2].c_str(), timestamp,
            fields[4].c_str(), fields[5].c_str());
    }

    } catch (const std::exception &e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
    }}
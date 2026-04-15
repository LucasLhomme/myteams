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

    if (first == std::string::npos) {
        return "";
    }
    const size_t last = value.find_last_not_of(" \n\r\t");

    return value.substr(first, last - first + 1);
}

void CreateCommand::execute(const std::string& input, Cli::Socket& socket, Cli::Context& context)
{
    try {
    const std::vector<std::string> args = split_args(input);

    socket.send_data(input + "\n");
    std::string response = socket.receive_data();
    
    if (response.find("ERROR_UNKNOWN_TEAM:") == 0) {
        std::string uuid = trim_copy(response.substr(std::string("ERROR_UNKNOWN_TEAM:").size()));
        client_error_unknown_team(uuid.c_str());
    } else if (response.find("ERROR_UNKNOWN_CHANNEL:") == 0) {
        std::string uuid = trim_copy(response.substr(std::string("ERROR_UNKNOWN_CHANNEL:").size()));
        client_error_unknown_channel(uuid.c_str());
    } else if (response.find("ERROR_UNKNOWN_THREAD:") == 0) {
        std::string uuid = trim_copy(response.substr(std::string("ERROR_UNKNOWN_THREAD:").size()));
        client_error_unknown_thread(uuid.c_str());
    } else if (response.find("ERROR_ALREADY_EXIST") != std::string::npos) {
        client_error_already_exist();
    } else if (response.find("ERROR") != std::string::npos) {
        client_error_unauthorized();
    } else {
        if (args.size() < 2) {
            return;
        }

        const std::string &arg1 = args[1];
        const std::string arg2 = args.size() > 2 ? args[2] : std::string();

        if (response.find("Team created") != std::string::npos) {
            size_t pos = response.find(": ") + 2;
            std::string uuid = response.substr(pos);
            uuid.erase(uuid.find_last_not_of(" \n\r\t") + 1);
            client_print_team_created(uuid.c_str(), arg1.c_str(), arg2.c_str());
        } else if (response.find("Channel created") != std::string::npos) {
            size_t pos = response.find(": ") + 2;
            std::string uuid = response.substr(pos);
            uuid.erase(uuid.find_last_not_of(" \n\r\t") + 1);
            client_print_channel_created(uuid.c_str(), arg1.c_str(), arg2.c_str());
        } else if (response.find("Thread created") != std::string::npos) {
            size_t pos = response.find(": ") + 2;
            std::string rest = response.substr(pos);
            std::istringstream data_stream(rest);
            std::string thread_uuid, user_uuid, timestamp_str;
            data_stream >> thread_uuid >> user_uuid >> timestamp_str;
            
            if (!thread_uuid.empty() && !user_uuid.empty() && !timestamp_str.empty()) {
                time_t timestamp = std::stol(timestamp_str);
                client_print_thread_created(thread_uuid.c_str(), user_uuid.c_str(), 
                    timestamp, arg1.c_str(), arg2.c_str());
            }
        } else if (response.find("Reply created") != std::string::npos) {
            size_t pos = response.find(": ") + 2;
            std::string rest = response.substr(pos);
            std::istringstream data_stream(rest);
            std::string reply_uuid, user_uuid, timestamp_str;
            data_stream >> reply_uuid >> user_uuid >> timestamp_str;
            
            if (!reply_uuid.empty() && !user_uuid.empty() && !timestamp_str.empty()) {
                time_t timestamp = std::stol(timestamp_str);
                std::string thread_uuid = context.get_thread_uuid();
                std::string team_uuid = context.get_team_uuid();
                
                if (!thread_uuid.empty()) {
                    client_print_reply_created(thread_uuid.c_str(), user_uuid.c_str(), 
                        timestamp, arg1.c_str());
                }
            }
        }
    }

    } catch (const std::exception &e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
    }}
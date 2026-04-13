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

void CreateCommand::execute(const std::string& input, Cli::Socket& socket, Cli::Context& context)
{
    socket.send_data(input + "\n");
    std::string response = socket.receive_data();
    
    if (response.find("ERROR_UNKNOWN_TEAM:") == 0) {
        std::string uuid = response.substr(18);
        uuid.erase(uuid.find_last_not_of(" \n\r\t") + 1);
        client_error_unknown_team(uuid.c_str());
    } else if (response.find("ERROR_UNKNOWN_CHANNEL:") == 0) {
        std::string uuid = response.substr(21);
        uuid.erase(uuid.find_last_not_of(" \n\r\t") + 1);
        client_error_unknown_channel(uuid.c_str());
    } else if (response.find("ERROR_UNKNOWN_THREAD:") == 0) {
        std::string uuid = response.substr(20);
        uuid.erase(uuid.find_last_not_of(" \n\r\t") + 1);
        client_error_unknown_thread(uuid.c_str());
    } else if (response.find("ERROR_ALREADY_EXIST") != std::string::npos) {
        client_error_already_exist();
    } else if (response.find("ERROR") != std::string::npos) {
        client_error_unauthorized();
    } else {
        std::istringstream iss(input);
        std::string cmd, arg1, arg2;
        iss >> cmd >> arg1 >> arg2;
        if (!arg1.empty() && arg1[0] == '"') arg1.erase(0, 1);
        if (!arg1.empty() && arg1[arg1.length()-1] == '"') arg1.erase(arg1.length()-1);
        if (!arg2.empty() && arg2[0] == '"') arg2.erase(0, 1);
        if (!arg2.empty() && arg2[arg2.length()-1] == '"') arg2.erase(arg2.length()-1);
        
        if (response.find("Team created") != std::string::npos) {
            size_t pos = response.find(": ") + 2;
            std::string uuid = response.substr(pos);
            uuid.erase(uuid.find_last_not_of(" \n\r\t") + 1);
            client_print_team_created(uuid.c_str(), arg1.c_str(), arg2.c_str());
            client_event_team_created(uuid.c_str(), arg1.c_str(), arg2.c_str());
        } else if (response.find("Channel created") != std::string::npos) {
            size_t pos = response.find(": ") + 2;
            std::string uuid = response.substr(pos);
            uuid.erase(uuid.find_last_not_of(" \n\r\t") + 1);
            client_print_channel_created(uuid.c_str(), arg1.c_str(), arg2.c_str());
            client_event_channel_created(uuid.c_str(), arg1.c_str(), arg2.c_str());
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
                    if (!team_uuid.empty()) {
                        client_event_thread_reply_received(team_uuid.c_str(), thread_uuid.c_str(),
                            user_uuid.c_str(), arg1.c_str());
                    }
                }
            }
        }
    }
}
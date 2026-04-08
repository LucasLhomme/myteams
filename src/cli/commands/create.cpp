/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** create
*/

#include "../../../include/cli/commands/create.hpp"
#include <iostream>
#include <sstream>

extern "C" {
#include "../../../libs/myteams/logging_client.h"
}

void CreateCommand::execute(const std::string& input, Cli::Socket& socket)
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
        if (response.find("Team created") != std::string::npos) {
            size_t pos = response.find(": ") + 2;
            std::string uuid = response.substr(pos);
            uuid.erase(uuid.find_last_not_of(" \n\r\t") + 1);
        } else if (response.find("Channel created") != std::string::npos) {
            size_t pos = response.find(": ") + 2;
            std::string uuid = response.substr(pos);
            uuid.erase(uuid.find_last_not_of(" \n\r\t") + 1);
            //client_print_channel_created(uuid.c_str(), name.c_str(), desc.c_str());
        } else if (response.find("Thread created") != std::string::npos) {
            size_t pos = response.find(": ") + 2;
            std::string uuid = response.substr(pos);
            uuid.erase(uuid.find_last_not_of(" \n\r\t") + 1);
            //client_print_thread_created(uuid.c_str(), user_uuid.c_str(), timestamp, title.c_str(), body.c_str());
        } else if (response.find("Reply created") != std::string::npos) {
            //client_print_reply_created(thread_uuid.c_str(), user_uuid.c_str(), timestamp, body.c_str());
        }
    }
    
    std::cout << response << std::endl;
}
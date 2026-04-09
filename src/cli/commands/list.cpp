/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** list
*/

#include "../../../include/cli/commands/list.hpp"
#include <iostream>
#include <sstream>

extern "C" {
#include "../../../libs/myteams/logging_client.h"
}

void ListCommand::execute(const std::string& input, Cli::Socket& socket)
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
    } else if (response.find("ERROR") != std::string::npos) {
        client_error_unauthorized();
    } else {
        // Success: parse list entries
        std::istringstream iss(response);
        std::string line;
        while (std::getline(iss, line)) {
            if (line.empty()) continue;
            
            // Detect list type by content
            // Format: TEAM/CHANNEL/THREAD/REPLY uuid name [description] [timestamp]
            std::istringstream entry(line);
            std::string type, uuid, name;
            entry >> type >> uuid >> name;
            
            if (type == "TEAM") {
                std::string desc;
                std::getline(entry, desc);
                desc.erase(0, desc.find_first_not_of(" "));
                client_print_teams(uuid.c_str(), name.c_str(), desc.c_str());
            } else if (type == "CHANNEL") {
                std::string desc;
                std::getline(entry, desc);
                desc.erase(0, desc.find_first_not_of(" "));
                client_team_print_channels(uuid.c_str(), name.c_str(), desc.c_str());
            } else if (type == "THREAD") {
                std::string user_uuid, body;
                entry >> user_uuid;
                std::getline(entry, body);
                body.erase(0, body.find_first_not_of(" "));
                client_channel_print_threads(uuid.c_str(), user_uuid.c_str(), 0, name.c_str(), body.c_str());
            } else if (type == "REPLY") {
                std::string user_uuid, body;
                entry >> user_uuid;
                std::getline(entry, body);
                body.erase(0, body.find_first_not_of(" "));
                client_thread_print_replies(uuid.c_str(), user_uuid.c_str(), 0, body.c_str());
            }
        }
    }
    
    std::cout << response << std::endl;
}
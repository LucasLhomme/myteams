/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** messages
*/

#include "../../../include/cli/commands/messages.hpp"
#include <iostream>
#include <sstream>

extern "C" {
#include "../../../libs/myteams/logging_client.h"
}

void MessagesCommand::execute(const std::string& input, Cli::Socket& socket, Cli::Context& context)
{
    socket.send_data(input + "\n");
    std::string response = socket.receive_data();
    
    if (response.find("ERROR_UNKNOWN_USER:") == 0) {
        std::string uuid = response.substr(19);
        uuid.erase(uuid.find_last_not_of(" \n\r\t") + 1);
        client_error_unknown_user(uuid.c_str());
        return;
    }

    std::istringstream iss(response);
    std::string line;
    while (std::getline(iss, line)) {
        if (line.empty() || line.find("ERROR") != std::string::npos || line.find("No message") != std::string::npos) continue;
        
        std::istringstream entry(line);
        std::string sender_uuid, timestamp_str, message;
        entry >> sender_uuid >> timestamp_str;
        std::getline(entry, message);
        message.erase(0, message.find_first_not_of(" "));
        
        if (!sender_uuid.empty()) {
            time_t timestamp = std::stol(timestamp_str);
            client_private_message_print_messages(sender_uuid.c_str(), timestamp, message.c_str());
        }
    }
}
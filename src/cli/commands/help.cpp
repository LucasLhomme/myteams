/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** help
*/

#include "../../../include/cli/commands/help.hpp"
#include <iostream>

void HelpCommand::execute(const std::string &input, Cli::Socket &socket,
                         [[maybe_unused]] Cli::Context &context) {
  try {
    const std::vector<std::string> args = split_args(input);

    if (args.size() != 1) {
      std::cout << "Usage: /help" << std::endl;
      return;
    }

    (void)context;
    socket.send_data(input);
    std::string response = socket.receive_data();
    std::cout << response << std::endl;

    } catch (const std::exception &e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
    }}
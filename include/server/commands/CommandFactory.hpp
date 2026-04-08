/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** Command Factory for instantiating commands dynamically 
*/

#ifndef COMMAND_FACTORY_HPP_
#define COMMAND_FACTORY_HPP_

#include <unordered_map>
#include <string>
#include <memory>
#include "ICommand.hpp"

class CommandFactory {
public:
    CommandFactory();
    ~CommandFactory() = default;

    /**
     * @brief Returns the command instance based on its name (e.g. /send, /login)
     * @param name The name of the command
     * @return std::shared_ptr<ICommand> to the command instance, or nullptr if not found
     */
    std::shared_ptr<ICommand> get_command(const std::string &name);

private:
    std::unordered_map<std::string, std::shared_ptr<ICommand>> _commands;
};

#endif /* !COMMAND_FACTORY_HPP_ */
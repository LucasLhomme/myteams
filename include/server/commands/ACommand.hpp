/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** abstract base command for shared logic
*/

#ifndef ACOMMAND_HPP_
#define ACOMMAND_HPP_

#include "ICommand.hpp"
#include "../server.hpp"
#include <string>
#include <stdexcept>

class ACommand : public ICommand {
public:
    virtual ~ACommand() = default;

    virtual void execute(Server &server, int client_socket, const std::vector<std::string> &args) = 0;

protected:
    /*
    ** Shared utility methods
    */
    void check_user_exists(const Server &server, const std::string &uuid_str) const;
};

#endif /* !ACOMMAND_HPP_ */
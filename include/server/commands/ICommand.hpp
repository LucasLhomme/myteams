/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** ICommand interface
*/

#ifndef ICOMMAND_HPP_
#define ICOMMAND_HPP_

#include <vector>
#include <string>

class Server;

class ICommand {
public:
    virtual ~ICommand() = default;

    /**
     * @brief Executes the business logic of the command
     * @param server Reference to the server to access data (users, sockets, etc.)
     * @param client_socket The socket of the client that issued the command
     * @param args The arguments passed to the command
     */
    virtual void execute(Server &server, int client_socket, const std::vector<std::string> &args) = 0;
};

#endif /* !ICOMMAND_HPP_ */
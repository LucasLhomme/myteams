/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** server
*/

#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <string>
#include <vector>
#include <poll.h>

class Server {
public:
    Server(int port);
    ~Server();

    void start();

private:
    int _port;
    int _server_socket;
    std::vector<int> _clients;

    bool init_socket();
    bool bind_socket();
    bool listen_socket();
    int accept_client();
    std::string read_command(int client_socket);
    void handle_command(int client_socket, const std::string &command);
    void send(int client_socket, const std::string &message);
    void cleanup();
    
    int setup_poll_fds(struct pollfd *fds);
    void handle_client_disconnect(int client_fd);
    void handle_clients_input(struct pollfd *fds);
};

#endif /* !SERVER_HPP_ */

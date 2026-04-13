/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** server with clean poll-based architecture
*/

#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <poll.h>
#include <string>
#include <sys/queue.h>
#include <unordered_map>
#include <uuid/uuid.h>
#include <vector>

#include "constants.hpp"
#include "user.hpp"
#include "team.hpp"
#include "commands/CommandFactory.hpp"

/**
 * @class Server
 * @brief Main server class handling multi-client connections with poll-based I/O
 * 
 * Manages:
 * - Accepting client connections
 * - Reading and writing data with poll()
 * - Processing commands per client
 * - Maintaining client state and context
 * - Persisting data to disk
 */
class Server {
public:
    /**
     * @brief Construct server with listening port
     * @param port Port to listen on
     */
    explicit Server(int port);
    ~Server();

    /**
     * @brief Start the server main loop with poll()
     * @return 0 on success, 84 on error
     */
    int start();

    // Data accessors
    const std::vector<User *> &get_users() const { return _users; }
    std::vector<User *> &get_users() { return _users; }

    const std::vector<Team *> &get_teams() const { return _teams; }
    std::vector<Team *> &get_teams() { return _teams; }

    const std::vector<Channel *> &get_channels() const { return _channels; }
    std::vector<Channel *> &get_channels() { return _channels; }

    const std::vector<Thread *> &get_threads() const { return _threads; }
    std::vector<Thread *> &get_threads() { return _threads; }

    const std::vector<Reply *> &get_replies() const { return _replies; }
    std::vector<Reply *> &get_replies() { return _replies; }

    const std::vector<PrivateMessage *> &get_private_messages() const
    {
      return _private_messages;
    }
    std::vector<PrivateMessage *> &get_private_messages() { return _private_messages; }

    // Client context accessors
    std::unordered_map<int, std::string> &get_socket_to_name() { return _socket_to_name; }
    std::unordered_map<int, std::string> &get_socket_to_uuid() { return _socket_to_uuid; }
    std::unordered_map<int, std::string> &get_use_team_context() { return _use_team_context; }
    std::unordered_map<int, std::string> &get_use_channel_context() { return _use_channel_context; }
    std::unordered_map<int, std::string> &get_use_thread_context() { return _use_thread_context; }

    // Response handling
    /**
     * @brief Queue a response to send to a client
     * @param client_socket Socket to send to
     * @param message Message to send
     */
    void send_response(int client_socket, const std::string &message);

    // Persistence
    void save_state();

    // Utilities
    std::vector<std::string> get_all_usernames();
    void GetUserInfo(int client_socket, const std::string &uuid_str);

private:
    //  Socket Management 
    int _port;
    int _server_socket{-1};
    
    /**
     * @brief Initialize server socket with SO_REUSEADDR
     * @return true if successful, false otherwise
     */
    bool init_socket();

    /**
     * @brief Bind server socket to listening port
     * @return true if successful, false otherwise
     */
    bool bind_socket();

    /**
     * @brief Start listening for incoming connections
     * @return true if successful, false otherwise
     */
    bool listen_socket();

    /**
     * @brief Create a pollfd structure for a file descriptor
     * @param fd File descriptor to monitor
     * @return Initialized pollfd structure with POLLIN event
     */
    struct pollfd make_pollfd(int fd);

    /**
     * @brief Accept incoming client connection
     */
    void accept_client();

    // I/O Polling
    std::vector<struct pollfd> _pollfds;
    std::unordered_map<int, std::string> _client_buffers;
      std::unordered_map<int, std::string> _output_buffers;

    /**
     * @brief Handle readable socket (incoming data)
     * @param poll_index Index in _pollfds array
     * @return true if client was removed, false otherwise
     */
    bool handle_client_data(size_t poll_index);

    /**
     * @brief Handle writable socket (outgoing data)
     * @param index Index in _pollfds array
     * @return true if client was removed, false otherwise
     */
    bool handle_writable(size_t index);

    /**
     * @brief Remove client from all tracking structures
     * @param client_socket Socket to remove
     */
    void remove_client(int client_socket);

    /**
     * @brief Clean up all resources
     */
    void cleanup();

    // Command Processing 
    /**
     * @brief Extract and process complete commands from client buffer
     * @param client_socket Socket of the client
     */
    void extract_and_process_commands(int client_socket);

    //  Client Context Management 
    /// Maps client socket to username
    std::unordered_map<int, std::string> _socket_to_name;
    /// Maps client socket to user UUID
    std::unordered_map<int, std::string> _socket_to_uuid;
    /// Maps client socket to selected team UUID
    std::unordered_map<int, std::string> _use_team_context;
    /// Maps client socket to selected channel UUID
    std::unordered_map<int, std::string> _use_channel_context;
    /// Maps client socket to selected thread UUID
    std::unordered_map<int, std::string> _use_thread_context;

    //  Data Storage
    std::vector<User *> _users;
    std::vector<Team *> _teams;
    std::vector<Channel *> _channels;
    std::vector<Thread *> _threads;
    std::vector<Reply *> _replies;
    std::vector<PrivateMessage *> _private_messages;

    //  Command Handling 
    using CommandHandler = void (Server::*)(int, const std::vector<std::string> &);
    std::unordered_map<std::string, CommandHandler> _command_map;
    CommandFactory _command_factory;

    //  Persistence 
    /**
     * @brief Load saved state from disk
     */
    void load_state();
};

#endif /* !SERVER_HPP_ */

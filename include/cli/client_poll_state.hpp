/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** client poll state
*/

#ifndef CLIENT_POLL_STATE_HPP_
#define CLIENT_POLL_STATE_HPP_

#include <array>
#include <poll.h>

namespace Cli {
    struct ClientPollState {
        pollfd stdin_fd{};
        pollfd server_fd{};

        explicit ClientPollState(int server_sockfd);

        std::array<pollfd, 2> to_array() const;
    };
}

#endif /* !CLIENT_POLL_STATE_HPP_ */
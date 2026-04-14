/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** server_exception
*/

#ifndef SERVER_EXCEPTION_HPP_
#define SERVER_EXCEPTION_HPP_

#include "../exception.hpp"

class ServerException : public Exception {
    public:
        ServerException(const std::string &message) : Exception(message) {}
};

class SubscriptionException : public ServerException {
    public:
        SubscriptionException(const std::string &message) : ServerException(message) {}
};

#endif /* !SERVER_EXCEPTION_HPP_ */

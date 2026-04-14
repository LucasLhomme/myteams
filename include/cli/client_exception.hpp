/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** client_exception
*/

#ifndef CLIENT_EXCEPTION_HPP_
#define CLIENT_EXCEPTION_HPP_

#include "../exception.hpp"

class ClientException : public Exception {
    public:
        ClientException(const std::string &message) : Exception(message) {}
};

#endif /* !CLIENT_EXCEPTION_HPP_ */

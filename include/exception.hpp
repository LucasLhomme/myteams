/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** exception
*/

#ifndef EXCEPTION_HPP_
#define EXCEPTION_HPP_

#include <exception>
#include <string>

class Exception : public std::exception {
    public:
        Exception(const std::string& message) : _message(message) {}
        const char* what() const noexcept override { return _message.c_str(); }
    private:
        std::string _message;
};

#endif /* !EXCEPTION_HPP_ */

/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** user
*/

#ifndef USER_HPP_
#define USER_HPP_

#include <uuid/uuid.h>
#include "constants.hpp"

struct User {
  uuid_t uuid;
  char username[MAX_NAME_LENGTH];
  bool is_online;
};

#endif /* !USER_HPP_ */
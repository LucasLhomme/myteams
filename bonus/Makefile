##
## EPITECH PROJECT, 2026
## MY-TEAMS-1
## File description:
## Makefile for graphical client
##

NAME        =   myteams_gui
SRC         =   src/main.cpp src/socket.cpp
OBJ         =   $(SRC:.cpp=.o)

CXX         =   g++
CXXFLAGS    =   -Wall -Wextra -Iinclude -g $(shell pkg-config --cflags sdl2 SDL2_ttf SDL2_image)
LDFLAGS     =   $(shell pkg-config --libs sdl2 SDL2_ttf SDL2_image)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) -o $(NAME) $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

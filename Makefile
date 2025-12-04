# Makefile for the philo project

NAME = ircserv
CC = c++
CFLAGS = -Wextra -Wall -Werror -std=c++98
SRCS = main.cpp \
       server.cpp \
	   clients.cpp \
	   channel.cpp
OBJS = $(SRCS:.cpp=.o)
RM = rm -f

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re

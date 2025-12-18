# Makefile for the philo project

NAME = ircserv
CC = c++
CFLAGS = -Wextra -Wall -Werror -std=c++98
OBJ_DIR = obj
SRCS = main.cpp \
       server.cpp \
	   clients.cpp \
	   channel.cpp \
	   command.cpp \
	   commands/nickCommand.cpp \
	   commands/userCommand.cpp \
	   commands/passCommand.cpp \
	   commands/joinCommand.cpp \
	   commands/privmsgCommand.cpp \
	   commands/topicCommand.cpp \
	   commands/kickCommand.cpp \
	   commands/quitCommand.cpp \
	   commands/modeCommand.cpp \
	   commands/inviteCommand.cpp \
	   commands/whoCommand.cpp \
	   commands/capCommand.cpp
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
RM = rm -f

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re

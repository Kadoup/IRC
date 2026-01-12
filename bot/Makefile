NAME = boopbot
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g3

SRCS_BONUS = client.cpp irc_client.cpp
OBJS_BONUS = $(SRCS_BONUS:.cpp=.o)

all:
	@echo "Use 'make bonus' to build the IRC bot client."

bonus: $(OBJS_BONUS)
	$(CC) $(CFLAGS) $(OBJS_BONUS) -o $(NAME)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o

fclean: clean
	rm -f $(NAME)

re: fclean bonus

.PHONY: all bonus clean fclean re
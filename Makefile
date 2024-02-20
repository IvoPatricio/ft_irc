SRC				= srcs/main.cpp srcs/server.cpp srcs/client.cpp srcs/sig_utils.cpp

OBJ				= $(SRC:.cpp=.o)

CC				= c++

FLAGS			= -Wall -Wextra -Werror -fsanitize=address -g -std=c++98

NAME			= ircserv

%.o: %.cpp
	$(CC) $(FLAGS) -o $@ -c $<

all:			$(NAME)

$(NAME): $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(NAME)

clean:
				rm -f $(OBJ)

fclean: 		clean
				rm -f $(NAME)

re: 			fclean $(NAME)

test:	all
		./ircserv 8686 abc

.PHONY: all clean fclean re test
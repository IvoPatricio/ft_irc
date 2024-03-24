SRC				= srcs/main.cpp srcs/server.cpp srcs/client.cpp srcs/sig_utils.cpp srcs/channel.cpp srcs/commands.cpp srcs/utils.cpp

OBJ				= $(SRC:.cpp=.o)

CC				= c++

FLAGS			= -fsanitize=address -g -std=c++98

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

test2:	all
		./ircserv 8585 123

test3:	all
		./ircserv 8484 YEY


.PHONY: all clean fclean re test
SRC				= main.cpp server.cpp

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

.PHONY: all clean fclean re
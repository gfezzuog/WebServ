NAME	=	WebServ

CC		=	g++

FLAGS	=	-std=c++98 -Wall -Wextra -Werror

RM		=	rm -rf

SRC		=	./srcs/main.cpp ./srcs/Server.cpp ./srcs/Routes.cpp ./srcs/Config.cpp \
			./srcs/ConfigFile.cpp ./srcs/Client.cpp ./srcs/RequestHeader.cpp  ./srcs/ResponseHeader.cpp \
		

OBJ	=	$(SRC:.cpp=.o)

%.o:%.cpp
			$(CC) $(FLAGS) -c $< -o $@

$(NAME):	$(OBJ)
			$(CC) $(FLAGS) $(SRC) -o $(NAME)
			@echo "WebServ compiled"

all:		$(NAME)

clean:
			${RM} $(OBJ)

fclean: 	clean
			${RM} $(NAME) ${OBJ}

re:			fclean all

.PHONY:		all clean fclean re
NAME=democheck

CC=clang++
FLAGS=-Wall -Werror -Wextra
DEBUG_FLAGS=-g -fsanitize=address
SRC=src/*.cpp out/*.cc
LIBS=-I inc/ -lprotobuf
OBJ=
RM =rm -rf

all: $(NAME)

$(NAME): $(SRC)
	$(CC) $(FLAGS) $(LIBS) $(SRC) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

run: re
	./$(NAME)

debug: fclean
	$(CC) $(FLAGS) $(DEBUG_FLAGS) $(LIBS) $(SRC) -o $(NAME)

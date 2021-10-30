NAME=democheck

CC=clang++
FLAGS=-Wall -Werror -Wextra
DEBUG_FLAGS=-g -fsanitize=address
SRC=src/*.cpp out/*.cc
PROTOSRC=packetmessages.proto
LIBS=-I inc/ -pthread -lprotobuf -lpthread
OBJ=
RM =rm -rf

all: $(NAME)

$(NAME): $(SRC) $(PROTOSRC)
	protoc --cpp_out=out packetmessages.proto 
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

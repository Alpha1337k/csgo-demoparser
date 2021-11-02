NAME=democheck

CC=clang++
FLAGS=
DEBUG_FLAGS=-g -fsanitize=address
SRC=src/*.cpp
PROTOF=out/*.cc
PROTOSRC=packetmessages.proto
LIBS=-I inc/ -pthread -lprotobuf -lpthread
OBJ=
RM =rm -rf

all: $(NAME)

$(NAME): $(SRC) $(PROTOSRC)
	protoc --cpp_out=out packetmessages.proto 
	$(CC) $(FLAGS) $(LIBS) $(SRC) $(PROTOF) -o $(NAME) -std=c++11

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

run: re
	./$(NAME)

debug: fclean
	$(CC) $(FLAGS) $(DEBUG_FLAGS) $(LIBS) $(SRC) -o $(NAME)

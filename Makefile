NAME=democheck
CC=clang++
FLAGS=-pthread -std=c++11
DEBUG_FLAGS=-g -fsanitize=address
LIBS=-I inc/
SRC = src
RM =rm -rf

SOURCES := $(wildcard $(SRC)/*/*.cpp)
SOURCES += $(wildcard $(SRC)/*.cpp)

OBJECTS := $(patsubst %.cpp, %.o, $(SOURCES))


all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(LIBS) -lprotobuf -lpthread $(FLAGS) $(OBJECTS) out/*.cc -o $(NAME)

$(OBJECTS): %.o : %.cpp
	$(CC) $(LIBS) $(FLAGS) -c $< -o $@

clean:
	$(RM) $(OBJECTS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

run: re
	./$(NAME) samples/wingman.dem

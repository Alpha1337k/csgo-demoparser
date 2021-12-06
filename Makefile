NAME=democheck
CC=clang++
FLAGS=-pthread -std=c++11 -Wall -Werror -Wextra -pedantic -g
LINKFLAGS=-lprotobuf -lpthread
DEBUG_FLAGS=-g -fsanitize=address
LIBS=-I inc/ -I startup-parser/src -I protobuf/
SRC = src
RM =rm -rf
MAKESILENT=make --no-print-directory -C

PROTOBUF =protobuf

SOURCES := $(wildcard $(SRC)/*/*.cpp)
SOURCES += $(wildcard $(SRC)/*.cpp)

OBJDIR = obj
OBJECTS := $(patsubst %.cpp, %.o, $(SOURCES))
OBJECTS := $(notdir $(OBJECTS))
OBJECTS := $(addprefix $(OBJDIR)/, $(OBJECTS))

VPATH = $(SRC):$(wildcard $(SRC)/*/)

all: buildproto $(OBJECTS) $(NAME)

buildproto:
	@$(MAKESILENT) $(PROTOBUF)

$(NAME): $(OBJECTS)
	$(CC) $(LIBS) $(LINKFLAGS) $(FLAGS) $(OBJECTS) $(PROTOBUF)/*.o -o $(NAME)

$(OBJDIR)/%.o : %.cpp
	$(CC) $(LIBS) $(FLAGS) -c $< -o $@

$(OBJECTS): | $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)


clean:
	$(RM) $(OBJECTS)
	$(MAKESILENT) $(PROTOBUF) clean

fclean: clean
	$(RM) $(NAME)
	$(MAKESILENT) $(PROTOBUF) fclean

re: fclean all

run: re
	./$(NAME) samples/wingman.dem

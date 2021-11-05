NAME=democheck
CC=clang++
FLAGS=-pthread -std=c++11
DEBUG_FLAGS=-g -fsanitize=address
LIBS=-I inc/
SRC = src
RM =rm -rf

SOURCES := $(wildcard $(SRC)/*/*.cpp)
SOURCES += $(wildcard $(SRC)/*.cpp)

OBJDIR = obj
OBJECTS := $(patsubst %.cpp, %.o, $(SOURCES))
OBJECTS := $(notdir $(OBJECTS))
OBJECTS := $(addprefix $(OBJDIR)/, $(OBJECTS))

VPATH = $(SRC):$(wildcard $(SRC)/*/)

all: $(OBJECTS)
	$(CC) $(LIBS) -lprotobuf -lpthread $(FLAGS) $(OBJECTS) out/*.cc -o $(NAME)

$(OBJDIR)/%.o : %.cpp
	$(CC) $(LIBS) $(FLAGS) -c $< -o $@

$(OBJECTS): | $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)


clean:
	$(RM) $(OBJECTS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

run: re
	./$(NAME) samples/wingman.dem

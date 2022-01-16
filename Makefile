NAME=democheck.a
NAME_EXEC=democheck
CC=clang++
FLAGS= -std=c++2a -Ofast -march=native
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

HEADERS := $(wildcard inc/*.h*)

DEP	:= $(patsubst %.cpp, %.d, $(SOURCES))
DEP := $(notdir $(DEP))
DEP := $(addprefix $(OBJDIR)/, $(DEP))

VPATH = $(SRC):$(wildcard $(SRC)/*/)


all: buildproto
	$(MAKE) -j $(NAME)

buildproto:
	@$(MAKESILENT) $(PROTOBUF)

$(NAME): $(OBJECTS)
	ar rcs $(NAME) $(OBJECTS) $(PROTOBUF)/*.o
	$(CC) $(LIBS) $(LINKFLAGS) $(FLAGS) $(NAME) -o $(NAME_EXEC)

$(OBJDIR)/%.o : %.cpp
	$(CC) $(LIBS) $(FLAGS) -c $< -o $@

$(OBJECTS): | $(OBJDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.d: %.cpp
	$(CC) $< -MM -MF $@ -MT $(OBJDIR)/$*.o $(FLAGS) $(LIBS)

clean:
	$(RM) $(OBJECTS)
	$(MAKESILENT) $(PROTOBUF) clean

fclean: clean
	$(RM) $(NAME) $(NAME_EXEC)
	$(MAKESILENT) $(PROTOBUF) fclean

re: fclean all

run: re
	./$(NAME) samples/wingman.dem

-include $(DEP)
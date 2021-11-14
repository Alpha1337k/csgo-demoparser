SRC := $(wildcard ./*.proto)
CFILES := $(patsubst %.proto, %.pb.cc, $(SRC))
HFILES := $(patsubst %.proto, %.pb.h, $(SRC))
OFILES := $(patsubst %.proto, %.o, $(SRC))
RM =rm -rf
CC = protoc
CLANG = clang++
CFLAGS = -std=c++11
FLAGS = --cpp_out=.

all: build

build: $(CFILES) $(HFILES) $(OFILES)

%.pb.cc : %.proto
	$(CC) $(FLAGS) $<

%.o : %.pb.cc
	$(CLANG) $(CFLAGS) -c $< -o $@


test:
	@echo $(SRC)
	@echo $(CFILES)
	@echo $(HFILES)

clean:
	$(RM) $(OFILES)

fclean: clean
	$(RM) $(CFILES) $(HFILES)

re: fclean all

run: re
	./$(NAME) samples/wingman.dem
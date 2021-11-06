SRC := $(wildcard ./*.proto)
CFILES := $(patsubst %.proto, %.pb.cc, $(SRC))
HFILES := $(patsubst %.proto, %.pb.h, $(SRC))
RM =rm -rf
CC = protoc
FLAGS = --cpp_out=.

all: build

build: $(CFILES) $(HFILES)

%.pb.cc : %.proto
	$(CC) $(FLAGS) $<


test:
	@echo $(SRC)
	@echo $(CFILES)
	@echo $(HFILES)

clean:
	$(RM) $(CFILES) $(HFILES)

fclean: clean

re: fclean all

run: re
	./$(NAME) samples/wingman.dem
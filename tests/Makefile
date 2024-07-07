CC := c++ -std=c++11

DIRSRC:=./src
DIRBUILD:=./build

CSOURCE:=$(shell find $(DIRSRC) -name "*.cpp")
TARGETS:=$(patsubst $(DIRSRC)/%.cpp, $(DIRBUILD)/%.test, $(CSOURCE))
FLAGS:= -O0 -g -Wall -DDEBUG -Wno-reorder-ctor -DTESTS $(shell pkg-config --cflags --libs raylib) -I../

.PHONY: all remake dev clean

all: $(TARGETS)
	@echo "----------- done -----------"

$(DIRBUILD)/%.test : $(DIRBUILD)/%.run
	@echo "----------- test -----------"
	$^
	@rm $^
	@rm -rf $^.dSYM

$(DIRBUILD)/%.run : $(DIRSRC)/%.cpp
	@echo "---------- build -----------"
	$(CC) $(FLAGS) -o $@ $^

remake:
	@echo "---------- remake ----------"
	@make all

dev:
	find ./src -name "*.cpp" | entr make remake

clean:
	@echo "---------- clean -----------"
	rm -rf $(DIRBUILD)
	mkdir -p $(DIRBUILD)

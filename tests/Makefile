CC := c++ -std=c++11

DIRSRC:=./src
DIRBUILD:=./build

CSOURCE:=$(shell find $(DIRSRC) -name "*.cpp")
TARGETS:=$(patsubst $(DIRSRC)/%.cpp, $(DIRBUILD)/%.run, $(CSOURCE))
FLAGS:= -O0 -g -Wall -DDEBUG -Wno-reorder-ctor -DTESTS $(shell pkg-config --cflags --libs raylib) -I../

.PHONY: all remake dev clean

all: $(TARGETS)
	@echo $@
	@echo "Tests => done"

$(DIRBUILD)/%.run : $(DIRSRC)/%.cpp
	@$(shell mkdir -p $(shell dirname $@))
	$(CC) $(FLAGS) -o $@ $^

remake:
	@make clean
	@make all

dev:
	find ./src -name "*.cpp" | entr make remake

clean:
	$(shell rm -rf $(DIRBUILD))
	@echo "Clean => done"

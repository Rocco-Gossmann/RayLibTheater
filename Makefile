CC := c++ -std=c++11

DIRSRC:=.
DIRBUILD:=./build

DEBUGFLAGS:= -O0 -g -Wall -DDEBUG -Wno-reorder-ctor
RELEASEFLAGS:= -O3

CSOURCE:=$(shell find $(DIRSRC) -name "*.cpp")
OBJSRC:=$(patsubst $(DIRSRC)/%.cpp, $(DIRBUILD)/%.o, $(CSOURCE))

RAYCFLAGS:=$(shell pkg-config --cflags raylib)
RAYLFLAGS:=$(shell pkg-config --libs raylib)
FLAGS := 

.PHONY: dev clean remake

remake:
	@make clean
	@make debug.run

debug.run : FLAGS := $(DEBUGFLAGS)
debug.run: $(OBJSRC)
	$(CC) $(RAYLFLAGS) $(FLAGS)  -o $@ $^

release.run : FLAGS := $(RELEASEFLAGS)
release.run: $(OBJSRC)
	$(CC) $(RAYLFLAGS) -o $@ $^

$(DIRBUILD)/%.o: $(DIRSRC)/%.cpp
	mkdir -p $(dir $@)
	$(CC) -g -c $(RAYCFLAGS) $(FLAGS) -o $@ $<

run: debug.run 
	./$<

dev:
	find . -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | entr make remake

clean:
	$(shell rm -rf $(DIRBUILD))
	$(shell rm -f debug.run)
	$(shell rm -f release.run)
	@echo "Clean => done"

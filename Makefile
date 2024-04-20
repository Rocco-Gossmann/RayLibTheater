CC := c++ -std=c++11
TARGET := RayWrapC.run
DIRSRC:=./src
DIRBUILD:=./build

CSOURCE:=$(shell find $(DIRSRC) -name "*.cpp")
OBJSRC:=$(patsubst $(DIRSRC)/%.cpp, $(DIRBUILD)/%.o, $(CSOURCE))

RAYCFLAGS:=$(shell pkg-config --cflags raylib) -I./src
RAYLFLAGS:=$(shell pkg-config --libs --cflags raylib) -I./src

DEBUGFLAGS:= -g -Wall -DDEBUG 
debug.run: $(OBJSRC)
	$(CC) $(RAYLFLAGS) $(DEBUGFLAGS) -o $@ $(OBJSRC)

release.run: $(OBJSRC)
	$(CC) $(RAYLFLAGS) -o $@ $(OBJSRC)

$(DIRBUILD)/%.o: $(DIRSRC)/%.cpp
	mkdir -p $(dir $@)
	$(CC) -g -c $(RAYCFLAGS) -o $@ $<

run: debug.run 
	./$<

.phony: dev clean remake

dev:
	find . -name "*.cpp" -o -name "*.h" -o -name "*.hpp" | entr make remake

remake:
	$(shell rm -rf $(DIRBUILD)/main.o)
	make debug.run

clean:
	$(shell rm -rf $(DIRBUILD))
	$(shell rm -f $(TARGET))
	$(shell rm -f debug.run)


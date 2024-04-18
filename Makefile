CC := c++ -std=c++11
TARGET := RayWrapC.run
DIRSRC:=./src
DIRBUILD:=./build

CSOURCE:=$(shell find $(DIRSRC) -name "*.cpp")
OBJSRC:=$(patsubst $(DIRSRC)/%.cpp, $(DIRBUILD)/%.o, $(CSOURCE))

RAYCFLAGS:=$(shell pkg-config --cflags raylib) -I./src
RAYLFLAGS:=$(shell pkg-config --libs --cflags raylib) -I./src

debug.run: $(OBJSRC)
	$(CC) -g -Wall $(RAYLFLAGS) -o $@ $(OBJSRC)

$(TARGET): $(OBJSRC)
	$(CC) $(RAYLFLAGS) -o $@ $(OBJSRC)


$(DIRBUILD)/%.o: $(DIRSRC)/%.cpp
	mkdir -p $(dir $@)
	$(CC) -g -c $(RAYCFLAGS) -o $@ $<

run: $(TARGET)
	./$<

.phony: dev clean

dev:
	find . -name "*.cpp" -o -name "*.h" | entr make debug.run 

clean:
	$(shell rm -rf $(DIRBUILD))
	$(shell rm -f $(TARGET))
	$(shell rm -f debug.run)


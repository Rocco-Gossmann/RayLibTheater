CC := zig cc
TARGET := RayWrapC.run
DIRSRC:=./src
DIRBUILD:=./build

CSOURCE:=$(shell find $(DIRSRC) -name "*.c")
OBJSRC:=$(patsubst $(DIRSRC)/%.c, $(DIRBUILD)/%.o, $(CSOURCE))

RAYFLAGS:=$(shell pkg-config --libs --cflags raylib)

debug.run: $(OBJSRC)
	$(CC) -g $(RAYFLAGS) -o $@ $(OBJSRC)

$(TARGET): $(OBJSRC)
	$(CC) $(RAYFLAGS) -o $@ $(OBJSRC)


$(DIRBUILD)/%.o: $(DIRSRC)/%.c
	mkdir -p $(dir $@)
	$(CC) -g -c $(RAYFLAGS) -o $@ $<

run: $(TARGET)
	./$<

.phony: dev clean

dev:
	find . -name "*.c" -o -name "*.h" | entr make debug.run 

clean:
	$(shell rm -rf $(DIRBUILD))
	$(shell rm $(TARGET))


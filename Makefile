CC := zig cc
TARGET := RayWrapC.run
DIRSRC:=./src
DIRBUILD:=./build

CSOURCE:=$(shell find $(DIRSRC) -name "*.c")
OBJSRC:=$(patsubst $(DIRSRC)/%.c, $(DIRBUILD)/%.o, $(CSOURCE))


$(TARGET): $(OBJSRC)
	$(CC) -g -o $@ $(OBJSRC)


$(DIRBUILD)/%.o: $(DIRSRC)/%.c
	mkdir -p $(dir $@)
	$(CC) -g -c -o $@ $<

run: $(TARGET)
	./$<

.phony: dev clean

dev:
	find . -name "*.c" -o -name "*.h" | entr make $(TARGET)

clean:
	$(shell rm -rf $(DIRBUILD))
	$(shell rm $(TARGET))


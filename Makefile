# reference: https://makefiletutorial.com/

CC=gcc
CFLAGS=-Wall -O2 -D_FORTIFY_SOURCE=2 -pipe -march=native -Werror=format-security -fpie -fstack-protector-all -fcf-protection -fPIC -fno-plt
LDFLAGS=-Wl,-pie
# Name of our executable
TARGET=toyserv

# Where the binary is stored
BUILD_DIR=build
# Where all the C files are stored
SOURCE_DIR=src

# Find all the C files we want to compile
SRCS=$(shell find $(SOURCE_DIR) -name '*.c')

# Puts all object files into the BUILD_DIR (src/file.c -> build/file.c.o)
OBJS=$(SRCS:%=$(BUILD_DIR)/%.o)

# Puts all dependencis into the BUILD_DIR (build/file.c.o -> build/file.c.d)
DEPS=$(OBJS:.o=.d)

# Any folder in SOURCE_DIR will be included (prefixed with -I)
INC_DIR=$(shell find $(SOURCE_DIR) -type d)
INC_FLAGS=$(addprefix -I,$(INC_DIR))

$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: clean $(BUILD_DIR)/$(TARGET)
	$(BUILD_DIR)/$(TARGET)

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

# include the .d makefiles
-include $(DEPS)
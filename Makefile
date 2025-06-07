# Define the compiler
CC = gcc

# Define compiler flags
CFLAGS = -Wall -Wextra -g -std=c11 -pthread

# Define linker flags
LDFLAGS = -pthread

# Define directories relative to the Makefile's location (tools/)
SRCDIR = src
BUILDDIR = build

# Target executable name
TARGET_EXEC = atc

# Full path to the target executable
TARGET = $(BUILDDIR)/$(TARGET_EXEC)

# Find all .c files in the source directory
SOURCES = $(wildcard $(SRCDIR)/*.c)

# Generate corresponding object file paths in the build directory
# e.g., ../src/atc.c -> ../build/atc.o
# e.g., ../src/utils.c -> ../build/utils.o
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SOURCES))

# --- Main Build Targets ---

# Default target: builds the executable
all: $(TARGET)

# Rule to create the build directory if it doesn't exist
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Rule to build the final executable
$(TARGET): $(OBJECTS) $(BUILDDIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# Rule to compile .c files into .o files in the build directory
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# --- Clean Targets ---

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
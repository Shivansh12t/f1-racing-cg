# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O2 -std=c99 # Use C99 standard
CPPFLAGS = -Iinclude # Preprocessor flags (include paths)
LDFLAGS = -Llib     # Linker flags (library paths)
LDLIBS = -lfreeglut -lglew32 -lopengl32 -lm -lglu32 # Libraries to link (-lm for math)
WINDOWS_LINK_FLAGS = -mwindows # Suppress console window on Windows

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Files
TARGET = f1_racer.exe
SOURCES = $(wildcard $(SRC_DIR)/*.c)
# Replace src/%.c with obj/%.o for object files
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

# Define the executable path
EXECUTABLE = $(BIN_DIR)/$(TARGET)

# Phony targets (targets that don't represent files)
.PHONY: all clean run directories

# Default target: Build everything
all: directories $(EXECUTABLE)
	@echo "Build successful!"
	@echo "Executable: $(EXECUTABLE)"
	@echo "Remember to copy freeglut.dll and glew32.dll to the $(BIN_DIR) directory."

# Rule to create the executable
$(EXECUTABLE): $(OBJECTS)
	@echo "Linking..."
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) $(LDLIBS) $(WINDOWS_LINK_FLAGS)

# Pattern rule to compile .c files into .o files
# $<: name of the first prerequisite (the .c file)
# $@: name of the target (the .o file)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR) # Dependency on obj dir existing
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Rule to create the necessary directories
# Using a phony target and order-only prerequisites for directories
directories: $(OBJ_DIR) $(BIN_DIR)

$(OBJ_DIR):
	@echo "Creating directory: $@"
	@mkdir -p $@

$(BIN_DIR):
	@echo "Creating directory: $@"
	@mkdir -p $@

# Target to clean up generated files
clean:
	@echo "Cleaning up..."
	@rm -f $(OBJ_DIR)/*.o
	@rm -f $(EXECUTABLE)
	@echo "Cleanup complete."
	@echo "Note: Directories $(OBJ_DIR) and $(BIN_DIR) are not removed."
	# Optional: remove directories if empty (might need different commands depending on shell)
	# @-rmdir $(OBJ_DIR) 2>/dev/null || :
	# @-rmdir $(BIN_DIR) 2>/dev/null || :

# Target to build and run the application
run: all
	@echo "Running $(EXECUTABLE)..."
	# Use start to run in background/separate window in MinGW/MSYS
	start $(EXECUTABLE)
	# Alternative for cmd.exe if 'start' doesn't work as expected:
	# cmd /c start $(subst /,\,$(EXECUTABLE))

# Help target (optional)
help:
	@echo "Available targets:"
	@echo "  all      - Build the project (default)"
	@echo "  run      - Build and run the project"
	@echo "  clean    - Remove compiled object files and the executable"
	@echo "  help     - Show this help message"
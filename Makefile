# cmpsh - Custom Shell Implementation
# Professional Makefile with organized directory structure

# Project configuration
PROJECT_NAME = cmpsh
VERSION = 1.1.0

# Directory structure
SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include
TEST_DIR = tests
DOCS_DIR = docs
EXAMPLES_DIR = examples
SCRIPTS_DIR = scripts

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic -g -O2
LDFLAGS = 
INSTALL_DIR = /usr/local/bin

# Source files and objects
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
TARGET = $(BUILD_DIR)/$(PROJECT_NAME)

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@echo "✓ Created build directory"

# Default target
all: $(TARGET)

# Build the main executable
$(TARGET): $(BUILD_DIR) $(OBJECTS)
	@echo "✓ Linking $(PROJECT_NAME)..."
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(TARGET)
	@echo "✓ Built $(PROJECT_NAME) successfully"
	@echo "✓ Executable: $(TARGET)"

# Compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "✓ Compiling $<..."
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Debug build with additional debugging info
debug: CFLAGS += -DDEBUG -g3 -fsanitize=address -fsanitize=undefined
debug: LDFLAGS += -fsanitize=address -fsanitize=undefined
debug: $(TARGET)
	@echo "✓ Built $(PROJECT_NAME) with debug symbols and sanitizers"

# Release build with optimizations
release: CFLAGS += -O3 -DNDEBUG -s
release: $(TARGET)
	@echo "✓ Built $(PROJECT_NAME) optimized for release"

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	@echo "✓ Cleaned build artifacts"

# Create project directory structure
setup-dirs:
	@mkdir -p $(SRC_DIR) $(INCLUDE_DIR) $(TEST_DIR) $(DOCS_DIR) $(EXAMPLES_DIR) $(SCRIPTS_DIR) $(BUILD_DIR)
	@echo "✓ Created project directory structure:"
	@echo "  📁 $(SRC_DIR)/ - Source code files"
	@echo "  📁 $(INCLUDE_DIR)/ - Header files"
	@echo "  📁 $(BUILD_DIR)/ - Build artifacts"
	@echo "  📁 $(TEST_DIR)/ - Test files"
	@echo "  📁 $(DOCS_DIR)/ - Documentation"
	@echo "  📁 $(EXAMPLES_DIR)/ - Example scripts"
	@echo "  📁 $(SCRIPTS_DIR)/ - Build and utility scripts"

# Organize existing files into proper structure
organize: setup-dirs
	@echo "✓ Organizing project files..."
	@# Move source code
	@if [ -f cmpsh.c ]; then mv cmpsh.c $(SRC_DIR)/; echo "  ✓ Moved cmpsh.c to $(SRC_DIR)/"; fi
	@# Move documentation
	@for doc in *.md; do \
		if [ -f "$$doc" ]; then \
			mv "$$doc" $(DOCS_DIR)/; \
			echo "  ✓ Moved $$doc to $(DOCS_DIR)/"; \
		fi; \
	done
	@# Move scripts
	@if [ -f run_tests.sh ]; then mv run_tests.sh $(SCRIPTS_DIR)/; echo "  ✓ Moved run_tests.sh to $(SCRIPTS_DIR)/"; fi
	@# Move example/demo scripts
	@for script in *demo*.sh simple_demo.sh feature_demo.sh; do \
		if [ -f "$$script" ]; then \
			mv "$$script" $(EXAMPLES_DIR)/; \
			echo "  ✓ Moved $$script to $(EXAMPLES_DIR)/"; \
		fi; \
	done
	@# Move test files
	@if [ -d test_scripts ]; then \
		mv test_scripts/* $(TEST_DIR)/ 2>/dev/null || true; \
		rmdir test_scripts 2>/dev/null || true; \
		echo "  ✓ Moved tests to $(TEST_DIR)/"; \
	fi
	@# Update scripts to use new paths
	@if [ -f $(SCRIPTS_DIR)/run_tests.sh ]; then \
		sed -i 's|\\./cmpsh|../build/cmpsh|g' $(SCRIPTS_DIR)/run_tests.sh; \
		echo "  ✓ Updated test script paths"; \
	fi
	@echo "✓ Project organization complete!"

# Run tests
test: $(TARGET)
	@echo "✓ Running test suite..."
	@if [ -f $(SCRIPTS_DIR)/run_tests.sh ]; then \
		cd $(SCRIPTS_DIR) && ./run_tests.sh; \
	else \
		./run_tests.sh $(TARGET); \
	fi

# Run the shell
run: $(TARGET)
	@echo "✓ Starting $(PROJECT_NAME)..."
	@$(TARGET)

# Development build - clean, build, and test
dev: clean all test
	@echo "✓ Development build complete"

# Static analysis using cppcheck (if available)
static-analysis:
	@if command -v cppcheck >/dev/null 2>&1; then \
		echo "✓ Running static analysis..."; \
		cppcheck --enable=all --std=c99 $(SRC_DIR)/; \
	else \
		echo "⚠ cppcheck not found, skipping static analysis"; \
	fi

# Memory leak detection using valgrind (if available)
memcheck: $(TARGET)
	@if command -v valgrind >/dev/null 2>&1; then \
		echo "✓ Running memory leak detection..."; \
		echo "exit" | valgrind --leak-check=full --show-leak-kinds=all $(TARGET); \
	else \
		echo "⚠ valgrind not found, skipping memory check"; \
	fi

# Code formatting (if clang-format available)
format:
	@if command -v clang-format >/dev/null 2>&1; then \
		echo "✓ Formatting code..."; \
		find $(SRC_DIR) -name "*.c" -o -name "*.h" | xargs clang-format -i; \
	else \
		echo "⚠ clang-format not found, skipping formatting"; \
	fi

# Create distribution package
dist: clean $(TARGET)
	@echo "✓ Creating distribution package..."
	@tar -czf $(PROJECT_NAME)-$(VERSION).tar.gz \
		$(SRC_DIR)/ $(INCLUDE_DIR)/ $(DOCS_DIR)/ $(EXAMPLES_DIR)/ $(SCRIPTS_DIR)/ \
		Makefile LICENSE $(BUILD_DIR)/$(PROJECT_NAME)
	@echo "✓ Created $(PROJECT_NAME)-$(VERSION).tar.gz"

# Install target (requires root/sudo)
install: $(TARGET)
	@echo "✓ Installing $(PROJECT_NAME)..."
	cp $(TARGET) $(INSTALL_DIR)/$(PROJECT_NAME)
	chmod +x $(INSTALL_DIR)/$(PROJECT_NAME)
	@echo "✓ Installed $(PROJECT_NAME) to $(INSTALL_DIR)"

# Uninstall target
uninstall:
	rm -f $(INSTALL_DIR)/$(PROJECT_NAME)
	@echo "✓ Uninstalled $(PROJECT_NAME) from $(INSTALL_DIR)"

# Show help
help:
	@echo "$(PROJECT_NAME) v$(VERSION) - Available targets:"
	@echo ""
	@echo "Building:"
	@echo "  all          - Build the project (default)"
	@echo "  debug        - Build with debug symbols and sanitizers"
	@echo "  release      - Build optimized for release"
	@echo "  clean        - Clean build artifacts"
	@echo ""
	@echo "Project Organization:"
	@echo "  setup-dirs   - Create organized directory structure"
	@echo "  organize     - Move files to organized structure"
	@echo ""
	@echo "Testing and Quality:"
	@echo "  test         - Run test suite"
	@echo "  dev          - Clean, build, and test"
	@echo "  static-analysis - Run static code analysis"
	@echo "  memcheck     - Run memory leak detection"
	@echo "  format       - Format source code"
	@echo ""
	@echo "Distribution:"
	@echo "  dist         - Create distribution package"
	@echo "  install      - Install to system (requires sudo)"
	@echo "  uninstall    - Uninstall from system"
	@echo ""
	@echo "Utilities:"
	@echo "  run          - Run the shell interactively"
	@echo "  help         - Show this help message"

# Phony targets
.PHONY: all debug release clean setup-dirs organize test run dev static-analysis memcheck format dist install uninstall help
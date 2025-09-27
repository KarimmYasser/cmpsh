#!/bin/bash

# cmpsh Test Runner
# Comprehensive test suite for the cmpsh shell implementation

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test configuration
SHELL_BINARY="../build/cmpsh"
TEST_DIR="../tests"
TEMP_DIR="test_temp"
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Function to print colored output
print_status() {
    local status=$1
    local message=$2
    case $status in
        "PASS")
            echo -e "${GREEN}✓ PASS${NC}: $message"
            ;;
        "FAIL")
            echo -e "${RED}✗ FAIL${NC}: $message"
            ;;
        "INFO")
            echo -e "${BLUE}ℹ INFO${NC}: $message"
            ;;
        "WARN")
            echo -e "${YELLOW}⚠ WARN${NC}: $message"
            ;;
    esac
}

# Function to run a test
run_test() {
    local test_name=$1
    local test_script=$2
    local expected_exit_code=${3:-0}
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    print_status "INFO" "Running test: $test_name"
    
    # Create temporary directory for test
    mkdir -p "$TEMP_DIR"
    cd "$TEMP_DIR"
    
    # Copy shell binary to temp directory
    cp "../$SHELL_BINARY" .
    
    # Run the test
    if timeout 10s ./cmpsh "../$TEST_DIR/$test_script" > output.txt 2> error.txt; then
        local actual_exit_code=$?
    else
        local actual_exit_code=$?
    fi
    
    # Check exit code
    if [ $actual_exit_code -eq $expected_exit_code ]; then
        print_status "PASS" "$test_name"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        print_status "FAIL" "$test_name (expected exit code $expected_exit_code, got $actual_exit_code)"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        echo "STDOUT:"
        cat output.txt
        echo "STDERR:"
        cat error.txt
    fi
    
    # Clean up
    cd ..
    rm -rf "$TEMP_DIR"
}

# Function to test interactive mode
test_interactive() {
    local test_name=$1
    local commands=$2
    local expected_output=$3
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    print_status "INFO" "Running interactive test: $test_name"
    
    # Create temporary directory
    mkdir -p "$TEMP_DIR"
    cd "$TEMP_DIR"
    cp "../$SHELL_BINARY" .
    
    # Run interactive test
    echo -e "$commands" | timeout 5s ./cmpsh > output.txt 2> error.txt || true
    
    if grep -q "$expected_output" output.txt; then
        print_status "PASS" "$test_name"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        print_status "FAIL" "$test_name (output not found)"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        echo "Expected to find: $expected_output"
        echo "Actual output:"
        cat output.txt
    fi
    
    cd ..
    rm -rf "$TEMP_DIR"
}

# Main test execution
main() {
    echo "==============================================="
    echo "           cmpsh Test Suite"
    echo "==============================================="
    echo
    
    # Check if shell binary exists
    if [ ! -x "$SHELL_BINARY" ]; then
        print_status "FAIL" "Shell binary $SHELL_BINARY not found or not executable"
        echo "Please run 'make' first to build the shell"
        exit 1
    fi
    
    # Check if test directory exists
    if [ ! -d "$TEST_DIR" ]; then
        print_status "WARN" "Test directory $TEST_DIR not found"
        echo "Creating basic test directory structure..."
        mkdir -p "$TEST_DIR"
    fi
    
    print_status "INFO" "Starting test execution..."
    echo
    
    # Test 1: Built-in commands
    if [ -f "$TEST_DIR/built_in.sh" ]; then
        run_test "Built-in Commands" "built_in.sh"
    else
        # Create basic built-in test
        cat > "$TEST_DIR/built_in.sh" << 'EOF'
pwd
cd /tmp
pwd
cd /
pwd
exit
EOF
        run_test "Built-in Commands" "built_in.sh"
    fi
    
    # Test 2: External commands
    cat > "$TEST_DIR/external.sh" << 'EOF'
echo "Hello World"
ls -la > /dev/null
date > /dev/null
exit
EOF
    run_test "External Commands" "external.sh"
    
    # Test 3: Piping
    if [ -f "$TEST_DIR/pipe.sh" ]; then
        run_test "Piping Support" "pipe.sh"
    else
        cat > "$TEST_DIR/pipe.sh" << 'EOF'
echo "test" | cat
ls | head -5
echo "multiple pipes" | cat | wc -l
exit
EOF
        run_test "Piping Support" "pipe.sh"
    fi
    
    # Test 4: Redirection
    if [ -f "$TEST_DIR/redirect.sh" ]; then
        run_test "I/O Redirection" "redirect.sh"
    else
        cat > "$TEST_DIR/redirect.sh" << 'EOF'
echo "redirect test" > test_output.txt
cat test_output.txt
ls -la > file_list.txt
exit
EOF
        run_test "I/O Redirection" "redirect.sh"
    fi
    
    # Test 5: Path command
    if [ -f "$TEST_DIR/path.sh" ]; then
        run_test "Path Command" "path.sh"
    else
        cat > "$TEST_DIR/path.sh" << 'EOF'
path /bin /usr/bin
echo "Path set successfully"
exit
EOF
        run_test "Path Command" "path.sh"
    fi
    
    # Test 6: Error handling
    cat > "$TEST_DIR/error_test.sh" << 'EOF'
nonexistent_command
invalid-command-name
exit
EOF
    run_test "Error Handling" "error_test.sh" 0  # Shell should continue after errors
    
    # Interactive tests
    print_status "INFO" "Running interactive mode tests..."
    
    test_interactive "Interactive Prompt" "pwd\nexit\n" "cmpsh>"
    test_interactive "Exit Command" "exit\n" ""
    
    # Summary
    echo
    echo "==============================================="
    echo "           Test Results Summary"
    echo "==============================================="
    echo "Total Tests: $TOTAL_TESTS"
    echo -e "Passed: ${GREEN}$PASSED_TESTS${NC}"
    echo -e "Failed: ${RED}$FAILED_TESTS${NC}"
    
    if [ $FAILED_TESTS -eq 0 ]; then
        echo -e "\n${GREEN}🎉 All tests passed!${NC}"
        exit 0
    else
        echo -e "\n${RED}❌ Some tests failed.${NC}"
        exit 1
    fi
}

# Handle script arguments
case "${1:-all}" in
    "all")
        main
        ;;
    "basic")
        print_status "INFO" "Running basic tests only..."
        TOTAL_TESTS=1
        run_test "Basic Functionality" "built_in.sh"
        ;;
    "help")
        echo "Usage: $0 [all|basic|help]"
        echo "  all   - Run full test suite (default)"
        echo "  basic - Run basic functionality tests only"
        echo "  help  - Show this help message"
        ;;
    *)
        echo "Unknown option: $1"
        echo "Use '$0 help' for usage information"
        exit 1
        ;;
esac
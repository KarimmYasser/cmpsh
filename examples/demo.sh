#!/bin/bash

# cmpsh Demo Script
# Showcases the capabilities of the cmpsh shell implementation

# Colors for output
BOLD='\033[1m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${BOLD}==============================================="
echo -e "           cmpsh Shell Demo"
echo -e "===============================================${NC}"
echo
echo "This demo showcases the key features of cmpsh,"
echo "a Unix-compatible shell written in C."
echo
echo -e "${YELLOW}Press Enter to continue between demonstrations...${NC}"
read -p ""

# Build the shell if it doesn't exist
if [ ! -x "./cmpsh" ]; then
    echo -e "${BLUE}Building cmpsh...${NC}"
    make
    echo
fi

echo -e "${GREEN}Demo 1: Built-in Commands${NC}"
echo "Demonstrating pwd, cd, and path commands:"
echo

# Create demo script for built-in commands
cat > demo_builtin.sh << 'EOF'
echo "=== Built-in Commands Demo ==="
pwd
echo "Current directory shown above"
echo
echo "Changing to /tmp directory..."
cd /tmp
pwd
echo
echo "Setting custom path..."
path /bin /usr/bin /usr/local/bin
echo "Path updated successfully"
echo
exit
EOF

echo "Running: ./cmpsh demo_builtin.sh"
./cmpsh demo_builtin.sh
rm -f demo_builtin.sh

read -p ""

echo -e "${GREEN}Demo 2: External Command Execution${NC}"
echo "Running system commands through the shell:"
echo

cat > demo_external.sh << 'EOF'
echo "=== External Commands Demo ==="
echo "File listing:"
ls -la | head -5
echo
echo "Current date and time:"
date
echo
echo "System information:"
uname -a
echo
exit
EOF

echo "Running: ./cmpsh demo_external.sh"
./cmpsh demo_external.sh
rm -f demo_external.sh

read -p ""

echo -e "${GREEN}Demo 3: Piping Support${NC}"
echo "Chaining multiple commands together:"
echo

cat > demo_piping.sh << 'EOF'
echo "=== Piping Demo ==="
echo "Counting files in current directory:"
ls -1 | wc -l
echo
echo "Finding .sh files and counting them:"
ls -la | grep "\.sh" | wc -l
echo
echo "Showing first few characters of files:"
echo "Hello World from cmpsh!" | cut -c1-5
echo
exit
EOF

echo "Running: ./cmpsh demo_piping.sh"
./cmpsh demo_piping.sh
rm -f demo_piping.sh

read -p ""

echo -e "${GREEN}Demo 4: I/O Redirection${NC}"
echo "Redirecting command output to files:"
echo

cat > demo_redirect.sh << 'EOF'
echo "=== I/O Redirection Demo ==="
echo "Creating a file with redirection:"
echo "This is a test file created by cmpsh" > demo_output.txt
echo "File created successfully"
echo
echo "Contents of the created file:"
cat demo_output.txt
echo
echo "Redirecting ls output to a file:"
ls -la > file_listing.txt
echo "Directory listing saved to file_listing.txt"
echo "First few lines:"
head -3 file_listing.txt
echo
exit
EOF

echo "Running: ./cmpsh demo_redirect.sh"
./cmpsh demo_redirect.sh
rm -f demo_redirect.sh demo_output.txt file_listing.txt

read -p ""

echo -e "${GREEN}Demo 5: Error Handling${NC}"
echo "Demonstrating graceful error handling:"
echo

cat > demo_errors.sh << 'EOF'
echo "=== Error Handling Demo ==="
echo "Trying to run a non-existent command:"
nonexistent_command_12345
echo "Shell continues after error"
echo
echo "Trying to change to non-existent directory:"
cd /this/directory/does/not/exist
echo "Shell handles the error gracefully"
echo
echo "Normal command after errors:"
echo "Shell is still working fine!"
exit
EOF

echo "Running: ./cmpsh demo_errors.sh"
./cmpsh demo_errors.sh 2>/dev/null  # Suppress error messages for cleaner demo
rm -f demo_errors.sh

read -p ""

echo -e "${GREEN}Demo 6: Interactive Mode${NC}"
echo "Starting interactive shell session..."
echo "Try these commands:"
echo "  - pwd"
echo "  - ls | head -3"
echo "  - echo 'Hello' > test.txt"
echo "  - cat test.txt"
echo "  - exit"
echo
echo -e "${YELLOW}Starting cmpsh in interactive mode:${NC}"
echo

# Start interactive mode
./cmpsh

echo
echo -e "${BOLD}==============================================="
echo -e "           Demo Complete!"
echo -e "===============================================${NC}"
echo
echo "cmpsh features demonstrated:"
echo "✓ Built-in commands (pwd, cd, path)"
echo "✓ External command execution"
echo "✓ Piping support"
echo "✓ I/O redirection"
echo "✓ Error handling"
echo "✓ Interactive mode"
echo
echo "Additional features not shown in demo:"
echo "• Signal handling (Ctrl+C, Ctrl+Z)"
echo "• Quote handling in commands"
echo "• Memory management and cleanup"
echo "• Non-interactive script execution"
echo
echo -e "${GREEN}Thank you for trying cmpsh!${NC}"
echo "For more information, see README.md"
echo
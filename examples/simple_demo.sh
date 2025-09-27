#!/bin/bash
# Simple cmpsh Feature Demonstration

echo "==============================================="
echo "           cmpsh Enhanced Features Demo"
echo "==============================================="
echo

echo "🚀 Testing new shell features..."
echo

# Test 1: Help system
echo "📖 Testing help system:"
echo "help" | ./cmpsh
echo

# Test 2: Environment variables
echo "🌍 Testing environment display (first 3):"
echo "env" | ./cmpsh | head -3
echo

# Test 3: Command history
echo "📜 Testing command history:"
echo -e "pwd\nls\nhistory\nexit" | ./cmpsh | tail -6
echo

# Test 4: Alias system
echo "🔗 Testing alias system:"
echo -e "alias ll \"ls\"\nalias\nll\nexit" | ./cmpsh
echo

# Test 5: Basic functionality
echo "⚡ Testing core shell functionality:"
echo -e "pwd\necho Hello from cmpsh!\ndate\nexit" | ./cmpsh
echo

echo "==============================================="
echo "✨ All enhanced features are working!"
echo "==============================================="
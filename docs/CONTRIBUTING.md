# Contributing to cmpsh

Thank you for your interest in contributing to cmpsh! This document provides guidelines for contributing to the project.

## 🤝 How to Contribute

### Reporting Issues

1. **Search existing issues** first to avoid duplicates
2. **Use a clear, descriptive title** that summarizes the issue
3. **Provide detailed information** including:
   - Steps to reproduce the issue
   - Expected vs actual behavior
   - Your operating system and environment
   - Shell command that caused the issue
   - Any error messages

### Suggesting Enhancements

1. **Check existing feature requests** to avoid duplicates
2. **Explain the motivation** for the enhancement
3. **Describe the expected behavior** in detail
4. **Consider backward compatibility** implications

### Code Contributions

#### Prerequisites

- GCC compiler
- Make
- Basic understanding of C programming
- Familiarity with Unix/Linux systems and shell concepts

#### Development Setup

```bash
# Fork and clone the repository
git clone https://github.com/your-username/cmpsh.git
cd cmpsh

# Build the project
make

# Run tests
./run_tests.sh

# Test your changes manually
./cmpsh
```

#### Pull Request Process

1. **Fork the repository** and create a feature branch

   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes** following the coding standards below

3. **Test thoroughly**:

   ```bash
   # Run full test suite
   ./run_tests.sh

   # Test manually with various commands
   ./cmpsh

   # Check for memory leaks (if valgrind available)
   make memcheck
   ```

4. **Update documentation** if needed (README.md, comments)

5. **Commit your changes** with clear, descriptive messages:

   ```bash
   git add .
   git commit -m "feat: add command history support"
   ```

6. **Push to your fork** and create a pull request:

   ```bash
   git push origin feature/your-feature-name
   ```

7. **Create a pull request** with:
   - Clear title and description
   - Reference to any related issues
   - Description of changes made
   - Testing performed

## 📝 Coding Standards

### Code Style

- **Indentation**: 4 spaces (no tabs)
- **Line Length**: 80-100 characters maximum
- **Naming**:
  - Functions: `snake_case`
  - Variables: `snake_case`
  - Constants: `UPPER_CASE`
  - Global variables: Minimize usage, prefix with meaningful name

### Documentation

- **Function Comments**: Use block comments for all functions

  ```c
  /**
   * Brief description of what the function does.
   * More detailed explanation if needed.
   *
   * @param param1 Description of parameter
   * @param param2 Description of parameter
   * @return Description of return value
   */
  ```

- **Inline Comments**: Explain complex logic, not obvious code
- **README Updates**: Update documentation for new features

### Code Quality

- **Error Handling**: Always check return values and handle errors
- **Memory Management**:
  - Always free allocated memory
  - Set pointers to NULL after freeing
  - Check for allocation failures
- **Input Validation**: Validate all user inputs
- **Buffer Safety**: Use safe string functions, avoid buffer overflows

### Testing

- **New Features**: Add corresponding tests to `test_scripts/`
- **Bug Fixes**: Add regression tests
- **Test Coverage**: Ensure tests cover both success and failure cases
- **Manual Testing**: Test interactively with various commands

## 🏗️ Project Structure

```
cmpsh/
├── cmpsh.c              # Main source code
├── Makefile             # Build configuration
├── README.md            # Project documentation
├── LICENSE              # MIT license
├── CONTRIBUTING.md      # This file
├── run_tests.sh         # Test runner script
├── test_scripts/        # Test cases
│   ├── built_in.sh      # Built-in command tests
│   ├── pipe.sh          # Piping tests
│   ├── redirect.sh      # Redirection tests
│   └── ...              # Other test files
└── Dockerfile           # Container configuration
```

## 🎯 Areas for Contribution

### High Priority

- [ ] Command history and recall (up/down arrows)
- [ ] Tab completion for commands and files
- [ ] Background job control (`&`, `jobs`, `fg`, `bg`)
- [ ] Improved error messages with suggestions

### Medium Priority

- [ ] Environment variable expansion (`$VAR`, `${VAR}`)
- [ ] Glob pattern matching (`*.txt`, `file?.c`)
- [ ] Configuration file support (`~/.cmpshrc`)
- [ ] Built-in help system

### Low Priority

- [ ] Command aliases
- [ ] Script debugging mode
- [ ] Performance optimizations
- [ ] Additional built-in commands

## 🧪 Testing Guidelines

### Test Categories

1. **Unit Tests**: Test individual functions
2. **Integration Tests**: Test command execution and piping
3. **Interactive Tests**: Test user interaction scenarios
4. **Error Tests**: Test error handling and edge cases
5. **Performance Tests**: Test with large inputs/outputs

### Test Naming

- Test files: `test_feature.sh`
- Test functions: `test_specific_behavior`
- Be descriptive: `test_pipe_three_commands` vs `test_pipe`

## 🐛 Debugging

### Common Issues

1. **Memory Leaks**: Use `valgrind` to detect
2. **Buffer Overflows**: Use sanitizers (`-fsanitize=address`)
3. **Signal Handling**: Test with Ctrl+C and Ctrl+Z
4. **Process Management**: Ensure proper cleanup of child processes

### Debugging Tools

```bash
# Memory leak detection
make memcheck

# Address sanitizer
make debug

# Static analysis
make analyze

# GDB debugging
gdb ./cmpsh
```

## 📋 Checklist for Contributors

Before submitting a pull request:

- [ ] Code compiles without warnings
- [ ] All existing tests pass
- [ ] New tests added for new functionality
- [ ] Code follows project style guidelines
- [ ] Documentation updated if needed
- [ ] Memory leaks checked with valgrind
- [ ] Tested on multiple scenarios manually
- [ ] Commit messages are clear and descriptive

## 🚀 Release Process

1. All tests pass
2. Version number updated
3. CHANGELOG.md updated
4. Tag created with version number
5. GitHub release created

## ❓ Questions?

If you have questions about contributing:

1. Check existing issues and documentation
2. Create a new issue with the question
3. Contact the maintainers

Thank you for contributing to cmpsh! 🎉

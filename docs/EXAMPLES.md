---
noteId: "e31009a19bcf11f0aea711b912eba21a"
tags: []

---

# cmpsh Usage Examples

This document provides comprehensive examples demonstrating all features of the enhanced cmpsh shell.

## Basic Usage Examples

### Interactive Mode

```bash
$ ./cmpsh
cmpsh> help
cmpsh - Custom Shell Implementation
Built-in commands:
  exit        - Exit the shell
  cd <dir>    - Change directory
  pwd         - Print working directory
  path <dirs> - Set executable search paths
  help        - Show this help message
  env         - Show environment variables
  history     - Show command history
  alias       - Show/set command aliases

cmpsh> pwd
/home/user/cmpsh
cmpsh> ls -la | grep ".c"
-rw-r--r-- 1 user user 12345 Oct 15 10:30 cmpsh.c
cmpsh> echo "Hello, World!" > greeting.txt
cmpsh> cat greeting.txt
Hello, World!
cmpsh> exit
```

### Script Mode

Create a script file:

```bash
#!/path/to/cmpsh
echo "Starting backup process..."
pwd
ls -la > backup_list.txt
echo "Backup list created"
```

Run the script:

```bash
./cmpsh backup_script.sh
```

## Feature Examples

### Enhanced Built-in Commands

```bash
# Help system
cmpsh> help
cmpsh - Custom Shell Implementation
Built-in commands:
  exit        - Exit the shell
  cd <dir>    - Change directory
  pwd         - Print working directory
  path <dirs> - Set executable search paths
  help        - Show this help message
  env         - Show environment variables
  history     - Show command history
  alias       - Show/set command aliases

# Environment variables
cmpsh> env
HOME=/home/user
PATH=/bin:/usr/bin:/usr/local/bin
USER=user
...

# Directory navigation
cmpsh> pwd
/home/user
cmpsh> cd Documents
cmpsh> pwd
/home/user/Documents

# Command history
cmpsh> ls
file1.txt  file2.txt  folder1/
cmpsh> pwd
/home/user/Documents
cmpsh> history
  1  ls
  2  pwd
  3  history

# Path management
cmpsh> path /bin /usr/bin /usr/local/bin
cmpsh> path /custom/path
```

## New Features Examples

### Alias System

```bash
# Create aliases
cmpsh> alias ll "ls -l"
Alias 'll' set to 'ls -l'
cmpsh> alias la "ls -la"
Alias 'la' set to 'ls -la'
cmpsh> alias grep-c "grep -c"
Alias 'grep-c' set to 'grep -c'

# View all aliases
cmpsh> alias
alias ll='ls -l'
alias la='ls -la'
alias grep-c='grep -c'

# Use aliases
cmpsh> ll
total 24
-rw-r--r-- 1 user user 1234 Sep 27 21:00 file1.txt
-rw-r--r-- 1 user user 5678 Sep 27 21:01 file2.txt
drwxr-xr-x 2 user user 4096 Sep 27 21:02 folder1/

cmpsh> la
total 32
drwxr-xr-x 3 user user 4096 Sep 27 21:02 .
drwxr-xr-x 5 user user 4096 Sep 27 21:00 ..
-rw-r--r-- 1 user user 1234 Sep 27 21:00 file1.txt
-rw-r--r-- 1 user user 5678 Sep 27 21:01 file2.txt
drwxr-xr-x 2 user user 4096 Sep 27 21:02 folder1/
```

### Environment Variable Expansion

```bash
# Variable expansion in commands
cmpsh> echo "Your home directory: $HOME"
Your home directory: /home/user

cmpsh> echo "Current user: $USER"
Current user: user

cmpsh> echo "Current directory: $PWD"
Current directory: /home/user/Documents

# Tilde expansion
cmpsh> echo "Home: ~"
Home: /home/user

cmpsh> ls ~/Documents
file1.txt  file2.txt  folder1/
```

### Command History Usage

```bash
# Execute several commands
cmpsh> pwd
/home/user
cmpsh> ls
Documents  Downloads  Pictures
cmpsh> cd Documents
cmpsh> ls -l
total 8
-rw-r--r-- 1 user user 1234 Sep 27 21:00 file1.txt
-rw-r--r-- 1 user user 5678 Sep 27 21:01 file2.txt
cmpsh> pwd

# View command history
cmpsh> history
  1  pwd
  2  ls
  3  cd Documents
  4  ls -l
  5  pwd
  6  history
```

### Piping

```bash
# Simple pipe
cmpsh> ls -la | grep ".txt"

# Multiple pipes
cmpsh> ps aux | grep "python" | awk '{print $2}' | head -5

# Pipe with built-ins
cmpsh> ls | wc -l
```

### I/O Redirection

```bash
# Redirect output
cmpsh> ls -la > directory_listing.txt
cmpsh> echo "Log entry" > application.log
cmpsh> date > timestamp.txt

# Use redirected files
cmpsh> cat directory_listing.txt | grep "drwx"
```

### Error Handling

```bash
# Command not found
cmpsh> nonexistent_command
An error has occurred: Command not found

# Invalid directory
cmpsh> cd /nonexistent/path
An error has occurred: Cannot change directory

# Shell continues after errors
cmpsh> echo "Still working!"
```

## Advanced Usage Patterns

### Log Processing

```bash
# Analyze log files
cmpsh> cat application.log | grep "ERROR" | wc -l
cmpsh> cat access.log | grep "404" > errors_404.txt
```

### File Management

```bash
# Find and process files
cmpsh> ls -la | grep "\.tmp" > temp_files.txt
cmpsh> cat temp_files.txt | awk '{print $9}' > temp_names.txt
```

### System Monitoring

```bash
# Process monitoring
cmpsh> ps aux | grep "httpd" > web_processes.txt
cmpsh> ps aux | head -10 > top_processes.txt
```

### Batch Operations

```bash
# Create multiple files
cmpsh> echo "File 1" > file1.txt
cmpsh> echo "File 2" > file2.txt
cmpsh> echo "File 3" > file3.txt

# Process them
cmpsh> cat file1.txt file2.txt file3.txt > combined.txt
```

## Script Templates

### Backup Script

```bash
#!/path/to/cmpsh
echo "Starting backup at $(date)"
pwd
ls -la > backup_manifest.txt
echo "Files backed up:"
cat backup_manifest.txt | grep "^-" | wc -l
echo "Backup completed"
```

### Log Analysis Script

```bash
#!/path/to/cmpsh
echo "Analyzing logs..."
cat /var/log/application.log | grep "ERROR" > errors.txt
echo "Error count:"
cat errors.txt | wc -l
echo "Recent errors:"
cat errors.txt | tail -5
```

### Development Helper Script

```bash
#!/path/to/cmpsh
echo "Development environment check"
pwd
ls -la | grep "\.c$" > source_files.txt
echo "C source files found:"
cat source_files.txt | wc -l
echo "Building project..."
```

## Tips and Best Practices

1. **Always use absolute paths** when scripting
2. **Check command output** before piping to critical operations
3. **Use quotes** for commands with special characters
4. **Test scripts** in interactive mode first
5. **Handle errors gracefully** in production scripts

## Common Patterns

### File Filtering

```bash
# Find specific file types
ls -la | grep "\.txt$"
ls -la | grep "^d"  # directories only

# Count files
ls -1 | wc -l
```

### Text Processing

```bash
# Extract columns
ps aux | awk '{print $2}'  # PIDs only
ls -la | awk '{print $9}'  # filenames only

# Sort and unique
cat file.txt | sort | uniq
```

### Output Formatting

```bash
# Create formatted reports
echo "System Report" > report.txt
echo "=============" >> report.txt
date >> report.txt
```

## Troubleshooting

### Common Issues

- **Command not found**: Check path settings with `path` command
- **Permission denied**: Ensure execute permissions on scripts
- **File not found**: Use absolute paths or verify working directory

### Debugging Tips

- Use `pwd` to verify current directory
- Test commands individually before piping
- Check file permissions with `ls -la`
- Verify path settings affect command resolution

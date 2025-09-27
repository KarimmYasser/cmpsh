/**
 * cmpsh - Custom Shell Implementation
 * 
 * A Unix-compatible shell written in C that provides:
 * - Interactive and non-interactive modes
 * - Built-in commands (exit, cd, pwd, path)
 * - External command execution with path resolution
 * - Piping support for command chaining
 * - I/O redirection to files
 * - Proper signal handling (SIGINT, SIGTSTP)
 * - Memory management and error handling
 * 
 * Author: Your Name
 * Date: September 2025
 */

#define _POSIX_C_SOURCE 200809L  /* Enable POSIX functions */
#define _DEFAULT_SOURCE          /* Enable additional functions like strdup */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>

/* Configuration constants */
#define MAX_LINE 1024        /* Maximum input line length */
#define MAX_TOKENS 10        /* Maximum tokens per command */
#define MAX_PATHS 10         /* Maximum search paths */
#define MAX_COMMANDS 10      /* Maximum commands in pipeline */
#define MAX_HISTORY 100      /* Maximum history entries */
#define MAX_ALIASES 50       /* Maximum number of aliases */

/* Alias structure */
typedef struct {
    char* name;              /* Alias name */
    char* command;           /* Command to execute */
} alias_t;

/* Global variables */
char** paths = NULL;         /* Array of executable search paths */
int num_paths = 0;          /* Number of configured paths */
pid_t current_child = -1;   /* PID of currently running child process */
char** command_history = NULL; /* Command history array */
int history_count = 0;      /* Number of commands in history */
alias_t aliases[MAX_ALIASES]; /* Command aliases */
int alias_count = 0;        /* Number of defined aliases */

/**
 * Signal handler for SIGINT (Ctrl+C)
 * Forwards the signal to the currently running child process
 * while keeping the shell alive.
 * 
 * @param sig Signal number (unused)
 */
void sigint_handler(int sig) {
    (void)sig; /* Suppress unused parameter warning */
    if (current_child != -1) {
        kill(current_child, SIGINT);
    }
}

/**
 * Signal handler for SIGTSTP (Ctrl+Z)
 * Forwards the signal to the currently running child process
 * to suspend it.
 * 
 * @param sig Signal number (unused)
 */
void sigtstp_handler(int sig) {
    (void)sig; /* Suppress unused parameter warning */
    if (current_child != -1) {
        kill(current_child, SIGTSTP);
    }
}

/**
 * Add a command to the history.
 * Manages a circular buffer of recent commands.
 * 
 * @param command Command string to add to history
 */
void add_to_history(const char* command) {
    if (!command || strlen(command) == 0) return;
    
    /* Initialize history array if needed */
    if (command_history == NULL) {
        command_history = malloc(MAX_HISTORY * sizeof(char*));
        if (!command_history) return;
        for (int i = 0; i < MAX_HISTORY; i++) {
            command_history[i] = NULL;
        }
    }
    
    /* Free oldest entry if history is full */
    if (history_count >= MAX_HISTORY) {
        free(command_history[0]);
        /* Shift all entries down */
        for (int i = 0; i < MAX_HISTORY - 1; i++) {
            command_history[i] = command_history[i + 1];
        }
        command_history[MAX_HISTORY - 1] = strdup(command);
    } else {
        command_history[history_count] = strdup(command);
        history_count++;
    }
}

/**
 * Display command history.
 */
void show_history(void) {
    if (history_count == 0) {
        printf("No commands in history\n");
        return;
    }
    
    int count = (history_count >= MAX_HISTORY) ? MAX_HISTORY : history_count;
    
    for (int i = 0; i < count; i++) {
        printf("%3d  %s\n", i + 1, command_history[i]);
    }
}

/**
 * Simple environment variable expansion.
 * Expands $HOME, $USER, and $PWD in command arguments.
 * 
 * @param arg Argument string that may contain environment variables
 * @return Expanded string (caller must free) or original string if no expansion
 */
char* expand_variables(const char* arg) {
    if (!arg || strlen(arg) == 0) return strdup("");
    
    /* Simple expansion for common variables */
    if (strcmp(arg, "$HOME") == 0) {
        char* home = getenv("HOME");
        return home ? strdup(home) : strdup("");
    } else if (strcmp(arg, "$USER") == 0) {
        char* user = getenv("USER");
        return user ? strdup(user) : strdup("");
    } else if (strcmp(arg, "$PWD") == 0) {
        char cwd[MAX_LINE];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            return strdup(cwd);
        }
        return strdup("");
    }
    
    /* Check for ~ expansion (home directory) */
    if (arg[0] == '~' && (arg[1] == '\0' || arg[1] == '/')) {
        char* home = getenv("HOME");
        if (home) {
            char* expanded = malloc(strlen(home) + strlen(arg) + 1);
            if (expanded) {
                strcpy(expanded, home);
                strcat(expanded, arg + 1); /* Skip the ~ */
                return expanded;
            }
        }
    }
    
    return strdup(arg); /* Return copy of original */
}

/**
 * Add or update an alias.
 * 
 * @param name Alias name
 * @param command Command to execute
 * @return 0 on success, -1 on error
 */
int add_alias(const char* name, const char* command) {
    if (!name || !command) return -1;
    
    /* Check if alias already exists */
    for (int i = 0; i < alias_count; i++) {
        if (strcmp(aliases[i].name, name) == 0) {
            /* Update existing alias */
            free(aliases[i].command);
            aliases[i].command = strdup(command);
            return 0;
        }
    }
    
    /* Add new alias if space available */
    if (alias_count < MAX_ALIASES) {
        aliases[alias_count].name = strdup(name);
        aliases[alias_count].command = strdup(command);
        alias_count++;
        return 0;
    }
    
    return -1; /* No space for new alias */
}

/**
 * Look up an alias and return the command it represents.
 * 
 * @param name Alias name to look up
 * @return Command string or NULL if not found
 */
const char* lookup_alias(const char* name) {
    if (!name) return NULL;
    
    for (int i = 0; i < alias_count; i++) {
        if (strcmp(aliases[i].name, name) == 0) {
            return aliases[i].command;
        }
    }
    return NULL;
}

/**
 * Display all defined aliases.
 */
void show_aliases(void) {
    if (alias_count == 0) {
        printf("No aliases defined\n");
        return;
    }
    
    for (int i = 0; i < alias_count; i++) {
        printf("alias %s='%s'\n", aliases[i].name, aliases[i].command);
    }
}

/**
 * Trim leading and trailing whitespace from a string.
 * Modifies the string in-place by moving the start pointer
 * and null-terminating at the new end.
 * 
 * @param str Input string to trim
 * @return Pointer to the trimmed string
 */
char* trim_whitespace(char* str) {
    char* end;
    
    /* Skip leading whitespace */
    while (isspace((unsigned char)*str)) {
        str++;
    }
    
    /* Handle empty string */
    if (*str == 0) {
        return str;
    }
    
    /* Trim trailing whitespace */
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        end--;
    }
    end[1] = '\0';
    
    return str;
}

/**
 * Free all tokens in a token array.
 * 
 * @param tokens Array of token pointers to free
 * @param num_tokens Number of tokens in the array
 */
void free_tokens(char* tokens[], int num_tokens) {
    for (int i = 0; i < num_tokens; i++) {
        if (tokens[i]) {
            free(tokens[i]);
            tokens[i] = NULL;
        }
    }
}

/**
 * Tokenize a command string into individual arguments.
 * Handles quoted strings (both single and double quotes) and
 * properly separates tokens by whitespace.
 * 
 * @param cmd Command string to tokenize
 * @param tokens Array to store token pointers (caller must free)
 * @param max_tokens Maximum number of tokens to extract
 * @return Number of tokens found, or -1 on error
 */
int tokenize_command(char* cmd, char* tokens[], int max_tokens) {
    int num_tokens = 0;
    char* ptr = cmd;
    char* token_start = NULL;
    int in_quotes = 0;
    char quote_char = 0;
    char* buffer = malloc(strlen(cmd) + 1);
    int buffer_idx = 0;

    if (!buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        return -1;
    }

    while (*ptr && num_tokens < max_tokens) {
        /* Handle whitespace outside quotes - token separator */
        if (isspace((unsigned char)*ptr) && !in_quotes) {
            if (token_start) {
                buffer[buffer_idx] = '\0';
                tokens[num_tokens] = strdup(buffer);
                if (!tokens[num_tokens]) {
                    free(buffer);
                    return -1;
                }
                num_tokens++;
                buffer_idx = 0;
                token_start = NULL;
            }
            ptr++;
            continue;
        }

        /* Handle opening quote */
        if ((*ptr == '"' || *ptr == '\'') && !in_quotes) {
            in_quotes = 1;
            quote_char = *ptr;
            token_start = ptr + 1;
            ptr++;
            continue;
        }

        /* Handle closing quote */
        if (*ptr == quote_char && in_quotes) {
            buffer[buffer_idx] = '\0';
            tokens[num_tokens] = strdup(buffer);
            if (!tokens[num_tokens]) {
                free(buffer);
                return -1;
            }
            num_tokens++;
            buffer_idx = 0;
            in_quotes = 0;
            token_start = NULL;
            ptr++;
            continue;
        }

        /* Start new token if not in quotes */
        if (!in_quotes && !token_start) {
            token_start = ptr;
        }

        /* Add character to current token */
        buffer[buffer_idx++] = *ptr++;
    }

    /* Handle the last token if present */
    if (token_start && num_tokens < max_tokens) {
        buffer[buffer_idx] = '\0';
        tokens[num_tokens] = strdup(buffer);
        if (!tokens[num_tokens]) {
            free(buffer);
            return -1;
        }
        num_tokens++;
    }

    /* Check for unclosed quotes */
    if (in_quotes) {
        fprintf(stderr, "An error has occurred: Unclosed quote\n");
        for (int i = 0; i < num_tokens; i++) {
            free(tokens[i]);
        }
        free(buffer);
        return -1;
    }

    free(buffer);
    return num_tokens;
}

/**
 * Main function - Entry point for the cmpsh shell.
 * 
 * Usage:
 *   ./cmpsh                 - Interactive mode
 *   ./cmpsh script.sh       - Non-interactive mode (execute script)
 * 
 * @param argc Argument count
 * @param argv Argument vector
 * @return Exit status (0 on success, 1 on error)
 */
int main(int argc, char* argv[]) {
    FILE* input;
    int interactive = 0;

    /* Determine input source based on command-line arguments */
    if (argc == 1) {
        /* Interactive mode - read from stdin */
        input = stdin;
        interactive = 1;
    } else if (argc == 2) {
        /* Non-interactive mode - read from script file */
        input = fopen(argv[1], "r");
        if (input == NULL) {
            fprintf(stderr, "An error has occurred: Cannot open file\n");
            exit(1);
        }
    } else {
        fprintf(stderr, "An error has occurred: Invalid arguments\n");
        exit(1);
    }

    /* Initialize default search paths with common system directories */
    num_paths = 3;
    paths = malloc(num_paths * sizeof(char*));
    if (paths == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    paths[0] = strdup("/bin");
    paths[1] = strdup("/usr/bin");
    paths[2] = strdup("/usr/local/bin");
    
    for (int i = 0; i < num_paths; i++) {
        if (paths[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            for (int j = 0; j < i; j++) {
                free(paths[j]);
            }
            free(paths);
            exit(1);
        }
    }

    /* Set up signal handlers for proper signal propagation */
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);

    /* Main shell loop */
    char line[MAX_LINE];
    while (1) {
        /* Display prompt in interactive mode */
        if (interactive) {
            printf("cmpsh> ");
            fflush(stdout);
        }

        /* Read input line */
        if (fgets(line, MAX_LINE, input) == NULL) {
            if (interactive) printf("\n");
            break; /* EOF reached */
        }

        /* Remove newline and trim whitespace */
        line[strcspn(line, "\n")] = 0;
        char* trimmed_line = trim_whitespace(line);
        if (strlen(trimmed_line) == 0) {
            continue; /* Skip empty lines */
        }

        /* Add command to history (only in interactive mode) */
        if (interactive) {
            add_to_history(trimmed_line);
        }

        /* Parse pipeline commands separated by '|' */
        char* commands[MAX_COMMANDS];
        int num_commands = 0;
        char* cmd = strtok(trimmed_line, "|");
        while (cmd && num_commands < MAX_COMMANDS) {
            commands[num_commands++] = trim_whitespace(cmd);
            cmd = strtok(NULL, "|");
        }
        if (num_commands == 0) {
            continue;
        }

        /* Tokenize each command in the pipeline */
        char* cmd_tokens[MAX_COMMANDS][MAX_TOKENS];
        int cmd_num_tokens[MAX_COMMANDS];
        for (int c = 0; c < num_commands; c++) {
            for (int i = 0; i < MAX_TOKENS; i++) {
                cmd_tokens[c][i] = NULL;
            }
            cmd_num_tokens[c] = tokenize_command(commands[c], cmd_tokens[c], MAX_TOKENS);
            if (cmd_num_tokens[c] < 0) {
                goto cleanup_tokens;
            }
            if (cmd_num_tokens[c] == 0) {
                fprintf(stderr, "An error has occurred: Empty command\n");
                goto cleanup_tokens;
            }
        }

        // Handle alias resolution for first command
        if (num_commands == 1) {
            const char* alias_cmd = lookup_alias(cmd_tokens[0][0]);
            if (alias_cmd) {
                /* Simple alias expansion - replace first token only for now */
                free(cmd_tokens[0][0]);
                cmd_tokens[0][0] = strdup(alias_cmd);
            }
        }

        // Handle built-in commands (only if single command)
        if (num_commands == 1) {
            if (strcmp(cmd_tokens[0][0], "exit") == 0) {
                if (cmd_num_tokens[0] != 1) {
                    fprintf(stderr, "An error has occurred: exit takes no arguments\n");
                } else {
                    for (int i = 0; i < num_paths; i++) {
                        free(paths[i]);
                    }
                    free(paths);
                    if (input != stdin) fclose(input);
                    exit(0);
                }
                continue;
            } else if (strcmp(cmd_tokens[0][0], "cd") == 0) {
                if (cmd_num_tokens[0] != 2) {
                    fprintf(stderr, "An error has occurred: cd requires exactly one argument\n");
                } else {
                    if (chdir(cmd_tokens[0][1]) < 0) {
                        fprintf(stderr, "An error has occurred: Cannot change directory\n");
                    }
                }
                continue;
            } else if (strcmp(cmd_tokens[0][0], "pwd") == 0) {
                if (cmd_num_tokens[0] != 1) {
                    fprintf(stderr, "An error has occurred: pwd takes no arguments\n");
                } else {
                    char cwd[MAX_LINE];
                    if (getcwd(cwd, sizeof(cwd)) != NULL) {
                        printf("%s\n", cwd);
                    } else {
                        fprintf(stderr, "An error has occurred: Cannot get current directory\n");
                    }
                }
                continue;
            } else if (strcmp(cmd_tokens[0][0], "help") == 0) {
                if (cmd_num_tokens[0] != 1) {
                    fprintf(stderr, "An error has occurred: help takes no arguments\n");
                } else {
                    printf("cmpsh - Custom Shell Implementation\n");
                    printf("Built-in commands:\n");
                    printf("  exit        - Exit the shell\n");
                    printf("  cd <dir>    - Change directory\n");
                    printf("  pwd         - Print working directory\n");
                    printf("  path <dirs> - Set executable search paths\n");
                    printf("  help        - Show this help message\n");
                    printf("  env         - Show environment variables\n");
                    printf("  history     - Show command history\n");
                    printf("  alias       - Show/set command aliases\n");
                    printf("\nFeatures:\n");
                    printf("  - Piping: command1 | command2\n");
                    printf("  - Redirection: command > file\n");
                    printf("  - Signal handling: Ctrl+C, Ctrl+Z\n");
                }
                continue;
            } else if (strcmp(cmd_tokens[0][0], "env") == 0) {
                if (cmd_num_tokens[0] != 1) {
                    fprintf(stderr, "An error has occurred: env takes no arguments\n");
                } else {
                    extern char **environ;
                    for (char **env = environ; *env != NULL; env++) {
                        printf("%s\n", *env);
                    }
                }
                continue;
            } else if (strcmp(cmd_tokens[0][0], "history") == 0) {
                if (cmd_num_tokens[0] != 1) {
                    fprintf(stderr, "An error has occurred: history takes no arguments\n");
                } else {
                    show_history();
                }
                continue;
            } else if (strcmp(cmd_tokens[0][0], "alias") == 0) {
                if (cmd_num_tokens[0] == 1) {
                    /* Show all aliases */
                    show_aliases();
                } else if (cmd_num_tokens[0] == 3) {
                    /* Add/update alias: alias name command */
                    if (add_alias(cmd_tokens[0][1], cmd_tokens[0][2]) == 0) {
                        printf("Alias '%s' set to '%s'\n", cmd_tokens[0][1], cmd_tokens[0][2]);
                    } else {
                        fprintf(stderr, "An error has occurred: Cannot set alias\n");
                    }
                } else {
                    fprintf(stderr, "An error has occurred: alias usage: alias [name command]\n");
                }
                continue;
            } else if (strcmp(cmd_tokens[0][0], "path") == 0 || strcmp(cmd_tokens[0][0], "paths") == 0) {
                if (cmd_num_tokens[0] < 2) {
                    fprintf(stderr, "An error has occurred: path requires at least one argument\n");
                } else {
                    // Free existing paths
                    for (int i = 0; i < num_paths; i++) {
                        free(paths[i]);
                    }
                    free(paths);

                    // Allocate new paths array
                    num_paths = cmd_num_tokens[0] - 1;
                    paths = malloc(num_paths * sizeof(char*));
                    if (paths == NULL) {
                        fprintf(stderr, "Memory allocation failed\n");
                        num_paths = 0;
                        continue;
                    }

                    // Copy new paths
                    for (int i = 0; i < num_paths; i++) {
                        paths[i] = strdup(cmd_tokens[0][i + 1]);
                        if (paths[i] == NULL) {
                            for (int j = 0; j < i; j++) {
                                free(paths[j]);
                            }
                            free(paths);
                            paths = NULL;
                            num_paths = 0;
                            fprintf(stderr, "Memory allocation failed\n");
                            continue;
                        }
                    }
                }
                continue;
            }
        }

        int redirect = -1;
        if (num_commands == 1) {
            for (int i = 0; i < cmd_num_tokens[0]; i++) {
                if (strcmp(cmd_tokens[0][i], ">") == 0) {
                    redirect = i;
                    break;
                }
            }
            if (redirect != -1) {
                if (redirect == 0 || redirect == cmd_num_tokens[0] - 1 || redirect != cmd_num_tokens[0] - 2) {
                    fprintf(stderr, "An error has occurred: Invalid redirection syntax\n");
                    continue;
                }
            }
        }

        int pipe_fds[MAX_COMMANDS - 1][2];
        for (int i = 0; i < num_commands - 1; i++) {
            if (pipe(pipe_fds[i]) < 0) {
                fprintf(stderr, "An error has occurred: Cannot create pipe \n");
                continue;
            }
        }

        pid_t pids[MAX_COMMANDS];
        for (int c = 0; c < num_commands; c++) {
            char full_path[MAX_LINE];
            int found = 0;
            for (int i = 0; i < num_paths; i++) {
                snprintf(full_path, sizeof(full_path), "%s/%s", paths[i], cmd_tokens[c][0]);
                if (access(full_path, X_OK) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                snprintf(full_path, sizeof(full_path), "%s", cmd_tokens[c][0]);
                if (access(full_path, X_OK) == 0) {
                    found = 1;
                }
            }
            if (!found) {
                fprintf(stderr, "An error has occurred: Command not found\n");
                for (int i = 0; i < num_commands - 1; i++) {
                    close(pipe_fds[i][0]);
                    close(pipe_fds[i][1]);
                }
                break;
            }

            pids[c] = fork();
            if (pids[c] == 0) {
                // Child process
                if (c > 0) {
                    dup2(pipe_fds[c-1][0], STDIN_FILENO);
                }
                if (c < num_commands - 1) {
                    dup2(pipe_fds[c][1], STDOUT_FILENO);
                }
                if (c == num_commands - 1 && redirect != -1) {
                    char* output_file = cmd_tokens[0][redirect + 1];
                    int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0) {
                        fprintf(stderr, "An error has occurred: Cannot open output file\n");
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                    cmd_tokens[0][redirect] = NULL;
                }

                for (int i = 0; i < num_commands - 1; i++) {
                    close(pipe_fds[i][0]);
                    close(pipe_fds[i][1]);
                }

                execv(full_path, cmd_tokens[c]);
                fprintf(stderr, "An error has occurred: Failed to execute\n");
                exit(1);
            } else if (pids[c] < 0) {
                fprintf(stderr, "An error has occurred: Fork failed \n");
                for (int i = 0; i < num_commands - 1; i++) {
                    close(pipe_fds[i][0]);
                    close(pipe_fds[i][1]);
                }
            }
        }

        for (int i = 0; i < num_commands - 1; i++) {
            close(pipe_fds[i][0]);
            close(pipe_fds[i][1]);
        }

        // Wait for all children
        for (int c = 0; c < num_commands; c++) {
            if (pids[c] > 0) {
                current_child = pids[c];
                int status;
                while (waitpid(pids[c], &status, 0) < 0) {
                    if (errno != EINTR) {
                        fprintf(stderr, "An error has occurred: Waitpid failed\n");
                        break;
                    }
                }
                current_child = -1;
            }
        }
cleanup_tokens:
        for (int c = 0; c < num_commands; c++) {
            free_tokens(cmd_tokens[c], cmd_num_tokens[c]);
        }
    }

    // Cleanup
    for (int i = 0; i < num_paths; i++) {
        free(paths[i]);
    }
    free(paths);
    
    /* Cleanup command history */
    if (command_history) {
        for (int i = 0; i < history_count && i < MAX_HISTORY; i++) {
            if (command_history[i]) {
                free(command_history[i]);
            }
        }
        free(command_history);
    }
    
    /* Cleanup aliases */
    for (int i = 0; i < alias_count; i++) {
        if (aliases[i].name) free(aliases[i].name);
        if (aliases[i].command) free(aliases[i].command);
    }
    
    if (input != stdin) {
        fclose(input);
    }

    return 0;
}
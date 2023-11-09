# Unix Shell (Wish)


## Shell Description

The shell implemented in this project acts as a command line interpreter (CLI) - a simplified version of the shells used daily in Unix systems. The basic functionality involves receiving user input commands, executing those commands, and prompting for further user input upon completion.

### Specifications

### Basic Shell: Wish

The core shell, named "wish" (Wisconsin Shell), operates in an interactive loop. It continuously prompts users with `wish>`, parses input commands, executes them, and awaits new commands until the user types "exit".

### Modes of Operation

The shell supports two modes:
- **Interactive Mode**: Allows users to type commands directly.
- **Batch Mode**: Reads input commands from a batch file. In batch mode, no prompt (`wish>`) is displayed.

### Structure

The shell operates in a while loop, fetching user input via `getline()`. It executes commands based on the input. It supports both interactive and batch modes and gracefully exits upon reaching the end-of-file marker (EOF).

### Program Execution

The shell utilizes functions like `fork()`, `exec()`, and `wait()/waitpid()` to execute commands. It mainly uses `execv` for command execution and handles errors if the command does not exist.

### Paths

The shell identifies command locations using a set of directories specified in the "path" variable. Initially set to `/bin`, it searches these directories to find executables and create a new process to run them.

### Built-in Commands

The shell implements three built-in commands:
- **exit**: Terminates the shell.
- **cd**: Changes the directory.
- **path**: Sets the search path for executables.

### Redirection

The shell supports standard output redirection. For instance, it redirects the output of a command to a file using the `>` symbol.

### Parallel Commands

The shell allows users to launch parallel commands using the ampersand (`&`) operator.

### Program Errors

An error message is displayed to stderr for encountered errors, allowing the shell to continue processing.

## Usage

### Building and Running

To compile the shell:

```bash
gcc -o wish wish.c
```

# mysh_MyShell
A Custom Shell Implementation
# mysh - A Custom Shell Implementation

## Overview

`mysh` is a basic shell implementation written in C that supports a limited set of built-in commands, variable handling, and simple input/output operations. It mimics certain functionalities of common Unix shells, allowing users to define variables, execute commands, and manipulate files and directories.

## Files

- `` - The main shell implementation. Handles command input, tokenization, and execution of built-in commands.
- ``** / **`` - Defines and implements built-in commands such as `echo`, `ls`, `cd`, `cat`, and `wc`.
- ``** / **`` - Provides functionality for setting, getting, and expanding shell variables.
- ``** / **`` - Contains functions for message display, error reporting, and input tokenization.

## Features

1. **Built-in Commands**

   - `echo`: Displays messages with variable expansion support.
   - `ls`: Lists files and directories, with optional filtering and recursive listing.
   - `cd`: Changes the current working directory.
   - `cat`: Displays the content of a file.
   - `wc`: Counts words, characters, and lines of a file.

2. **Variable Handling**

   - Allows setting variables using `key=value` syntax.
   - Supports variable expansion using `$key` within commands.

3. **Error Handling**

   - Provides descriptive error messages for unsupported commands and invalid operations.

## Compilation

Compile the project using the following command:

```
gcc -o mysh mysh.c builtins.c io_helpers.c variables.c -Wall -Werror
```

## Usage

Start the shell by running:

```
./mysh
```

Example commands:

```
mysh$ echo Hello World
Hello World

mysh$ name=Alice
mysh$ echo Hello $name
Hello Alice

mysh$ ls
file1.txt  file2.c  mysh
```

## Future Improvements

- Adding support for pipes (`|`) and file redirection (`>` and `<`).
- Implementing more built-in commands (e.g., `mkdir`, `rm`).
- Enhancing variable handling with support for nested or chained variables.

## License

This project is licensed under the MIT License.


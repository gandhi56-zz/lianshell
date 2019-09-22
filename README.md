# AnshEll

## Project description
AnshEll is a UNIX-based shell program that supports the following features:
1. Support for built-in commands as defined the environment variables.
2. Runs external programs from any specified location in the file system.
3. Sequential execution of multiple processes.
4. Redirection of output from a process to a file.

## Issues
1. Run processes in the background without streaming characters into STDIN.
2. Fix a2path.
3. Implement piping of output from one program to another.
4. Signal handling for keyboard.
5. Check if the input command is available for use, do not fork if the service is unavailable.
6. Buffer cleanup routine.
7. Auto-completion of commands.
8. Use ncurses.

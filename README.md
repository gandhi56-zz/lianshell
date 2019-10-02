# AnshEll

![](images/dragonshell.png)

## Project description
AnshEll is a UNIX-based shell program that supports the following features:
1. Support for built-in commands as defined the environment variables.
2. Runs external programs from any specified location in the file system.
3. Sequential execution of multiple processes.

## Issues
1. Run processes in the background without streaming characters into STDIN.
2. Redirection of output from a process to a file.
3. Implement piping of output from one program to another.
4. Signal handling for keyboard.
5. Check if the input command is available for use, do not fork if the service 6s unavailable.
7. Buffer cleanup routine.
8. Auto-completion of commands.
9. Use ncurses.
10. TERM environment variable is shown to be not set, unable to run 'clear'.

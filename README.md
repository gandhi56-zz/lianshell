# anshell

## Issues
1. Reorganize the program flow, seperate the jobs first and then process each job.
2. Fix external program run by execv, there is a potential memory leak because of the dynamic memory.
3. Run programs in the background.
4. Implement redirection '>'.
5. Implement piping.
6. Buffer cleanup routine.


//#define beta
#define SHELL 			"dragonshell: "
#define MAX_LEN			120
#define MAX_JOBS		20
#define NUM_ARGS		20
#define ENV_LEN			64
#define FNAME_SIZE		32
#define getmin(a,b) 	((a)<(b)?(a):(b))

// error codes
#define VALID_INSTR		0
#define E_REDIR			1
#define E_PIPE			1<<1

#include <map>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>	// system calls
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <signal.h>

// global variables
pid_t childPid = 0;

void display_splash(){
	printf("                                   ______________                                           					\n");
	printf("                             ,===:'.,            `-._                                       					\n");
	printf("                                  `:.`---.__         `-._                                   					\n");
	printf("                                    `:.     `--.         `.                                 					\n");
	printf("                                      \\.        `.         `.                               					\n");
	printf("                              (,,(,    \\.         `.   ____,-`.,                            					\n");
	printf("                           (,'     `/   \\.   ,--.___`.'                                     					\n");
	printf("                       ,  ,'  ,--.  `,   \\.;'         `                                     					\n");
	printf("                        `{D, {    \\  :    \\;                                                					\n");
	printf("                          V,,'    /  /    //                                                					\n");
	printf("                          j;;    /  ,' ,-//.    ,---.      ,                                					\n");
	printf("                          \\;'   /  ,' /  _  \\  /  _  \\   ,'/                                					\n");
	printf("                                \\   `'  / \\  `'  / \\  `.' /                                 					\n");
	printf("                                 `.___,'   `.__,'   `.__,'                                  					\n");
	printf("                                                                                            					\n");
	printf("__      __   _                    _         ___                           ___ _        _ _ 						\n");
	printf("\\ \\    / /__| |__ ___ _ __  ___  | |_ ___  |   \\ _ _ __ _ __ _ ___ _ _   / __| |_  ___| | | 					\n");
	printf(" \\ \\/\\/ / -_) / _/ _ \\ '  \\/ -_) |  _/ _ \\ | |) | '_/ _` / _` / _ \\ ' \\  \\__ \\ ' \\/ -_) | | 			\n");
	printf("  \\_/\\_/\\___|_\\__\\___/_|_|_\\___|  \\__\\___/ |___/|_| \\__,_\\__, \\___/_||_| |___/_||_\\___|_|_|			\n");
	printf("                                                         |___/													\n");
	printf("																												\n");
	printf("																												\n");
	printf("																												\n");
}

void print_arr(char* arr[], int cmdLen, const char* cap){
	printf("-------------\n");
	for (int i =0 ; i < cmdLen; ++i){
		printf("%s[%d] = %s\n", cap, i, arr[i]);
	}
	printf("-------------\n");
}

bool cmd_exists(const char* cmd, char* varPath, char* path){
	/*
		TODO use tokenize instead, more efficient
	*/
	if (!path)
		return false;
	if(strchr(cmd, '/')) {
		// if cmd starts with a '/', run access directly
        return access(cmd, X_OK)==0;
    }

    char *buf = (char*)malloc(strlen(path)+strlen(cmd)+3);

    for(; *path; ++path) {
        char *p = buf;
        for(; *path && *path!=':'; ++path,++p) {
            *p = *path;
        }
        if(p==buf) *p++='.';
        if(p[-1]!='/') *p++='/';
        strcpy(p, cmd);
		//printf("trying %s\n", buf);
        if(access(buf, X_OK)==0) {
        	strcpy(varPath, buf);
            free(buf);
            return true;
        }
        if(!*path) break;
    }
    // not found
    free(buf);
    return false;
}

/**
 * @brief Tokenize a C string
 *
 * @param str - The C string to tokenize
 * @param delim - The C string containing delimiter character(s)
 * @param argv - A char* array that will contain the tokenized strings
 * Make sure that you allocate enough space for the array.
 */
int tokenize(char* str, const char* delim, char ** argv) {
	char* token;
	token = strtok(str, delim);
	int count = 0;
	for(size_t i = 0; token != NULL; ++i){
		argv[i] = token;
		token = strtok(NULL, delim);
		count++;
	}
	return count;
}

void change_dir(char* cmd[NUM_ARGS], int& buffLen){
	if (buffLen < 2){
		perror("expected argument to chdir");
		return;
	}
	if (chdir(cmd[1]) == -1){
		perror("unable to run chdir");
	}
}

void show_path(char* env){
	printf("Current PATH: %s\n", env);
}

void update_path(char* env, const char* var, bool overwrite){
	if (!overwrite){
		if (strlen(env) >= 2)
			strcat(env, ":");
		strcat(env, var);
	}
	else{
		strcpy(env, var);
	}
}

void output_redirection(char* arg, char* env){
	// check if this is a background process
	bool runInBackground = (bool)(strchr(arg, '&') != NULL);
	char* cmd[NUM_ARGS];
	int cmdLen = tokenize(arg, ">", cmd);
	cmd[cmdLen] = NULL;

	// print_arr(cmd, cmdLen, "cmd");
	// cmd[0] = process
	// cmd[1] = output file name

	// check if this command exists
	char* process[strlen(cmd[0])];
	int processLen = tokenize(cmd[0], " ", process);
	char varPath[MAX_LEN];
	if (!cmd_exists(process[0], varPath, env)){
		printf("%s: command not found in PATH during redirection\n", cmd[0]);
		return;
	}
	process[0] = varPath;
	process[processLen] = NULL;

	// execute process
	int fd = -1;

	if (runInBackground){
		pid_t pid = fork();
		if (pid == 0){
			// strip whitespaces from the left
			int i = 0;
			while (cmd[1][i] == ' ')	i++;
			char* fname[NUM_ARGS];
			char* ffname[NUM_ARGS];
			tokenize(cmd[1], "&", fname);
			tokenize(fname[0], " ", ffname);
			fd = open(ffname[0], O_CREAT | O_RDWR, 0666);
			dup2(fd, STDOUT_FILENO);
			if (execve(process[0], process, NULL) == -1){
				perror("execve");
				_exit(-1);
			}
			close(fd);
		}
		else if (pid > 0){
			waitpid(pid, NULL, 0);
		}
		else{
			printf("Child process could not be created.\n");
		}
	}
	else{
		pid_t pid = fork();
		if (pid == 0){
			// strip whitespaces from the left
			int i = 0;
			while (cmd[1][i] == ' ')	i++;
			char fname[strlen(cmd[1]) - i + 1];
			for (int k = 0; k < strlen(cmd[1])-i; ++k){
				fname[k] = cmd[1][k+i];
			}
			fname[strlen(cmd[1]) - i] = (char)NULL;
			fd = open(fname, O_CREAT | O_RDWR, 0666);
			dup2(fd, STDOUT_FILENO);
			if (execve(process[0], process, NULL) == -1){
				perror("execve");
				_exit(-1);
			}
			close(fd);
		}
		else if (pid > 0){
			waitpid(pid, NULL, 0);
		}
		else{
			printf("Child process could not be created.\n");
		}
	}
}

void run_pipe(char* arg, char* env){
	// printf("piping... %s\n", arg);
	bool runInBackground = (bool)(strchr(arg, '&') != NULL);
	char* cmd[NUM_ARGS];
	int cmdLen = tokenize(arg, "|", cmd);
	cmd[cmdLen] = NULL;

	// print_arr(cmd, cmdLen, "cmd");

	// parse and check if process1 exists
	char* process1[strlen(cmd[0])];
	int processLen1 = tokenize(cmd[0], " ", process1);
	char varPath1[MAX_LEN];
	if (!cmd_exists(process1[0], varPath1, env)){
		printf("%s: command not found in PATH during redirection\n", cmd[0]);
		return;
	}
	process1[0] = varPath1;
	process1[processLen1] = NULL;
	// print_arr(process1, processLen1, "process1");

	// parse and check if process2 exists
	char* process2[strlen(cmd[1])];
	int processLen2 = tokenize(cmd[1], " ", process2);
	char varPath2[MAX_LEN];
	if (!cmd_exists(process2[0], varPath2, env)){
		printf("%s: command not found in PATH during redirection\n", cmd[1]);
		return;
	}
	process2[0] = varPath2;
	process2[processLen2] = NULL;

	// get rid of '&'
	if (runInBackground)
		process2[processLen2-1] = NULL;
	// print_arr(process2, processLen2, "process2");

	// create a pipe
	int fd[2];
	if (pipe(fd) == -1){
		perror("pipe");
		return;
	}

	// run processes
	pid_t pid1;
	pid_t pid2 = fork();
	if (pid2 == 0){
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);
		// print_arr(process2, processLen2, "process2");
		execve(process2[0], process2, NULL);
		perror("execve");
		_exit(0);
	}
	else if (pid2 > 0){
		// close(fd[1]);
		// waitpid(pid2, NULL, 0);
		// printf("done executinng process1 %d\n", pid);
		pid1 = fork();
		if (pid1 == 0){
			// print_arr(process1, processLen1, "process1");
			close(fd[0]);
			dup2(fd[1], STDOUT_FILENO);
			close(fd[1]);
			execve(process1[0], process1, NULL);
			_exit(0);
		}
		else if (pid1 > 0){
			// printf("starting process2 %d\n", pid1);
			// waitpid(pid1, NULL, 0);
			// printf("done both processes\n");
		}
	}
	else{
		printf("Child process could not be created.\n");
	}
	// printf("back to pavilion\n");
	close(fd[0]);
	close(fd[1]);
	wait(NULL);
	wait(NULL);
}

void run_process(char* cmd[NUM_ARGS], int cmdLen, char* env){
	// check if this command exists
	char varPath[MAX_LEN];
	if (!cmd_exists(cmd[0], varPath, env)){
		printf("%s: command not found in PATH\n", cmd[0]);
		return;
	}
	cmd[0] = varPath;

	// check if this process will run in the background
	bool runInBackground = false;
	if (strcmp(cmd[cmdLen-1], "&") == 0){
		runInBackground = true;
		cmd[cmdLen-1] = NULL;
	}

	if (runInBackground){
		// execute process
		pid_t pid = fork();
		if (pid == 0){
			close(STDOUT_FILENO);
			close(STDERR_FILENO);
			if (execve(cmd[0], cmd, NULL) == -1){
				perror("execve");
			}
		}
		else if (pid > 0){
			printf("PID %d is running in the background\n", pid);
			// waitpid(pid, NULL, 0);
		}
		else{
			printf("Child process could not be created.\n");
		}
	}
	else{
		// execute process
		pid_t pid = fork();
		if (pid == 0){
			if (execve(cmd[0], cmd, NULL) == -1){
				perror("execve");
			}
		}
		else if (pid > 0){
			childPid = pid;
			waitpid(pid, NULL, 0);
		}
		else{
			printf("Child process could not be created.\n");
		}
	}

}

void run_cmd(char* arg, char* env){
	if (strchr(arg, '>') != NULL){
		output_redirection(arg, env);
	}
	else if (strchr(arg, '|') != NULL){
		run_pipe(arg, env);
	}
	else{
		char* cmd[NUM_ARGS];
		int cmdLen = tokenize(arg, " ", cmd);
		cmd[cmdLen] = NULL;

		if (strcmp(cmd[0], "cd") == 0){
			change_dir(cmd, cmdLen);
		}
		else if (strcmp(cmd[0], "pwd") == 0){
			char* cwd = get_current_dir_name();
			printf("%s\n", cwd);
			free(cwd);
		}
		else if (strcmp(cmd[0], "$PATH") == 0){
			show_path(env);
		}
		else if (strcmp(cmd[0], "a2path") == 0){
			char* args[ENV_LEN];
			int argLen = tokenize(cmd[1], ":", args);
			for (int i = 1; i < argLen; ++i){
				update_path(env, args[i], strcmp(args[0], "$PATH"));
			}
		}
		else if (strcmp(cmd[0], "exit") == 0){
			printf("Thanks for using the dragonshell.\n");
			_exit(0);
		}
		else {
			run_process(cmd, cmdLen, env);
		}
	}
}

void strip_newline(char* arg, int len){
	int i = 0;
	while (i < len and arg[i] != '\n')
		i++;
	if (i < len)
		arg[i] = (char)NULL;
}

int cmd_ok(char* job, int jobLen){
	// printf("checking job:%s\n", job);
	char* args[NUM_ARGS];
	int argsLen = tokenize(job, " ", args);
	// print_arr(args, argsLen, "args");

	for (int i =0 ; i < argsLen; ++i){
		if (strcmp(args[i], ">") == 0 and (i == argsLen-1)){
			return E_REDIR;
		}
		else if (strcmp(args[i], "|") == 0 and i == argsLen-1){
			return E_PIPE;
		}
	}

	return VALID_INSTR;
}

// signal handling ----------------------------------------------------------------
void sigint_handler(int signum){
	// send sigint to child processes
	// printf("\n");
	printf("\n");
}

void sigtstp_handler(int signum){
	printf("\n");
}
// --------------------------------------------------------------------------------

int main(int argc, char **argv) {
	// print the string prompt without a newline, before beginning to read
	// tokenize the input, run the command(s), and print the result
	// do this in a loop
	char 	cmd	[MAX_LEN];
	char* 	jobs[MAX_JOBS];
	char 	env	[ENV_LEN * FNAME_SIZE];
	struct sigaction sa_int, sa_tstp;
	sa_int.sa_flags = 0;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_handler = sigint_handler;

	sa_tstp.sa_flags = 0;
	sigemptyset(&sa_tstp.sa_mask);
	sa_tstp.sa_handler = sigtstp_handler;

	update_path(env, "/usr/bin/", true);
	update_path(env, "/bin/", false);
	display_splash();

	// handle signals here
	if (sigaction(SIGINT, &sa_int, NULL) == -1 or sigaction(SIGTSTP, &sa_tstp, NULL) == -1){
		perror("sigaction");
	}

	while (1){
		
		#ifdef beta
			printf("dragonshell: %s > ", get_current_dir_name());
		#else
			printf("dragonshell [%d]: > ", getpid());
		#endif

		childPid = 0;
		fgets(cmd, MAX_LEN, stdin);
		// memset(jobs, 0, MAX_JOBS * sizeof(char));
		tokenize(cmd, "\n", jobs);
		int numJobs = tokenize(cmd, ";", jobs);
		for (int i = 0; i < numJobs; ++i){
			if (strcmp(jobs[i], "\n") == 0 or strcmp(jobs[i], " ") == 0)
				continue;
			char currJob[NUM_ARGS] = {0};
			strcpy(currJob, jobs[i]);
			// printf("before %s\n", currJob);
			int cmdStatus = cmd_ok(jobs[i], strlen(jobs[i]));
			// printf("after %s\n", currJob);
			// printf("status = %d\n", cmdStatus);
			if (cmdStatus == E_REDIR or cmdStatus == E_PIPE){
				char tmp[MAX_LEN];
				fgets(tmp, MAX_LEN, stdin);
				strip_newline(tmp, strlen(tmp));
				strcat(currJob, tmp);
				// printf("after %s\n", currJob);
			}
			run_cmd(currJob, env);
		}
		// printf("childPid = %d\n", childPid);
	}

	return 0;
}

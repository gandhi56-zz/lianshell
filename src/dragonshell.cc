
//#define beta
#define SHELL 			"dragonshell: "
#define MAX_LEN			120
#define MAX_JOBS		20
#define NUM_ARGS		20
#define ENV_LEN			64
#define FNAME_SIZE		32
#define getmin(a,b) 	((a)<(b)?(a):(b))

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

void run_child_bg(char* cmd[NUM_ARGS], int& buffLen){
	printf("Putting job %d in the background.\n", getpid());
	// int cmdLen = 0;
	// for (int i = 0; i < buffLen; ++i){
	// 	cmdLen += strlen(cmd[i]);
	// }

	// char proc[cmdLen];
	// int k =0 ;
	// for (int i = 0; i < buffLen; ++i){
	// 	strcpy(&proc[k], cmd[i]);
	// }

	// //close(STDIN_FILENO);
	// //close(STDOUT_FILENO);
	// //close(STDERR_FILENO);

	// char* child[buffLen];
	// for (int i =0 ; i < buffLen-1; ++i)
	// 	child[i] = cmd[i];
	// child[buffLen-1] = NULL;

	// int x = open("/dev/null", O_RDWR);
	// printf("%d\n", x);
	// dup(x);
	// dup(x);
	// execvp(child[0], child);
	// _exit(1);



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
	printf("piping...\n");
	// check if this is a background process

	int fd[2];
	char str[] = "hello, world 1234\n";
	char readBuffer[100];
	pipe(fd);
	pid_t pid = fork();
    if (pid == 0){
        // child process
        close(fd[0]);
        write(fd[1], str, (strlen(str)+1));
        _exit(0);
    }
    else{
        close(fd[1]);
        int nbytes = read(fd[0], readBuffer, sizeof(readBuffer));
        printf("received: %s", readBuffer);
    }

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
			print_arr(cmd, cmdLen, "cmd");
			print_arr(args, argLen, "args");
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

// signal handling ----------------------------------------------------------------
void sigint_handler(int signum){
	// send sigint to child processes
	if (childPid){
		printf("killing child\n");
		if (kill(childPid, SIGINT) == -1)
			perror("kill");
	}
}

void sigtstp_handler(int signum){

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
		tokenize(cmd, "\n", jobs);
		int numJobs = tokenize(cmd, ";", jobs);
		for (int i = 0; i < numJobs; ++i){
			run_cmd(jobs[i], env);
		}
		printf("childPid = %d\n", childPid);
	}

	return 0;
}

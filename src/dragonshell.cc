
// TODO when an invalid command is entered, a child process is created executing the dragonshell from fork()...

//#define beta
#define SHELL 			"dragonshell: "
#define MAX_LEN			120
#define MAX_JOBS		20
#define NUM_ARGS		20
#define ENV_LEN			64
#define getmin(a,b) 	((a)<(b)?(a):(b))

#include <vector>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>	// system calls
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <iostream>
#include <fcntl.h>


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

void change_dir(char* buff[NUM_ARGS], int& buffLen){
	if (buffLen < 2){
		perror("expected argument to chdir");
		return;
	}
	if (chdir(buff[1]) == -1){
		perror("unable to run chdir");
	}
}

void update_path(char* buff[NUM_ARGS], int& buffLen, std::vector<const char*>& env){
	/*
		TODO - grammar check; does the given variable even exist?
	*/
	char* pathArgs[NUM_ARGS];
	buffLen = tokenize(buff[1], ":", pathArgs);
	if (buffLen == 0){
		printf("Please provide the argument to run the command.\n");
		return;
	}
	if (strcmp(pathArgs[0], "$PATH") == 0 or pathArgs[0][0] == '$'){
		// append
		env.push_back(pathArgs[1]);
	}
	else{
		// overwrite
		env.clear();
		env.push_back(pathArgs[0]);
	}
}

void run_child_bg(char* buff[NUM_ARGS], int& buffLen){
	printf("Putting job %d in the background.\n", getpid());
	int cmdLen = 0;
	for (int i = 0; i < buffLen; ++i){
		cmdLen += strlen(buff[i]);
	}

	char proc[cmdLen];
	int k =0 ;
	for (int i = 0; i < buffLen; ++i){
		strcpy(&proc[k], buff[i]);
	}

	//close(STDIN_FILENO);
	//close(STDOUT_FILENO);
	//close(STDERR_FILENO);

	char* child[buffLen];
	for (int i =0 ; i < buffLen-1; ++i)
		child[i] = buff[i];
	child[buffLen-1] = NULL;

	int x = open("/dev/null", O_RDWR);
	printf("%d\n", x);
	dup(x);
	dup(x);
	execvp(child[0], child);
	_exit(1);
}

void run_child(char* buff[NUM_ARGS], int& procLen){
	char* proc[procLen];
	for (int i = 0; i < procLen; ++i){
		proc[i] = buff[i];
	}
	proc[procLen] = NULL;
	execvp(proc[0], proc);
}

void run_cmd(char* arg, std::vector<int>& backProc, std::vector<const char*>& env){
	/*
		arg: raw command from user input, ignoring '\n'
		procArgs:
		buff: 
	*/
	
	
	char* buff[NUM_ARGS];
	int len = tokenize(arg, " ", buff);
	buff[len] = NULL;
	printf("arg = %s\n", arg);

	// char* pipeArgs[NUM_ARGS];
	// int pipeLen = 0;
	// pipeLen = tokenize(procArgs, "|", pipeArgs);
	// for (int i = 0; i < pipeLen; ++i){
	// 	printf("pipeArgs[%d] = %s\n", i,  pipeArgs[i]);
	// }

	// change directory
	if (strcmp(buff[0], "cd") == 0){
		change_dir(buff, len);
	} 
	else if (strcmp(buff[0], "$PATH") == 0){
		printf("Current PATH: ");
		for (int i = 0; i < env.size(); ++i){
			printf("%s", env[i]);
			if (i < env.size()-1)
				printf(":");
		}
		printf("\n");
	}
	else if (strcmp(buff[0], "a2path") == 0){
		update_path(buff, len, env);
	}
	else if (strcmp(buff[0], "exit") == 0){
		printf("Thanks for using the dragonshell.\n");
		_exit(0);
	}

	// execute command
	else if (len){
		
		int procLen = len+1;

		pid_t pid = fork();
		if (pid == 0){
			if (buff[len-1][0] == '&'){
				run_child_bg(buff, len);
			}
			else{
				run_child(buff, procLen);
			}
		}

		else if (pid > 0){
			if (buff[len-1] != "&"){
				waitpid(pid, NULL, 0);
			}
		}
		else{
			printf("Child process could not be created.\n");
		}

	}
}

int main(int argc, char **argv) {
	// print the string prompt without a newline, before beginning to read
	// tokenize the input, run the command(s), and print the result
	// do this in a loop
	char 	cmd[MAX_LEN];
	char* 	jobs[MAX_JOBS];
	std::vector<const char*> env({"/bin/", "/usr/bin/"});
	std::vector<int> backProc;
	
	display_splash();
	
	while (1){
		
		#ifdef beta
			printf("dragonshell: %s > ", get_current_dir_name());
		#else
			printf("dragonshell [%d]: > ", getpid());
		#endif

		fgets(cmd, MAX_LEN, stdin);
		tokenize(cmd, "\n", jobs);
		int numJobs = tokenize(cmd, ";", jobs);
		for (int i = 0; i < numJobs; ++i){
			run_cmd(jobs[i], backProc, env);
		}

	}

	return 0;
}

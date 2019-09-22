
// TODO when an invalid command is entered, a child process is created executing the dragonshell from fork()...

//#define beta
#define SHELL 			"dragonshell: "
#define MAX_LEN			120
#define MAX_JOBS		20
#define NUM_ARGS		20
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
int tokenize_c(char* str, const char* delim, char ** argv) {
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
		printf("%s expected argument to \"cd\"\n", SHELL);
		return;
	}
	if (chdir(buff[1]) == -1){
		printf("%s No such file or directory\n", SHELL);
	}
}

void update_path(char* buff[NUM_ARGS], int& buffLen){
	// TODO is this implemented correctly?
	char* env = getenv("PATH");
	char* pathArgs[NUM_ARGS];
	buffLen = tokenize_c(buff[1], ":", pathArgs);
	for (int i = 0; i < buffLen; ++i){
		printf("pathArgs[%d] = %s\n", i, pathArgs[i]);
	}
	if (buffLen == 0){
		printf("Please provide the argument to run the command.\n");
		return;
	}
	printf("len = %d\n", buffLen);
	if (strcmp(pathArgs[0], "$PATH") == 0 or pathArgs[0][0] == '$'){
		// append
		//strcat(env, ":");
		//strcat(env, pathArgs[1]);
		printf("0 - adding %s...\n", pathArgs[1]);
		setenv("PATH", pathArgs[1], 0);
	}
	else{
		// overwrite
		printf("1 - adding %s...\n", pathArgs[0]);
		setenv("PATH", pathArgs[0], 1);
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

void run_child(char* buff[NUM_ARGS], int& procLen, int& fileDcpt){
	char* proc[procLen];
	for (int i = 0; i < procLen; ++i){
		proc[i] = buff[i];
	}
	proc[procLen] = NULL;
	if (fileDcpt > 0){
		dup2(fileDcpt, 1);	// redirect output to file
	}
	execvp(proc[0], proc);
}

void run_cmd(char* arg, std::vector<int>& backProc){
	char* buff[NUM_ARGS];
	int len = tokenize_c(arg, "\n", buff);
	len = tokenize_c(arg, " ", buff);

	// expected NULL by system calls
	buff[len] = NULL;

	// change directory
	if (strcmp(buff[0], "cd") == 0){
		change_dir(buff, len);
	}
	else if (strcmp(buff[0], "$PATH") == 0){
		printf("Current PATH: %s\n", getenv("PATH"));
	}
	else if (strcmp(buff[0], "a2path") == 0){
		update_path(buff, len);
	}
	else if (strcmp(buff[0], "exit") == 0){
		printf("Thanks for using the dragonshell.\n");
		_exit(0);
	}

	// execute command
	else if (len){

		// check if this process redirects
		int redirIndex = 0;
		int fileDcpt = 0;
		int procLen = len+1;
		bool openFile = false;
		if (len >= 3){
			while (redirIndex < len){
				if (buff[redirIndex][0] == '>'){
					openFile = true;
					break;
				}
				redirIndex++;
			}
			procLen = redirIndex;
		}
		
		pid_t pid = fork();
		if (pid == 0){

			// set up the target file for redirection, if specified
			if (openFile){
				fileDcpt = open(buff[redirIndex+1], O_CREAT | O_RDWR);
				printf("opening file descriptor %s\n", buff[redirIndex+1]);
				printf("redirIndex = %d\n", redirIndex);
			}

			if (buff[len-1][0] == '&'){
				run_child_bg(buff, len);
			}
			else{
				run_child(buff, procLen, fileDcpt);
			}
		}

		else if (pid > 0){
			if (buff[len-1] != "&"){
				waitpid(pid, NULL, 0);
			}
			if (fileDcpt > 0)
				close(fileDcpt);
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
	std::vector<int> backProc;
	
	display_splash();
	
	while (1){
		
		#ifdef beta
			printf("dragonshell: %s > ", get_current_dir_name());
		#else
			printf("dragonshell [%d]: > ", getpid());
		#endif

		fgets(cmd, MAX_LEN, stdin);
		tokenize_c(cmd, "\n", jobs);
		int numJobs = tokenize_c(cmd, ";", jobs);
		for (int i = 0; i < numJobs; ++i){
			run_cmd(jobs[i], backProc);
		}

	}

	return 0;
}

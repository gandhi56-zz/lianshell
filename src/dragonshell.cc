
//#define beta
#define SHELL 			"dragonshell: "
#define MAX_LEN			120
#define MAX_JOBS		20
#define NUM_ARGS		20

#include <vector>
#include <cstring>
//#include <cstring>

#include <unistd.h>	// system calls
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>

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
 * @brief Tokenize a string 
 * 
 * @param str - The string to tokenize
 * @param delim - The string containing delimiter character(s)
 * @return std::vector<std::string> - The list of tokenized strings. Can be empty

std::vector<std::string> tokenize(const std::string &str, const char *delim) {
	char* cstr = new char[str.size() + 1];
	std::strcpy(cstr, str.c_str());

	char* tokenized_string = strtok(cstr, delim);

	std::vector<std::string> tokens;
	while (tokenized_string != NULL){
		tokens.push_back(std::string(tokenized_string));
		tokenized_string = strtok(NULL, delim);
	}
	delete[] cstr;

	return tokens;
}
 */

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

void run_cmd(char* arg, std::vector<int>& backProc){
	char* buff[NUM_ARGS];
	int len = tokenize_c(arg, "\n", buff);
	len = tokenize_c(arg, " ", buff);

	// expected nullptr by system calls
	buff[len] = nullptr;

	// print current working directory
	if (strcmp(buff[0], "pwd") == 0){
		printf("%s\n", get_current_dir_name());
	}

	// change directory
	else if (strcmp(buff[0], "cd") == 0){
		if (len >= 2){
			if (chdir(buff[1]) == -1){
				printf("%s No such file or directory\n", SHELL);
			}
		}
		else{
			printf("%s expected argument to \"cd\"\n", SHELL);
		}
	}
	else if (strcmp(buff[0], "$PATH") == 0){
		// TODO
	}
	else if (strcmp(buff[0], "a2path") == 0){
		// TODO
	}

	// call _exit system call
	else if (strcmp(buff[0], "exit") == 0){
		printf("Need to ensure all child processes have been terminated. ");
		printf("Besides, thanks for using the dragonshell.\n");
		_exit(0);
	}

	// execute command
	else if (len){
		
		pid_t pid = fork();
		if (pid > 0){
			if (buff[len-1] == "&"){
				
			}
			else{
				while (1){
					int status;
					int wpid = wait(&status);
					if (wpid == pid){
						break;
					}
					//waitpid(pid, &status, 0);
				}
			}
		}
		else if (pid == 0){
			// child process
			execvp(buff[0], buff);
		}
		else{
			printf("%s Could not create child process.\n", SHELL);
			return;
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
			printf("dragonshell: > ");
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

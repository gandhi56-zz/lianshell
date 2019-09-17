
#define beta
#define SHELL "dragonshell: "

#include <vector>
#include <string>
#include <cstring>
#include <iostream>

#include <unistd.h>	// system calls
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>

#ifdef beta
	#include <dirent.h>	// for "ls" command, not required by assignment
#endif

void display_splash(){
	std::cout<< "                                   ______________                                           					"<<std::endl;
	std::cout<< "                             ,===:'.,            `-._                                       					"<<std::endl;
	std::cout<< "                                  `:.`---.__         `-._                                   					"<<std::endl;
	std::cout<< "                                    `:.     `--.         `.                                 					"<<std::endl;
	std::cout<< "                                      \\.        `.         `.                               					"<<std::endl;
	std::cout<< "                              (,,(,    \\.         `.   ____,-`.,                            					"<<std::endl;
	std::cout<< "                           (,'     `/   \\.   ,--.___`.'                                     					"<<std::endl;
	std::cout<< "                       ,  ,'  ,--.  `,   \\.;'         `                                     					"<<std::endl;
	std::cout<< "                        `{D, {    \\  :    \\;                                                					"<<std::endl;
	std::cout<< "                          V,,'    /  /    //                                                					"<<std::endl;
	std::cout<< "                          j;;    /  ,' ,-//.    ,---.      ,                                					"<<std::endl;
	std::cout<< "                          \\;'   /  ,' /  _  \\  /  _  \\   ,'/                                				"<<std::endl;
	std::cout<< "                                \\   `'  / \\  `'  / \\  `.' /                                 				"<<std::endl;
	std::cout<< "                                 `.___,'   `.__,'   `.__,'                                  					"<<std::endl;
	std::cout<< "                                                                                            					"<<std::endl;
	std::cout<< "__      __   _                    _         ___                           ___ _        _ _ 					"<<std::endl;
	std::cout<< "\\ \\    / /__| |__ ___ _ __  ___  | |_ ___  |   \\ _ _ __ _ __ _ ___ _ _   / __| |_  ___| | | 				"<<std::endl;
	std::cout<< " \\ \\/\\/ / -_) / _/ _ \\ '  \\/ -_) |  _/ _ \\ | |) | '_/ _` / _` / _ \\ ' \\  \\__ \\ ' \\/ -_) | | 		"<<std::endl;
	std::cout<< "  \\_/\\_/\\___|_\\__\\___/_|_|_\\___|  \\__\\___/ |___/|_| \\__,_\\__, \\___/_||_| |___/_||_\\___|_|_|		"<<std::endl;
	std::cout<< "                                                         |___/													"<<std::endl;
	std::cout<< "																												"<<std::endl;
	std::cout<< "																												"<<std::endl;
	std::cout<< "																												"<<std::endl;
}


/**
 * @brief Tokenize a string 
 * 
 * @param str - The string to tokenize
 * @param delim - The string containing delimiter character(s)
 * @return std::vector<std::string> - The list of tokenized strings. Can be empty
 */

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

/**
 * @brief Tokenize a C string
 *
 * @param str - The C string to tokenize
 * @param delim - The C string containing delimiter character(s)
 * @param argv - A char* array that will contain the tokenized strings
 * Make sure that you allocate enough space for the array.
 */
void tokenize_c(char* str, const char* delim, char ** argv) {
	char* token;
	token = strtok(str, delim);
	for(size_t i = 0; token != NULL; ++i){
		argv[i] = token;
		token = strtok(NULL, delim);
	}
}

int main(int argc, char **argv) {
	// print the string prompt without a newline, before beginning to read
	// tokenize the input, run the command(s), and print the result
	// do this in a loop
	
	display_splash();

	std::string cmd;
	std::vector<std::string> tokens;
	while (1){
		
		#ifdef beta
			std::cout << "dragonshell: " << get_current_dir_name() << " > ";
		#else
			std::cout << "dragonshell: > ";
		#endif

		getline(std::cin, cmd);
		if (cmd == "")
			continue;

		tokens = tokenize(cmd, " ");
		if (tokens[0] == "cd"){
			if (tokens.size() >= 2){
				if (chdir(tokens[1].c_str()) == -1){
					std::cout << SHELL << "No such file or directory" << std::endl;
				}
			}
			else{
				std::cout << SHELL << "expected argument to \"cd\"" << std::endl;
			}
		}
		else if (tokens[0] == "pwd"){
			std::cout << get_current_dir_name() << std::endl;
		}

		else if (tokens[0] == "$PATH"){
			// TODO
			char buff[40];
			if (getcwd(buff, sizeof(buff)) == nullptr){
				std::cout << SHELL << "Error occured when printing $PATH" << std::endl;
			}
			else{
				std::cout << buff << std::endl;
			}

		}
		else if (tokens[0] == "a2path"){
			// TODO
		}
		else if (tokens[0] == "exit"){
			std::cout << "Need to ensure all child processes have been terminated. ";
			std::cout << "Besides, thanks for using the dragonshell." << std::endl;
			break;
		}
		else if (tokens.size() > 0){
			pid_t pid = fork();
			#ifdef DEBUG
				std::cout << SHELL << pid << std::endl;
			#endif
			if (pid < 0){
				std::cout << SHELL << "Could not create child process." << std::endl;
			}
			else if (pid == 0){
				char** args = new char*[20];
				std::cout << "tokenizing " << cmd << std::endl;
				tokenize_c((char*)cmd.c_str(), " ", args);
				execvp(args[0], args);
				delete [] args;
			}
			else{
				wait(nullptr);
			}
		}
		else{
			continue;
		}
	}

	return 0;
}

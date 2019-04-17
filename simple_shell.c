//Author: Zihao Zhang
//Date: 4.8

#include<stdio.h> 
#include<string.h> 
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>


#define TRUE 1
#define MAXTOKEN 32 // max length of tokens to be supported 
#define MAXLINE 512 // max length of lines to be supported 
#define STD_INPUT 0
#define STD_OUTPUT 1


void test_print_tokens(int numTokens, char tokens[MAXLINE][MAXTOKEN]){
	for(int i = 0; i < numTokens; i++){
			printf("%s", tokens[i]);
			printf(" ");
		}
	printf("\n");
}


void test_print_commands(int numCommands, char commands[50][MAXLINE][MAXTOKEN]){
	for(int i = 0; i < numCommands; i++){
			test_print_tokens( 4 , commands[i]);			//STUB
	}
}


void type_prompt(){
	printf("shell: ");
}


void get_line(char line[MAXLINE]){
	fgets(line, MAXLINE, stdin);
	line[strlen(line)-1]='\0';
}


int split_line(char line[MAXLINE], char tokens[MAXLINE][MAXTOKEN]){

	char buffer[MAXTOKEN];
	int bufferPosition = 0;
	int tokenPosition = 0;
	char c;
	char op[2];
	op[0] = ' ';
	op[1] = '\0';

	for(int i = 0; i < strlen(line); i++){
		
		c = line[i];

		if(c == ' '){
			
			if(bufferPosition != 0){
				buffer[bufferPosition] = '\0';
				strcpy(tokens[tokenPosition], buffer);
				memset(buffer,0,strlen(buffer));
				bufferPosition = 0;
				tokenPosition++;
			}
		}else if( c == '<' || c == '>' || c == '|' || c == '&'){
		
			if(bufferPosition != 0){
				buffer[bufferPosition] = '\0';
			    strcpy(tokens[tokenPosition], buffer);
				memset(buffer,0,strlen(buffer));
				bufferPosition = 0;
				tokenPosition++;
			}
			op[0] = c;
			strcpy(tokens[tokenPosition], op);
			tokenPosition++;

		}else{
			
			buffer[bufferPosition] = c;
			bufferPosition++;

		}
	}
    
    if(bufferPosition != 0){
		buffer[bufferPosition] = '\0';
		strcpy(tokens[tokenPosition], buffer);
		memset(buffer,0,strlen(buffer));
		tokenPosition++;
	}

 	return tokenPosition;        // return the total number of tokens founded 
}


int split_tokens(int numTokens, char tokens[MAXLINE][MAXTOKEN], char commands[50][MAXLINE][MAXTOKEN]){
	int numCommands = 0;
	int tokenPosition = 0;

	if(numTokens == 0){
		return 0;
	}

	for(int i = 0; i < numTokens; i++){
		if(strcmp(tokens[i], "&") == 0){
			// do nothing
		}
		else if(strcmp(tokens[i], "|") != 0){
			strcpy( commands[numCommands][tokenPosition] , tokens[i] );
			tokenPosition++;
		}else{
			strcpy( commands[numCommands][tokenPosition] , "end" );

			numCommands++;
			tokenPosition = 0;
			strcpy( commands[numCommands][tokenPosition] , tokens[i] );
		}
	}
	strcpy( commands[numCommands][tokenPosition] , "end" );

	return numCommands + 1;
}


void execute_single_command(char tokens[MAXLINE][MAXTOKEN], bool isBackground){
	int status;
	char *args[MAXLINE];
	int numArgs = 0;
	int pid;
	bool redirectIn = false;
	bool redirectOut = false;
	char inputFile[MAXTOKEN];
	char outputFile[MAXTOKEN];

	int i = 0;
	while(strcmp(tokens[i],"end") != 0){

		if( strcmp(tokens[i], "<") == 0 ){			// redirect input
			redirectIn = true;
			strcpy(inputFile, tokens[++i]);
			i++;
		}
		else if( strcmp(tokens[i], ">") == 0 ){		// redirect output
			redirectOut = true;
			strcpy(outputFile, tokens[++i]);
			i++;
		}
		else{
        	args[numArgs] = tokens[i];
        	numArgs++;
        	i++;
		}

	}
	args[numArgs] = 0;

	pid = fork();
	if(pid == -1){
		perror("ERROR: fork failed!\n");
	}

	else if(pid == 0){
		// Child
		if (redirectIn == true){
			int fd0 = open(inputFile, O_RDONLY);
	        dup2(fd0, STD_INPUT);
	        close(fd0);
    	}
    	if (redirectOut == true){
	    	int fd1 = creat(outputFile, 0644);
	        dup2(fd1, STD_OUTPUT);
	        close(fd1);
    	}
		if(execvp(args[0], args) == -1){
    		printf("ERROR: %s\n", strerror(errno));
    	}

	}
	else{
		// Parent
		if(isBackground == false){
			//printf("11111\n");
			waitpid(-1, &status, 0);
		}else{
			//printf("22222\n");
			waitpid(-1, &status, WNOHANG);
		}
	}
}

// void run_pipe_command(int in, int out, char command[MAXLINE][MAXTOKEN]){
// 	char *args[MAXLINE];
// 	int numArgs = 0;
// 	bool redirectIn = false;
// 	bool redirectOut = false;
// 	char inputFile[MAXTOKEN];
// 	char outputFile[MAXTOKEN];

// 	int i = 0;
// 	while(strcmp(command[i],"end") != 0){
// 		if( strcmp(command[i], "<") == 0 ){			// redirect input
// 			redirectIn = true;
// 			strcpy(inputFile, command[++i]);
// 			i++;
// 		}
// 		else if( strcmp(command[i], ">") == 0 ){		// redirect output
// 			redirectOut = true;
// 			strcpy(outputFile, command[++i]);
// 			i++;
// 		}
// 		else{
// 	        args[numArgs] = command[i];
// 	        numArgs++;
// 	        i++;
// 	    }
// 	}
// 	args[numArgs] = 0;

// 	if(in != STD_INPUT){
// 		dup2(in,STD_INPUT);
//     	close(in);
// 	}else{
// 		if (redirectIn == true){
// 			int fd0 = open(inputFile, O_RDONLY);
// 	        dup2(fd0, STD_INPUT);
// 	        close(fd0);
//     	}
// 	}
	
//     if(out != STD_OUTPUT){
//     	dup2(out, STD_OUTPUT);
//     	close(out);
//     }else{
//     	if (redirectOut == true){
// 	    	int fd1 = creat(outputFile, 0644);
// 	        dup2(fd1, STD_OUTPUT);
// 	        close(fd1);
//     	}
//     }

//     if(execvp(args[0], args) == -1){
//     	 printf("ERROR: %s\n", strerror(errno));
//     }

// }


// void launch_pipes(int numCommands, char commands[50][MAXLINE][MAXTOKEN], bool isBackground){
// 	int pid;
// 	int fd[2];
// 	int status;
// 	int in = 0;
// 	int out = 1;

// 	// first command to the second last command
// 	for(int i = 0; i < numCommands - 1; i++){
// 		pipe(fd);
// 		pid = fork();
// 		if(pid == -1){
// 			printf("ERROR: %s\n", strerror(errno));
// 		}else if(pid == 0){
// 			// Child
// 			close(fd[0]);
// 			run_pipe_command(in, fd[1], commands[i]);
	
// 		}else{
// 			// Parent
// 			close(fd[1]);
// 			waitpid(-1, &status, 0);
// 			if(in != STD_INPUT){
// 				close(in);
// 			}
// 			in = fd[0];
// 		}
// 	}

// 	pid = fork();
// 	if(pid == -1){	
// 	  	printf("ERROR: %s\n", strerror(errno));
// 	}else if(pid == 0){
// 	  	//Child
// 		run_pipe_command(in, out, commands[numCommands-1]);
// 	}else{
// 	  	// Parent

// 	  	if(isBackground == false){
// 	  		//printf("33333\n");
// 		  	waitpid(-1, &status, 0);
// 		}else{
// 			//printf("44444\n");
// 			waitpid(-1, &status, WNOHANG);
// 		}

// 		if(in != STD_INPUT){
// 			close(in);
// 		}
// 	}

// }


int main(){

	char line[MAXLINE];
	char tokens[MAXLINE][MAXTOKEN]; 
	char commands[50][MAXLINE][MAXTOKEN];
	int numTokens = 0;
	int numCommands = 0;
	bool isBackground = false;
	
	while(!feof(stdin)){

		type_prompt(); 

		memset(line,0,strlen(line));
		get_line(line);
		if(feof(stdin)){
			exit(1);
		}
		//printf("Line : %s \n", line);			// for testing


		for(int i = 0; i < numTokens; i++){
			memset(tokens[i],0,strlen(tokens[i]));
		}
		numTokens = split_line(line, tokens);
		//printf("Tokens : %d \n", numTokens);				// for testing
		//test_print_tokens(numTokens, tokens);

		for(int i = 0; i < numCommands; i++){
			for(int j = 0; j < MAXLINE; j++){
				memset(commands[i][j],0,strlen(commands[i][j]));
			}
		}
		numCommands = split_tokens(numTokens, tokens, commands);			
		//printf("commands : %d \n", numCommands);					// for testing
		//test_print_commands(numCommands, commands);

		isBackground = false;
		if(strcmp(tokens[numTokens - 1], "&") == 0){
			isBackground = true;
		}
		//printf("isBackground : %d\n", isBackground);		// for testing

		
		if(numCommands == 0){
			// do nothing
		}
		else if(numCommands == 1){
			//printf("excuting single command\n");			// for testing
			execute_single_command(commands[0], isBackground);
		}else{
			//printf("launching pipe\n");
			//launch_pipes(numCommands, commands, isBackground);
		}
		
	}

	return 0;

}

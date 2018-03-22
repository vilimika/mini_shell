#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define BUFSIZE_COMMAND 512
#define BUFSIZE_ARGS    64

void interactive();
char *get_command();
char **parse_cmd(char*);
int execute_command(char**);

int main(int argc, char **argv)
{
	if(argc == 1) interactive();
	
}

//this fucntion controls the execution of interactive mode
void interactive()
{
	char *command;
	char **args;
	int control = 1;

	while(control)
	{
		printf("prompt> ");
		command = get_command();
		args = parse_cmd(command);
		control = execute_command(args);

		free(command);
		free(args);
	//test to see if the args array was made correctly
	/* 
		int z = 0;
		for(;z < 2;z++)
			printf("%s\n", args[z]);
	*/	
	//************************************************
		

	}
}

//this function reads in the command from user
char *get_command()
{
	int buf_size = BUFSIZE_COMMAND, i = 0, ch;
	char *buf = malloc(sizeof(char) * buf_size);


	//check if memory was allocated properly
	if(!buf)
	{
		fprintf(stderr, "get_command - malloc error\n");
		exit(EXIT_FAILURE);
	}

	//loop to read from stdin and make the command
	while(1)
	{
		ch = getchar();

		if(ch == '\n' || ch == EOF)
		{
			buf[i] = '\0';
			return buf;
		}
		else
			buf[i] = ch;
		i++;
	}
}

//this function parses the command from get_command to make argv
char **parse_cmd(char* command)
{
	int i = 0, buf_size = BUFSIZE_ARGS;
	char **args = malloc(sizeof(char*) * buf_size);
	char *token , *delim;
	delim = " \t\n\r";
	
	//check if malloc did the magic
	if(!args)
	{
		fprintf(stderr, "parse_cmd - malloc error\n");
		exit(EXIT_FAILURE);
	}


	//parse the string
	token = strtok(command, delim);
	while(token != NULL)
	{
		args[i] = token;
		token = strtok(NULL, delim);
		i++;
		if(token == NULL){break;}
	}
	
	
	args[i] = NULL;
//	printf("%s placeholder %i %s %s\n", args[0], i, args[1], args[2]);
	return args;
}


//this function uses execvp to execute commands 
int execute_command(char **args)
{
	//if no args provided go back to prompt
	if(!args[0] && !args[1])
		return 1;
	//check for exit or barrier
	if(!args[1])
	{
		if(strcmp(args[0],"exit") == 0)
			exit(EXIT_SUCCESS);
	}

	int status;
	pid_t wait_pid, pid = fork();

	//check if fork created child
	if(pid == -1)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	//child - run the specifiend command
	else if(pid == 0)
	{
		if(execvp(args[0], args) == -1)
		{
			perror("mini shell");
			exit(EXIT_FAILURE);
		}
	}
	//parent - wait for child to execute
	else
	{
		do
		{
			wait_pid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status)); 
	}
	
	return 1;
}








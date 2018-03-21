#include <stdio.h>
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
//	/* 
		int z = 0;
		for(;z < 3;z++)
			printf("%s\n", args[z]);
//	*/	
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
	delim = " \t";
	
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
	}

	i++;
	args[i] = NULL;
	return args;
}










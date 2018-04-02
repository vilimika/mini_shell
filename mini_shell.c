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
char **parse_cmd(char*, int*, char**, int*);
int execute_command(char**, int, int, char*);
void batch_mode(char*);

int main(int argc, char **argv)
{	
	if(argc > 2) {printf("Too many arguments provided"); exit(EXIT_FAILURE);}
	if(argc == 1) interactive();
	if(argc == 2) batch_mode(argv[1]);
	
	return 0;
}

//this fucntion controls the execution of interactive mode
void interactive()
{
	char *command;
	char **args;
	int control = 1, status = 0;

	while(control)
	{
		int bg = 0, *p_bg = &bg;
		int rd = 0, *p_rd = &rd;
		char *rd_path = NULL;
		printf("prompt> ");
		command = get_command();
		args = parse_cmd(command, p_bg, &rd_path, p_rd);
		if(args[0])
		{
			if(strcmp(args[0], "barrier") == 0)
			{
				while ((waitpid(-1, &status, 0)) != -1);
				continue;
			}
		}
//		printf("%s%i\n", rd_path, rd);
		control = execute_command(args, bg, rd, rd_path);
		
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
char **parse_cmd(char* command, int *bg, char **rd_path, int *rd)
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
		if(token != NULL)
		{
			if(strcmp(token, "&") == 0)
			{
				(*bg)++;
				token = strtok(NULL, delim);
			}
			
			if(token != NULL)
			{
				if(strcmp(token, ">") == 0)	
				{
					(*rd)++;
					token = strtok(NULL, delim);
					*rd_path = (char *) malloc(strlen(token) + 1);
					strcpy(*rd_path, token);
					token = strtok(NULL, delim);
				}
			}
		}
		i++;
	}
	
	
	args[i] = NULL;
//	printf("%s placeholder %i %s %s\n", args[0], i, args[1], args[2]);
	return args;
}


//this function uses execvp to execute commands 
int execute_command(char **args, int bg, int rd, char *rd_path)
{
	//if no args provided go back to prompt
	if(!args[0] && !args[1])
		return 1;
	//check for exit or barrier
	if(!args[1])
	{
		if(strcmp(args[0],"quit") == 0)
			exit(EXIT_SUCCESS);
	}

	int status = 0;
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
		
		if(rd == 1)
		{
			int fd;
			fd = open(rd_path, O_CREAT | O_TRUNC | O_WRONLY, 0766 );
			if(fd == -1) {perror("Output file error");}
			if(dup2(fd, 1) == -1) {perror("dup2 error duplicating file descriptor");}
			close(fd);
		}

	
		if(execvp(args[0], args) == -1)
		{
			perror("mini shell");
			exit(EXIT_FAILURE);
		}
	}
	//parent - wait for child to execute
	else
	{
		if(bg == 1)
		{	
			return 1;
		}
		
		do
		{
			wait_pid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status)); 
	}

	return 1;
}

//function reads a file and executes commands specified in the file
void batch_mode(char *file)
{
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	char *command, **args;
	int status = 0, control = 1;

	fp = fopen(file, "r");
	if(fp == NULL) {perror("batch file open error"); return;}

	while((read = getline(&line, &len, fp)) != -1)
	{
		int bg, *p_bg = &bg;
		int rd, *p_rd = &rd;
		char *rd_path = NULL;
		printf("Executing: %s", line);
		args = parse_cmd(line, p_bg, &rd_path, p_rd);
		printf("%s %s\n", args[0], args[1]);	
		if(args[0])
		{
			if(strcmp(args[0], "barrier") == 0)
			{
				while((waitpid(-1, &status, 0 )) != -1);
				continue;
			}
		}	
		printf("%s %s\n", args[0], args[1]);	
		control = execute_command(args, bg, rd, rd_path);

		free(args);
	}
}

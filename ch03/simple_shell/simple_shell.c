#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define MAX_LINE 80

#define FLAG_DEFAULT 0			// 0000
#define FLAG_AMPERSAND 1		// 0001
#define FLAG_LAST_COMMAND 2		// 0010

void init_args(char **args)
{
	int i;
	for(i = 0; i < MAX_LINE/2; i++)
	{
		args[i] = NULL;
	}
}

void get_input(char *cmd)
{
	fgets(cmd, MAX_LINE, stdin);
	if(cmd[strlen(cmd)-1] == '\n')
	{
		cmd[strlen(cmd)-1] = '\0';
	}
}

int parse_input(char *cmd, char **args, char *infile, char *outfile)
{
	char *token;
	int i = 0;
	int flag = FLAG_DEFAULT;
	int red = 0;
	
	token = strtok(cmd, " ");
	while(token != NULL)
	{
		if(red)
		{
			switch(red)
			{
			case '<':
				strcpy(infile, token);
				infile[strlen(token)] = '\0';
				break;
			case '>':
				strcpy(outfile, token);
				outfile[strlen(token)] = '\0';
				break;
			default:
				break;
			}
			red = 0;
		}
		else if(!strcmp(token, "&"))
		{
			flag = flag | FLAG_AMPERSAND;
		}
		else if(!strcmp(token, "<") || !strcmp(token, ">"))
		{
			red = token[0];
		}
		else
		{
			args[i] = (char*)malloc(sizeof(char) * (strlen(token) + 1));
			strcpy(args[i], token);
			args[i][strlen(token)] = '\0';
			//printf("%s\n", args[i]);
			i++;
		}
		
		token = strtok(NULL, " ");
	}
	args[i] = NULL;
	
	return flag;
}

void clear_args(char **args)
{
	int i = 0;
	while(args[i] != NULL)
	{
		free(args[i]);
		args[i] = NULL;
		i++;
	}
}

int main(void)
{
	char *args[MAX_LINE/2 + 1];
	int should_run = 1;
	int flag;
	int child_err = 0;
	char cmd[MAX_LINE];
	char cmd_buf[MAX_LINE+1];
	char infile[MAX_LINE];
	char outfile[MAX_LINE];
	int fd_in, fd_out;
	int dup_in, dup_out;
	
	pid_t pid;
	
	init_args(args);
	cmd_buf[0] = '\0';
	
	while(should_run)
	{
		printf("osh>");
		fflush(stdout);
		
		clear_args(args);
		infile[0] = '\0';
		outfile[0] = '\0';
		
		get_input(cmd);
		//printf("%s\n", cmd);
		
		// history feature
		if(!strcmp(cmd, "!!"))
		{
			if(strlen(cmd_buf) <= 0)
			{
				printf("No commands in history.\n");
				continue;
			}
			strcpy(cmd, cmd_buf);
		}
		strcpy(cmd_buf, cmd);
		
		// parse command
		flag = parse_input(cmd, args, infile, outfile);
		
		// check exit condition
		if(!strcmp(args[0], "exit"))
		{
			clear_args(args);
			break;
		}
		
		// (1) fork a child process using fork()
		pid = fork();
		if(pid < 0)
		{
			fprintf(stderr, "Fork Failed");
			clear_args(args);	// TODO: should we clear memory when fork fails???
			return 1;
		}
		// (2) the child process will invoke execvp()
		else if(pid == 0)
		{
			// redirection
			if(strlen(infile))
			{
				if((fd_in = open(infile, O_RDWR, 0644)) == -1)
				{
					printf("Open Error.\n");
					continue;
				}
				if((dup_in = dup2(fd_in, STDIN_FILENO)) == -1)
				{
					printf("Dup2 Error.\n");
					close(fd_in);
					exit(0);
					continue;
				}
			}
			if(strlen(outfile))
			{
				if((fd_out = open(outfile, O_RDWR|O_CREAT, 0644)) == -1)
				{
					printf("Open Error.\n");
					continue;
				}
				if((dup_out = dup2(fd_out, STDOUT_FILENO)) == -1)
				{
					printf("Dup2 Error.\n");
					close(fd_out);
					exit(0);
					continue;
				}
			}
			
			child_err = execvp(args[0], args);
			if(child_err < 0)
			{
				printf("No command '%s' found.\n", args[0]);
				exit(0);
			}
			should_run = 0;
			
			// close redirection
			if(strlen(infile))
			{
				close(dup_in);
				close(fd_in);
			}
			if(strlen(outfile))
			{
				close(dup_out);
				close(fd_out);
			}
		}
		// (3) parent will invoke wait() unless command include &
		else
		{
			// FIXME: when including '&', the input and output shifts
			if(!(flag & FLAG_AMPERSAND) || strlen(outfile))
			{
				wait(NULL);
			}
			
		}
	}
	
	return 0;
}

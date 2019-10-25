#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define MAX_LINE 80

#define FLAG_DEFAULT 0			// 00000
#define FLAG_ERROR 1			// 00001
#define FLAG_AMPERSAND 2		// 00010
#define FLAG_LAST_COMMAND 4		// 00100
#define FLAG_PIPE 8				// 01000

#define READ_END 0
#define WRITE_END 1

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
//	printf("%d\n", strlen(cmd));
	if(strlen(cmd) > 1 && cmd[strlen(cmd)-1] == '\n')
	{
		cmd[strlen(cmd)-1] = '\0';
	}
}

int parse_input(char *cmd, char **args, char *infile, char *outfile, int *pipe_sep)
{
	char *token;
	int i = 0;
	int flag = FLAG_DEFAULT;
	int red = 0;
	int pipe_cnt = 0;
	
	token = strtok(cmd, " ");
	while(token != NULL)
	{
		if(strlen(token) <= 0)
		{
			token = strtok(NULL, " ");
			continue;
		}
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
			if(!strcmp(token, "<") && pipe_cnt != 0)
			{
				printf("Input redirection can only be used in the FIRST command in the sequence.\n");
				flag = flag | FLAG_ERROR;
			}
		}
		else if(!strcmp(token, "|"))
		{
			if(strlen(outfile))
			{
				printf("Output redirection can only be used in the LAST command in the sequence.\n");
				flag = flag | FLAG_ERROR;
			}
		
			flag = flag | FLAG_PIPE;
			args[i] = NULL;
			pipe_sep[pipe_cnt] = ++i - (pipe_cnt > 0 ? pipe_sep[pipe_cnt-1] : 0);
			//printf("%d\n", pipe_sep[pipe_cnt]);
			pipe_cnt++;
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
	pipe_sep[pipe_cnt] = -1;
	
	if(red)
	{
		printf("Empty file descriptor.\n");
		flag = flag | FLAG_ERROR;
	}
	if(i == 0 || args[0][0] == '\n')
	{
		flag = flag | FLAG_ERROR;
	}
	
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

// FIXME: need to press 'q' frequently to exit less, extra q's will be taken as inputs
int pipe_exe(char **args, int *pipe_sep, char *infile, char *outfile)
{
	int fd[2];
	pid_t pid;
	int child_err;
	int fd_red;
	int tmp_fd;
	tmp_fd = dup(STDOUT_FILENO);
	
	if(pipe(fd) == -1)
	{
		fprintf(stderr, "Pipe Failed");
		return -1;
	}
	
	pid = fork();
	if(pid < 0)
	{
		fprintf(stderr, "Fork Failed");
		return -1;
	}
	if(pid > 0)
	{
		printf("parent process\n");
		printf("%d\n", pid);
		//printf("%s\n", infile);
		close(fd[READ_END]);
		dup2(fd[WRITE_END], STDOUT_FILENO);
		close(fd[WRITE_END]);

		if(strlen(infile))
		{
			if((fd_red = open(infile, O_RDWR, 0644)) == -1)
			{
				printf("Open Error.\n");
				exit(0);
			}
			dup2(fd_red, STDIN_FILENO);
		}
		
		child_err = execvp(args[0], args);
		//close(fd[WRITE_END]);
		//dup2(STDOUT_FILENO, tmp_fd);
//		close(tmp)
		if(strlen(infile))
		{
			close(fd_red);
		}
		if(child_err < 0)
		{
			printf("No command '%s' found.\n", args[0]);
			exit(0);
		}
		printf("wait\n");
		waitpid(pid, NULL, 0);
		printf("finish wait\n");
	}
	else
	{
		close(fd[WRITE_END]);
		dup2(fd[READ_END], STDIN_FILENO);
		close(fd[READ_END]);
		
		args = args + pipe_sep[0];
		if(pipe_sep[1] == -1)
		{
			if(strlen(outfile))
			{
				printf("redirect");
				if((fd_red = open(outfile, O_RDWR|O_CREAT, 0644)) == -1)
				{
					printf("Open Error.\n");
					exit(0);
				}
				dup2(fd_red, STDOUT_FILENO);
			}
		
			child_err = execvp(args[0], args);
			if(strlen(outfile));
			{
				close(fd_red);
			}
			if(child_err < 0)
			{
				printf("No command '%s' found.\n", args[0]);
				exit(0);
			}
		}
		else
		{
			pipe_exe(args, pipe_sep+1, "", outfile);
		}
		//close(fd[READ_END]);
	}
	exit(0);
}

int pipe_exe_chain(char **args, int *pipe_sep, char *infile, char *outfile)
{
	size_t children[MAX_LINE];
	int fd[MAX_LINE][2];
	int child_idx = 0;
	int err_flag = 0;
	int fd_red_in = -1;
	int fd_red_out = -1;
	int i;

	if(strlen(infile) &&
		(fd_red_in = open(infile, O_RDWR, 0644)) == -1)
	{
		printf("Open Error.\n");
		exit(0);
	}
	if(strlen(outfile) &&
		(fd_red_out = open(outfile, O_RDWR|O_CREAT, 0644)) == -1)
	{
		printf("Open Error.\n");
		exit(0);
	}

	while(true)
	{
		// only establish pipe to next child if there exists a next command
		if(pipe_sep[0] != -1 && pipe(fd[child_idx]) == -1){
			fprintf(stderr, "Pipe Failed");
			err_flag = 1;
			break;
		}
		
		// fork child process
		children[child_idx] = fork();
		if(children[child_idx] < 0)
		{
			fprintf(stderr, "Fork Failed");
			err_flag = 1;
			break;
		}
		
		if(children[child_idx] == 0)
		{
			// the first child only checks input file redirection
			if(child_idx == 0)
			{
				if(fd_red_in != -1)
				{
					dup2(fd_red_in, STDIN_FILENO);
				}
			}
			// other children establish previous pipe redirection
			else
			{
				close(fd[child_idx-1][WRITE_END]);
				dup2(fd[child_idx-1][READ_END], STDIN_FILENO);
				close(fd[child_idx-1][READ_END]);
			}
			// the last child only checks output file redirection
			if(pipe_sep[0] == -1)
			{
				if(fd_red_out != -1)
				{
					dup2(fd_red_out, STDOUT_FILENO);
				}
			}
			// other children establish next pipe redirection
			// secure: pipe only establish when it is not the last child
			else
			{
				close(fd[READ_END]);
				dup2(fd[WRITE_END], STDOUT_FILENO);
				close(fd[WRITE_END]);
			}
			// execute command
			child_err = execvp(args[0], args);
			if(child_err < 0)
			{
				printf("No command '%s' found.\n", args[0]);
				exit(0);
			}
		}
		
		// update child index and command information
		child_idx++;
		if(pipe_sep[0] == -1)
		{
			break;
		}
		args += pipe_sep[0];
		pipe_sep++;
	}

	// wait for termination of all children once
	for(i=0; i<child_idx; i++)
	{
		waitpid(children[i], NULL, 0);
	}
	exit(err_flag);
}

int main(void)
{
	char *args[MAX_LINE/2 + 1];
	int should_run = 1;
	int flag, last_flag = 0;
	int child_err = 0;
	char cmd[MAX_LINE];
	char cmd_buf[MAX_LINE+1];
	char infile[MAX_LINE];
	char outfile[MAX_LINE];
	int fd_in, fd_out;
	int dup_in, dup_out;
	int pipe_sep[MAX_LINE];
	
	pid_t pid;
	
	init_args(args);
	cmd_buf[0] = '\0';
	
	while(should_run)
	{
		printf("osh>");
		fflush(stdout);
		//fflush(stdin);
		
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
		flag = parse_input(cmd, args, infile, outfile, pipe_sep);
		if(flag & FLAG_ERROR)
		{
			continue;
		}
		
		// if(last_flag & FLAG_AMPERSAND)
		// {
		// 	wait(NULL);
		// }
		// last_flag = flag;
		
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
			if(flag & FLAG_PIPE)
			{
				pipe_exe(args, pipe_sep, infile, outfile);
				pipe_exe_chain(args, pipe_sep, infile, outfile);
			}
			else
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

				// FIXME: although concurrent running is available,
				// still need to recheck the correctness of file operaiton
				if(!(flag & FLAG_AMPERSAND) || strlen(outfile) || 0 == fork())
				{
					child_err = execvp(args[0], args);
					if(child_err < 0)
					{
						printf("No command '%s' found.\n", args[0]);
						exit(0);
					}
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
		}
		// (3) parent will invoke wait() unless command include &
		else
		{
			waitpid(pid, NULL, 0);
		}
	}
	
	return 0;
}


#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

typedef struct s_cmd
{
	char	**cmd;
	int		*status;
	int		start;
	int		end;
	int		pipenum;
	int		*pipe;
}					t_cmd;

#define COM 1
#define ERR 2
#define PIPE 3
#define SEMQ 4

void	cd(t_cmd mini)
{
	
}

int	*creat_pipe(t_cmd mini)
{
	int	*pip;
	int i = 0;

	pip = (int *)malloc(sizeof(int) * (mini.pipenum + 1) * 2);
	while (i < mini.pipenum) {
		pipe(pip + i * 2);
	}
	return (pip);
}

void	exec(t_cmd mini, char **envp)
{
	int	*pipe;
	pid_t pid;

	if (mini.pipenum == 0 && !strncmp(mini.cmd, "cd"))
		cd(mini);
	else {
		pipe = creat_pipe(mini);
		for (int i = 0; i <= mini.pipenum; i++) {
			pid = fork();
			if (pid == 0) {
				if (i == 0)	{
					if (mini.pipenum != 0)
						dup2(pipe[1], 1);
				} 
				else if (i == mini.pipenum)
					dup2(pipe[2 * i - 1], 0);
				else
				{
					dup2(pipe[2 * i - 2], 0);
					dup2(pipe[2 * i + 1], 1);
				}
				//!execveだけどどこまでかはまだ
			} else {
				if (i != 0)
					close (pipe[2 * 1 - 2]);
			}
		}
	}
	free(pipe);
}

int	main(int argc, char **argv, char **envp){
	t_cmd	mini;
	int		i = 0;

	if (argc > 1) {
		mini.cmd = argv;
		mini.status = (int *)malloc(sizeof(int) * argc + 1);
		mini.status[0] = 0;
		mini.pipenum = 0;
		mini.start = 1;
		while (argv[++i]) {
			if (!strcmp(mini.cmd[i], "|"))
			{	
				mini.status = PIPE;
				mini.pipenum ++;
			}
			else if (!strcmp(mini.cmd[i], ";"))
			{
				mini.end = i;
				mini.status = SEMQ;
				exec(mini, envp);
				mini.start = i + 1;
			}
			else
				mini.status = COM;
		}
		mini.end = i;
		exec(mini, envp);
		free(mini.status);
	}
	return ;
}
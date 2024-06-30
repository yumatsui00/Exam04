#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>

//iこのコマンドを実行

int	err(char *str) {
	while (*str)
		write(2, str++, 1);
	return 1;
}

int	cd(char **argv, int i) {
	if (i != 2)
		return err("error: cd: bad arguments\n");
	if (chdir(argv[1]) == -1)
		return err("error: cd: cannot change directory to "), err(argv[1]), err("\n");
	return 0;
}

int	exec(char **argv, int i, char **envp) {
	int	fd[2];
	int	status;
	int	ispipe = argv[i] && !strcmp(argv[i], "|");

	if (!ispipe && !strcmp(*argv, "cd"))
		return cd(argv, i);
	if (ispipe && pipe(fd) == -1)
		return err("error: fatal\n");

	pid_t pid = fork();
	if (pid == 0) {
		if (ispipe && (dup2(fd[1], 1) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
			return err("error: fatal\n");
		if (!strcmp(*argv, "cd"))
			return cd(argv, i);
		argv[i] = NULL;
		execve(*argv, argv, envp);
		return err("error: cannot execute "), err(*argv), err("\n");
	}
	waitpid(pid, &status, 0);
	if (ispipe && (dup2(fd[0], 0) == -1 || close(fd[0]) == -1 || close(fd[1]) == -1))
		return err("error: fatal\n") && WEXITSTATUS(status);

}

int main(int argc, char **argv, char **envp) {
	int	i = 0;
	int	status = 0;

	if (argc > 1) {
		while (argv[i] && argv[++i]) {
			argv += i; //次のコマンドまで移動
			i = 0; //実行するコマンドの数を数える
			while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
				i++;
			if (i)
				status = exec(argv, i, envp); //これが終了ステータスになる
		}
	}
	return status;
}
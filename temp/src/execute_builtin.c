#include "minishell.h"



void ft_env_val_vol2(t_gen *gen, char *trimmed_env, int fd)
{
	int i;
	int j;
	int len_env;
	char *env;
	char *gg;

	i = 0;
	while(gen->envs[i])
	{
		if(ft_strchr(gen->envs[i], '='))
		{
			j = 0;
			len_env = ft_strlen_env(gen->envs[i], '=');
			env = malloc(sizeof(char) * (len_env + 1));
			while(gen->envs[i][j] != '=')
			{
				env[j] = gen->envs[i][j];
				j++;
			}
			env[j] = '\0';
			if(!ft_strcmp(trimmed_env, env))
			{
				ft_putstr_fd(ft_strrchr(gen->envs[i], '=') + 1, fd);
				free(env);
				return ;
			}
			free(env);
		}
		i++;
	}
}

void ft_env_val(t_gen *gen, char *str, int fd)
{
	char *trimmed_env;
	int len_env;
	int i;

	trimmed_env = malloc(sizeof(char) * ft_strlen(str));
	i = 0;
	while(str[i + 1])
	{
		trimmed_env[i] = str[i+1];
		i++;
	}
	trimmed_env[i] = '\0';
	if(!ft_strcmp(trimmed_env, "?"))
	{
		ft_putnbr_fd(gen->exit_status, fd);
	}
	ft_env_val_vol2(gen, trimmed_env, fd);
	free(trimmed_env);
}

void	builtin_echo(t_arg *args, t_redirs *redirs, t_gen *gen)
{
	int		i;
	int		j;
	int		fd;
	bool	n;
	
	fd = STDOUT_FILENO;
	i = 1; // bo 0 to echo
	n = false;
	while (args[i].arg && args[i].arg[0] == '-')
	{
		j = 1;
		while (args[i].arg[j] == 'n') // -n -nn -nnnnn może być więcej
			j++;
		if (args[i].arg[j] == '\0') // nie może być -nnnk
		{
			n = true;
			i++;
		}
		else
			break ;
	}
	if (redirs->is_append || redirs->output_redir != -1)
		fd = redirs->output_redir;
	while (args[i].arg)
	{
		if (args[i].arg[0] == '$' && args[i].in_quotes == false)
		{
			ft_env_val(gen, args[i].arg, fd); //not sure jak chcesz do tego podejść to zostawie tak narazie
		}
		else
			ft_putstr_fd(args[i].arg, fd);
		if (args[i + 1].arg)
			ft_putstr_fd(" ", fd);
		i++;
	}
	if (!n || redirs->is_append)
		ft_putstr_fd("\n", fd);
}

void	builtin_cd(t_arg *args)
{
	char	*home_dir;

	if (!args[1].arg)
	{
		home_dir = getenv("HOME");
		if (home_dir)
		{
			if (chdir(home_dir) != 0)
				perror("cd failed");
		}
		else
			printf("cd: HOME not set\n");
	}
	else
	{
		if (chdir(args[1].arg) != 0)
			perror("cd failed");
	}
}

void	builtin_pwd(void)
{
	char	cwd[4096]; //max path sprawdzone _PC_PATH_MAX - przynajmniej na kompie w 42

	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		printf("%s\n", cwd);
	}
	else
	{
		perror("getcwd failed");
	}
}

void	builtin_exit(t_arg *args)
{
	int	i;

	printf("exit\n");
	i = 0;
	while (args[i].arg)
		i++;
	if (i == 2)
	{
		if (digits_only(args[1].arg))
			exit(ft_atoi(args[1].arg));
		else
		{
			printf("bash: exit: %s: numeric argument required\n", args[1]);
			exit (255);
		}
	}
	else if (i > 2)
	{
		printf("bash: exit: too many arguments\n");
		return ;
	}
	else
		exit(0);
}

void	execute_builtin(t_command *command, t_gen *gen)
{
	if (ft_strcmp(command->args[0].arg, "echo") == 0)
		builtin_echo(command->args, &command->redirs, gen);
	else if (ft_strcmp(command->args[0].arg, "cd") == 0)
		builtin_cd(command->args);
	else if (ft_strcmp(command->args[0].arg, "pwd") == 0)
		builtin_pwd();
	else if (ft_strcmp(command->args[0].arg, "exit") == 0)
		builtin_exit(command->args);
	else if (ft_strcmp(command->args[0].arg, "unset") == 0)
		ft_unset_env(gen, command->args[1].arg);
	else if (ft_strcmp(command->args[0].arg, "history") == 0)
		ft_display_history_list(gen);
	else if (ft_strcmp(command->args[0].arg, "export") == 0)
		ft_export_env(gen, command->args[1].arg);
}

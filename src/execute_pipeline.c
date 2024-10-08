/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 14:02:12 by pruszkie          #+#    #+#             */
/*   Updated: 2024/09/07 12:37:39 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <sys/stat.h>
#include <sys/types.h>

void	ft_read_fd(t_command *command, t_gen *gen, int i)
{
	int	j;

	j = 0;
	if ((i == 0) && (command->redirs.input_redir > 0))
		if (dup2(command->redirs.input_redir, 0) == -1)
			perror("Error!");
	if (i > 0)
		if (dup2(gen->pipes[i - 1][0], 0) == -1)
			perror("Error!");
	while (j < gen->num_of_cmds - 1)
	{
		if (j != (i - 1))
			close(gen->pipes[j][0]);
		j++;
	}
}

void	ft_write_fd(t_command *command, t_gen *gen, int i)
{
	int	j;

	if (i < gen->num_of_cmds - 1)
		if (dup2(gen->pipes[i][1], 1) == -1)
			perror("Error!");
	if ((i == gen->num_of_cmds - 1) && (command->redirs.output_redir > 0))
		if (dup2(command->redirs.output_redir, 1) == -1)
			perror("Error!");
	j = 0;
	while (j < gen->num_of_cmds - 1)
	{
		if (j != i)
			close(gen->pipes[j][1]);
		j++;
	}
}

void	ft_child_process(t_command *command, t_gen *gen, int i)
{
	char	**argv;

	argv = convert_args(command->args);
	ft_read_fd(command, gen, i);
	ft_write_fd(command, gen, i);
	if (execve(command->path, argv, gen->envs) == -1)
	{
		write(2, command->args[0].arg, ft_strlen(command->args[0].arg));
		write(2, ": command not found\n", 20);
		exit(127);
	}
	free(argv);
}

int	execute_pipeline(t_command *command, t_gen *gen)
{
	int	i;
	int	status;

	gen->pipes = malloc((gen->num_of_cmds - 1) * sizeof(int *));
	init_pipes(gen);
	gen->pids = malloc((gen->num_of_cmds + 1) * sizeof(int));
	create_child_processes(command, gen);
	close_pipes(gen);
	if (command->redirs.is_heredoc)
		unlink("heredoc.txt");
	i = 0;
	while (i < gen->num_of_cmds)
	{
		wait(&status);
		gen->exit_status = WEXITSTATUS(status);
		i++;
	}
	gen->is_blocking = 0;
	return (0);
}

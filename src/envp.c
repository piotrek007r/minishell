/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 14:02:12 by lkoc              #+#    #+#             */
/*   Updated: 2024/09/08 01:44:12 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	ft_env_cmp(t_gen *gen, char *env)
{
	bool	match;
	int		i;

	match = false;
	i = 0;
	while (gen->envs[i])
	{
		if (ft_strncmp(gen->envs[i], env, ft_strlen_env(gen->envs[i],
					'=')) == 0)
			match = true;
		i++;
	}
	return (match);
}

char	**ft_unset_env_vol2(t_gen *gen, char *env, int env_len)
{
	char	**temp;
	int		i;
	int		j;

	temp = malloc(sizeof(char *) * (env_len + 1));
	if (!temp)
		return (NULL);
	i = 0;
	j = 0;
	while (gen->envs[i])
	{
		if (ft_strncmp(gen->envs[i], env, ft_strlen_env(gen->envs[i],
					'=')) != 0)
		{
			temp[j] = ft_strdup(gen->envs[i]);
			free(gen->envs[i]);
			j++;
		}
		else
			free(gen->envs[i]);
		i++;
	}
	temp[j] = NULL;
	return (temp);
}

void	ft_unset_env(t_gen *gen, char *env)
{
	char	**temp;
	int		env_len;

	env_len = 0;
	while (gen->envs[env_len] != NULL)
		env_len++;
	if (!env)
		return ;
	if (!ft_env_cmp(gen, env))
		return ;
	temp = ft_unset_env_vol2(gen, env, env_len);
	free(gen->envs);
	gen->envs = malloc(sizeof(char *) * (env_len + 1));
	ft_copy_arr(gen->envs, temp, env_len - 1);
	gen->envs[env_len - 1] = NULL;
	ft_free_arr(temp);
}

void	ft_export_env_vol2(t_gen *gen, char *env, int env_len)
{
	char	**temp;

	temp = malloc(sizeof(char *) * (env_len + 2));
	ft_copy_arr(temp, gen->envs, env_len);
	temp[env_len] = ft_strdup(env);
	temp[env_len + 1] = NULL;
	ft_free_arr(gen->envs);
	gen->envs = malloc(sizeof(char *) * (env_len + 2));
	ft_copy_arr(gen->envs, temp, env_len + 1);
	gen->envs[env_len + 1] = NULL;
	ft_free_arr(temp);
}

void	ft_export_env(t_gen *gen, char *env)
{
	int		env_len;
	int		i;
	char	*clean_env;

	i = 0;
	if (!env)
	{
		while (gen->envs[i])
			printf("declare -x %s\n", gen->envs[i++]);
		return ;
	}
	clean_env = ft_remove_outer_quotes(env);
	env_len = 0;
	while (gen->envs[env_len] != NULL)
		env_len++;
	if (!ft_check_format_export(clean_env))
		return ;
	if (ft_env_cmp(gen, clean_env))
	{
		ft_unset_env(gen, clean_env);
		env_len--;
	}
	ft_export_env_vol2(gen, clean_env, env_len);
	free(clean_env);
}

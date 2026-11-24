# minishell

Our own small bash, written in C. 42 Warsaw project, done in pair with Łukasz Koc ([kocyk908](https://github.com/kocyk908)).

## What it can do

- runs programs from PATH or by full path (`ls`, `/bin/ls`)
- pipes: `ls | grep a | wc -l`
- redirections `<`, `>`, `>>` and heredoc `<<`
- env variables like `$HOME`, `$?` and quotes `'` / `"`
- builtins: `echo`, `cd`, `pwd`, `export`, `unset`, `exit`, `history`
- ctrl-C, ctrl-D and ctrl-\ work like in bash

## My part

I did the execution part, so everything that happens after the line is parsed:
forking a child for every command, connecting them with `pipe()` and `dup2()`,
finding the program in PATH, `execve()` and getting the exit code back for `$?`.
I also did `$` expansion, `export` / `unset`, redirections, heredoc and history.

Hardest thing was closing all the unused pipe ends. If one stays open, the next
command waits forever for input that never comes.

## Build and run

You need readline (on Ubuntu: `sudo apt install libreadline-dev`).

```
make
./minishell
```

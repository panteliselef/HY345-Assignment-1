#include<stdlib.h>
#include<stdio.h>
#include<string.h>

char **tokenize_pipe(char * str);
char **tokenize_line(char *str);
void print_tokens(char **tokens);
void print_pipes(char **tokens);
int has_no_command(char *command);
int has_redirections(char *pipe);
int has_read_rdr(char *pipe);
int has_write_rdr(char *pipe);
int has_append_rdr(char *pipe);
void print_env_var(char *env_var);

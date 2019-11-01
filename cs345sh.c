/* 
 * Pantelis Elef
 * csd3942
 * C Shell Implementation  
 */ 

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include "my_utils.h"

#define MAX_CHARS_USERNAME 40
#define MAX_CHARS_PROMPT 400

void printPrompt()
{
  char *cwd;
  char *loginUser;
  char *promptMsg;
  promptMsg = (char *)malloc(MAX_CHARS_PROMPT * sizeof(char));
  loginUser = (char *)malloc(MAX_CHARS_USERNAME * sizeof(char));
  loginUser = getlogin();
  if(loginUser == NULL) {
    printf("Couldn't get login user\n");
    exit(EXIT_FAILURE);
  }
  cwd = getcwd(NULL,0);
  strcat(promptMsg, loginUser);
  strcat(promptMsg, "@cs345sh");
  strcat(promptMsg,cwd);
  strcat(promptMsg, "/$ ");
  printf("\n%s", promptMsg);
}


char *read_command(void){
  size_t bufsize = 0; /* make getline decide the bufsize */
  char *line = NULL;
  getline(&line, &bufsize, stdin);
  return line;
}



void generate_child (int in, int out, char *pipe)
{
  char **token = tokenize_line(pipe);
  pid_t pid;
  pid = fork();
  if(pid < 0){
    perror("fork");
    exit(EXIT_FAILURE);
  }
  if (pid == 0){ /* child code */
    if (in != 0) { /* if in is not stdin */
      dup2 (in, 0);
      close (in);
    }

    if (out != 1){ /* if out is not stdout */
      dup2 (out, 1);
      close (out);
    }

    execvp(token[0],token);
    exit(EXIT_FAILURE);
  }else{ /* parent code */
    wait(NULL);
  }
}


char * get_input_file_offset(char *pipe){
  char **token = tokenize_line(pipe);
  int i = 0;
  while(token[i] != NULL){
    if(strcmp(token[i],"<") == 0){
      return token[i+1];
    }
    i++;
  }
  return NULL;
}
char * get_ouput_file_offset(char *pipe){
  char **token = tokenize_line(pipe);
  int i = 0;
  while(token[i] != NULL){
    if(strcmp(token[i],">") == 0 || strcmp(token[i],">>") == 0){
      return token[i+1];
    }
    i++;
  }
  return NULL;
}
char * exclude_read_redirection(char *pipe){
  char **token = tokenize_line(pipe);
  char *new_command = (char *)malloc(40);
  int i = 0;
  while(token[i] != NULL){
    if(strcmp(token[i],"<") == 0){
      if(token[i+2] != NULL){
        i+=2;
      }else{
      return new_command;
      }
    }
    strcat(new_command," ");
    strcat(new_command,token[i]);
    i++;
  }
  return new_command;
}
char * exclude_write_redirection(char *pipe){
  char **token = tokenize_line(pipe);
  char *new_command = (char *)malloc(40);
  int i = 0;
  while(token[i] != NULL){
    if(strcmp(token[i],">") == 0){
      return new_command;
    }
    strcat(new_command," ");
    strcat(new_command,token[i]);
    i++;
  }
  return new_command;

}
char * exclude_append_redirection(char *pipe){
  char **token = tokenize_line(pipe);
  char *new_command = (char *)malloc(40);
  int i = 0;
  while(token[i] != NULL){
    if(strcmp(token[i],">>") == 0){
      return new_command;
    }
    strcat(new_command," ");
    strcat(new_command,token[i]);
    i++;
  }
  return new_command;
}
int exec_commands(int size,char **pipes){
  char **tokens = tokenize_line(pipes[0]);
  if(strcmp(tokens[0],"cd") == 0){
    chdir(tokens[1]);
    return 1;
  }else if(strcmp(tokens[0],"exit") == 0) {
    return 0;
  }else if(strcmp(tokens[0],"setenv") == 0){
    setenv(tokens[1],tokens[2],1);
    return 1;
  }else if(strcmp(tokens[0],"unsetenv") == 0){
    unsetenv(tokens[1]);
    return 1;
  }else if(strcmp(tokens[0],"env") == 0){
    print_env_var("HOME");
    print_env_var("PATH");
    return 1;
  }
  else{
    int i;
    int in, fd[2],out;
    char * s_command;
    char * input_file;
    char * output_file;

    in = 0; /* read from STDIN at first */

    for(i = 0; i < size; i++){

      pipe(fd);
      out = fd[1];

      s_command = pipes[i];

      if(has_redirections(pipes[i])){

        if(has_read_rdr(pipes[i])){
          input_file =  get_input_file_offset(pipes[i]);
          s_command = exclude_read_redirection(pipes[i]);
          in = open(input_file,O_RDONLY);
        }

        /* ATTENTION the value of s_command below may have been altered from exclude_read_redirection */
        if(has_write_rdr(pipes[i])){
          output_file =  get_ouput_file_offset(s_command);
          s_command = exclude_write_redirection(s_command);
          out = open(output_file, O_CREAT | O_WRONLY | O_TRUNC,0666);
        }
        else if(has_append_rdr(pipes[i])){
          output_file =  get_ouput_file_offset(s_command);
          s_command = exclude_append_redirection(s_command);
          out = open(output_file, O_CREAT | O_APPEND | O_WRONLY,0666);
        }
      }

      if(i == size-1){
        if(!has_write_rdr(pipes[i]) && !has_append_rdr(pipes[i])){
          out = 1;
        }
        generate_child(in, out, s_command);
        close(fd[0]);
        close(fd[1]);

      }else{
        
        generate_child(in, out, s_command);
        close (fd [1]);

        
        in = fd [0]; /* save read-end current pipe */
      }

    }    
    return 1;
  }
}


int main(int argc, char **argv){
  int status = 1;
  char *command;
  do{
    printPrompt();
    command = read_command();
    if(!has_no_command(command)) {
      char **pipes = tokenize_pipe(command);
      int i = 0;
      while(pipes[i])i++;
      status = exec_commands(i,pipes);
    }
    free(command);
  }while(status);

  return 0;
}

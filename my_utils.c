#include "my_utils.h"
#define COMMAND_DELIM " \t\r\n\a"
#define PIPE_DELIM "|"

char **tokenize_pipe(char * str){
  
  char **pipe_table = (char **) malloc(40 * sizeof(char *)); 

  int i = 0;
  char *token;
  token = strtok(str,PIPE_DELIM);

  while(token!=NULL){/* walk through tokens */
    pipe_table[i] = token;
    i++;
    /* do the reallocation here */
    token = strtok(NULL,PIPE_DELIM);
  }

  pipe_table[i] = NULL; /*in order for developer to know when table ends*/
  return pipe_table;
}

char **tokenize_line(char * str){
  char **token_table = (char **) malloc(40 * sizeof(char *)); /* create a 2D table with length of 40 which may contain strings*/

  int i = 0;
  char *token;
  char *newStr = (char *)malloc(40);
  newStr = strcpy(newStr,str);
  token = strtok(newStr,COMMAND_DELIM);
  while(token!=NULL){/* walk through tokens */
    token_table[i] = token;
    i++;
    /* do the reallocation here */
    token = strtok(NULL,COMMAND_DELIM);
  }

  token_table[i] = NULL; /*in order for developer to know when table ends*/
  return token_table;
}


void print_tokens(char **tokens){
  int i = 0;
  while (tokens[i] != NULL){
    printf("--token:%s\n",tokens[i]);
    i++;
  }
}

void print_pipes(char **tokens){
  int i = 0;
  while (tokens[i] != NULL){
    printf("--pipe:%s\n",tokens[i]);
    i++;
  }
}

int has_no_command(char *command){

  int i;
  for(i = 0; i < strlen(command); i++ ){
    if(command[i] != ' ' && command[i] !='\n')return 0;
  }

  return 1;

}

int has_redirections(char *pipe){
  if(strstr(pipe,"<") != NULL || strstr(pipe,">") != NULL || strstr(pipe,">>") != NULL){
    return 1;
  }
  return 0;
}


int has_read_rdr(char * pipe){

  char **token = tokenize_line(pipe);
  int i = 0;
  while(token[i] != NULL){
    if(strcmp(token[i],"<") == 0){
      return 1;
    }
    i++;
  }

  return 0;

}

int has_write_rdr(char * pipe){

  char **token = tokenize_line(pipe);
  int i = 0;
  while(token[i] != NULL){
    if(strcmp(token[i],">") == 0){
      return 1;
    }
    i++;
  }

  return 0;

}

int has_append_rdr(char * pipe){

  char **token = tokenize_line(pipe);
  int i = 0;
  while(token[i] != NULL){
    if(strcmp(token[i],">>") == 0){
      return 1;
    }
    i++;
  }

  return 0;

}

void print_env_var(char * env_var){
  if(getenv(env_var) == NULL){
    printf("%s=\n",env_var);  
  }else{
    printf("%s=%s\n",env_var,getenv(env_var));
  }
}
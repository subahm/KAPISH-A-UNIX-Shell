/* A minimal killer application interactive UNIX shell
   Author: Subah Mehrotra
   Reference: https://brennan.io/2015/01/16/write-a-shell-in-c/
*/

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LSH_RL_BUFSIZE 513
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

void lsh_loop(void){
  char *line;
  char **args;
  int status;
  do {
    printf("? ");
    line = read_line();
    args = split_line(line);
    status = execute(args);
    free(line);
    free(args):
  } while (status);
}

char *read_line(){
  int size = LSH_RL_BUFSIZE;
  char *line = malloc(sizeof(char)*size);
  int pos = 0;
  int c;

  if(!line){
    fprintf(stderr, "Error in reading line\n");
    exit(EXIT_FAILURE);
  }

  while(1){
    // Read a character
    c = getchar();
    if(c == EOF){
      exit(EXIT_SUCCESS);
    }
    else if(c == '\n'){
      line[pos] = '\0';
      return line;
    }
    else{
      line[pos] = c;
    }
    pos++;
    if(line >= 512){
      fprintf(stderr, "Line characters limit exceeded.\n");
      return NULL;
    }
  }
  return line;
}

char **split_line(char *line){
  int size = LSH_RL_BUFSIZE;
  int length = strlen(line);
  int pos = 0;
  char **tokens_backup;
  char **tokens = malloc(sizeof(char *) *length);
  if(!tokens){
    fprintf(stderr, "Problem in tokenizing\n");
    exit(EXIT_FAILURE);
  }
  *tokens = strtok(line, LSH_TOK_DELIM);
  while(tokens[pos] != NULL){
    token[pos+1] = strtok(NULL, LSH_TOK_DELIM);
    pos++;

    if(pos >= bufsize){
      bufsize += LSH_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize *sizeof(char*));
      if(!tokens){
        free(tokens_backup);
        fprintf(stderr, "allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
  return tokens;
}

int execute(char **args){
  if(args[0] == NULL){
    return 1;
  }
  else if(strcmp(args[0],"cd") == 0)
    return cd(args);
  else if(strcmp(args[0],"exit") == 0)
    return exit(args);
  else if(strcmp(args[0],"setenv") == 0)
    return setenv(args);
  else if(strcmp(args[0],"unsetenv") == 0)
    return unsetenv(args);
  else
    return lsh_launch(args);
}

int lsh_launch(char **args){
  pid = fork();
  if(pid == 0){
    //if(execvp(args[0], args) == -1))
      perror("Error");
  }
  else if(pid < 0)
    perror("Error");
  else
    waitpid(pid, NULL, 0);
  return 1;
}

int main(int argc, char **argv){
  // Load config files
  // Run command loop
  lsh_loop();
  // Perform any cleanup/shutdown
}

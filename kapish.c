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
}

int main(int argc, char **argv){
  // Load config files
  // Run command loop
  lsh_loop();
  // Perform any cleanup/shutdown
}

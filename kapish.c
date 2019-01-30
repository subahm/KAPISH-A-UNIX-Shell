/* A minimal killer application interactive UNIX shell
   Author: Subah Mehrotra
   Reference: https://brennan.io/2015/01/16/write-a-shell-in-c/
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define LSH_RL_BUFSIZE 513
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
int pid;

/*void sighand(int signum){
	int ret = kill(pid, SIGINT);
	printf("\n");

	if(ret == -1 ){
		perror("ERROR");
		return;
	}
}*/

int lsh_launch(char **args){
  pid = fork();
  if(pid == 0){
    //if(execvp(args[0], args) == -1))
      perror("Error");
  }
  else if(pid < 0)
    perror("Error");
  else{
    //signal(SIGINT, sighand);
    waitpid(pid, NULL, 0);
  }
  return 1;
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
    if(pos >= 512){
      fprintf(stderr, "Line characters limit exceeded.\n");
      return NULL;
    }
  }
  return line;
}

char **split_line(char *line){
  int bufsize = LSH_RL_BUFSIZE;
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
    tokens[pos+1] = strtok(NULL, LSH_TOK_DELIM);
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

int change_dir(char **args){
  int dir;
  if(args[1] == NULL){
    fprintf(stderr, "Expected argument tp \"cd\"\n");
  }
  else if(strcasecmp("HOME", args[1]) == 0){
    dir = chdir(getenv("HOME"));
    if( dir == -1)
      perror("Error");
  }
  else{
    dir = chdir(args[1]);
    if(dir == -1)
      perror("Error");
  }
  return 1;
}

int setEnvironment(char **args){
  if(setenv(args[1], args[2], 1))
    perror("Error");
  return 1;
}

int unsetEnvironment(char **args){
  if(unsetenv(args[1]))
    perror("Error");
  return 1;
}

int terminate_kapish(){
  return 0;
}

void readFile(){
  char *path = malloc(sizeof(char)*(strlen(getenv("HOME")+strlen("/.kapishrc")+1)));
  if(!path){
    fprintf(stderr, "Error finding /.kapishrc\n");
    exit(EXIT_FAILURE);
  }
  path = memcpy(path, getenv("HOME"), strlen(getenv("HOME")));
  FILE *fp = fopen(strcat(path, "/.kapishrc"), "r");
  if(!fp){
    printf("Error: Can't find .kapishrc");
    return;
  }
  char line[512];
  printf("Reading .kapishrc\n");
  while(fgets(line, sizeof(line), fp))
    printf("? %s", line);
  printf("Done reading. \n");
  free(path);
  fclose(fp);
}

int execute(char **args){
  if(args[0] == NULL){
    return 1;
  }
  else if(strcmp(args[0],"cd") == 0)
    return change_dir(args);
  else if(strcmp(args[0],"exit") == 0)
    return terminate_kapish();
  else if(strcmp(args[0],"setenv") == 0)
    return setEnvironment(args);
  else if(strcmp(args[0],"unsetenv") == 0)
    return unsetEnvironment(args);
  else
    return lsh_launch(args);
}

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
    free(args);
  } while (status);
}

int main(int argc, char **argv){
  // Load config files
  // Run command loop
  lsh_loop();
  // Perform any cleanup/shutdown
}

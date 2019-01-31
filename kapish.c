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

/*
  Function Declarations for builtin shell commands:
 */
int kapish_cd(char **args);
int kapish_help(char **args);
int kapish_exit(char **args);
int kapish_setvar(char **args);
int kapish_unsetvar(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "cd",
  "setenv",
  "unsetenv",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &kapish_cd,
  &kapish_setvar,
  &kapish_unsetvar,
  &kapish_exit
};

int kapish_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int kapish_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "kapish: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("kapish: Error");
    }
  }
  return 1;
}

int kapish_setvar(char **args){
  char * var = args[2];
  if(var == NULL)
    var = "";
  if(setenv(args[1], var, 1))
    perror("kapish: Error");
  //printf("%s\n", getenv(args[1]));    Used for testing this function
  return 1;
}

int kapish_unsetvar(char **args){
  //printf("%s\n", getenv(args[1]));    Used for testing this function
  if(unsetenv(args[1]))
    perror("kapish: Error");
  //printf("%s\n", getenv(args[1]));    Used for testing this function
  return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int kapish_exit(char **args)
{
  return 0;
}

void read_kapishrc(){
  /* getting path to the home directory*/
	//char * path = malloc(sizeof(char)*(strlen(getenv("HOME"))+strlen("/.kapishrc")+1));
  char * path = getenv("HOME");
  if(!path){
		fprintf(stderr, "Problem allocating in read_kapishrc()\n");
		exit(EXIT_FAILURE);
	}

  /* reading .kapishrc file*/
	FILE * fp = fopen(strcat(path, "/.kapishrc"), "r");

	/* error in reading .kapishrc*/
	if(!fp){
    fprintf(stderr, "Problem reading kapishrc()\n");
		exit(EXIT_FAILURE);
	}

	char array[512];
	printf("Reading .kapishrc\n");
	while(fgets(array, sizeof(array), fp)){
		printf("? %s", array);
	}
  printf("\n");
	fclose(fp);
}

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int kapish_launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("kapish: Error");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("kapish: Error");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int kapish_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < kapish_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return kapish_launch(args);
}

#define kapish_RL_BUFSIZE 512
/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 */
char *kapish_read_line(void)
{
  int bufsize = kapish_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "kapish: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    if (c == EOF) {
      exit(EXIT_SUCCESS);
    } else if (c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      fprintf(stderr, "kapish: error exceeding limit\n");
      exit(EXIT_FAILURE);
    }
  }
}

#define kapish_TOK_BUFSIZE 64
#define kapish_TOK_DELIM " \t\r\n\a"
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **kapish_split_line(char *line)
{
  int bufsize = kapish_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "kapish: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, kapish_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += kapish_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
		free(tokens_backup);
        fprintf(stderr, "kapish: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, kapish_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

/**
   @brief Loop getting input and executing it.
 */
void kapish_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("? ");
    line = kapish_read_line();
    args = kapish_split_line(line);
    status = kapish_execute(args);

    free(line);
    free(args);
  } while (status);
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv)
{
  // Load config files, if any.
  // Run command loop.
  read_kapishrc();
  kapish_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}

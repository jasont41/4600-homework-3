/***************************************************************************//**

  @file         main.c

  @author       Jason Eastman

  @date         October 1, 2020

  @brief        LSH (Libstephen SHell)

*******************************************************************************/

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/*
  Function Declarations for builtin shell commands:
 */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

int setshellname(char **args);
int stop(char **args);
int setterminator(char ** args);
int newname(char **args);
int listNewNames();
int saveNewNames(char **args);
int readNewNames(char **args);
int help();

char *shellname = "myshell";
char terminator = '>';



int new_name_count = 0;
char new_name_arr [10][20]; //array for new names
char old_name_arr [10][20]={ //array for old names
 "cd",
 "help",
 "exit",
 "setshellname",
 "setterminator",
 "newname",
 "listNewNames",
 "saveNewNames",
 "readNewNames",
 "help"
};
/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = { //built in functions
  "cd",
  "help",
  "exit",
  "setshellname",
  "stop",
  "setterminator",
  "newname",
  "listNewNames",
  "saveNewNames",
  "readNewNames",
  "help"
};

int (*builtin_func[]) (char **) = { // memory address for functions
  &lsh_cd,
  &lsh_help,
  &lsh_exit,
  &setshellname,
  &stop,
  &setterminator,
  &newname,
  &listNewNames,
  &saveNewNames,
  &readNewNames,
  &help
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}
// ^ number of built in functions
/*
  Builtin function implementations.
*/
int help(){ //produces a list of built in functions
	for(int i = 0; builtin_str[i]; i++){
		printf("\n%s", builtin_str[i]);
	}
	return 1;
}

//reads new names from file
int readNewNames(char **args){
	char character;
	if(args[1] == NULL){
		printf("readNewNames <file_name>\n");
	}
	else
	{
		FILE *file;
		file =fopen(args[1],"r");
		if(file == NULL){
			printf("invalid file \n");
			return 1;
		}
		character = fgetc(file);
		while(character!= EOF){
			printf("%c",character);
			character = fgetc(file);
		}
		fclose(file);
	}
	return 1;

}


//writes new names from file
int saveNewNames(char **args){
	if(args[1] == NULL){
		printf("Not a valid argument\n");
	}
	else{
		FILE  *file;
		file = fopen(args[1], "w");
		for (int i = 0; i < new_name_count; i++){
			fprintf(file, "%s\t %s \n", new_name_arr[i], old_name_arr[i]);
		}
		fclose(file);
		return 1;
	}
}
//prints a list of new names
int listNewNames(){
	printf("New Names\n");
	for(int i = 0; i < new_name_count;i++){
		printf("%s\t %s\n", new_name_arr[i], old_name_arr[i]);
	}
	return 1;
}

//creates a new name for a built in function
int newname(char **args){
 int i = 0;
 int pos_arr=0;
   	if(args[1] == NULL) { printf("No name Entered"); }
  else if(args[2] == NULL){
    printf("Enter a valid no name");
    return 1;
  }
  else{
    while(i < new_name_count && old_name_arr[0] != args[2]){
	i++;

    }
  }
    if(old_name_arr[i] == args[2]){
    	strcpy(new_name_arr[i], args[1]);
	    //new_name_arr[i] == args[1];
    }
    else{
    	strcpy(old_name_arr[new_name_count],args[2]);
	strcpy(new_name_arr[new_name_count],args[1]);
    	//old_name_arr[pos_arr] = args[2];
       // new_name_arr[pos_arr] = args[1];
	new_name_count++;
    }
    return 1;
}

//sets new terminator
int setterminator(char **args){
if (args[1] == NULL){
	terminator = '>';
}
else{
	terminator = args[1][0];
}
}

//sets a new shell name
int setshellname(char **args)
{
    if(args[1] == NULL){
        shellname = "myshell";
    }
    else{
        shellname = args[1];
    }
    return 1;
}



/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int lsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

/**
   @brief Builtin command: print help.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */
int lsh_help(char **args)
{
  int i;
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int lsh_exit(char **args)
{
  return 0;
}

int stop(char **args)
{
	return 0;
}


/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int lsh_launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
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
int lsh_execute(char **args)
{
  int i;
  int j;
  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for(i = 0; i < new_name_count; i++){
  	if(strcmp(args[0],new_name_arr[i]) ==0)
	{
		args[0] = old_name_arr[i];
	}
  }

  for (j = 0;j < lsh_num_builtins(); j++) {
    if (strcmp(args[0], builtin_str[j]) == 0) {
      return (*builtin_func[j])(args);
    }
  }

  return lsh_launch(args);
}

/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 */
char *lsh_read_line(void)
{
#ifdef LSH_USE_STD_GETLINE
  char *line = NULL;
  ssize_t bufsize = 0; // have getline allocate a buffer for us
  if (getline(&line, &bufsize, stdin) == -1) {
    if (feof(stdin)) {
      exit(EXIT_SUCCESS);  // We recieved an EOF
    } else  {
      perror("lsh: getline\n");
      exit(EXIT_FAILURE);
    }
  }
  return line;
#else
#define LSH_RL_BUFSIZE 1024
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
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
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
#endif
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
		free(tokens_backup);
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

/**
   @brief Loop getting input and executing it.
 */
void lsh_loop(void)
{
  char *line;
  char **args;
  int status;

  do {
    printf("%s",shellname);
    printf("%c ",terminator);
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    //free(line);
    //free(args);
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
  lsh_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}

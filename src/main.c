#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <dirent.h>
#include "dynamicArray.h"


// Starts the shell.

#define MAX_COMMAND_SIZE 512
#define MIN_PATH_SIZE 1024
#define MAX_ARGS 64 

char** commandCompletion(const char* , int , int); 

char *commandMatch(const char* , int ); 

const char* builitInCommands[] = {
  "echo", 
  "exit", 
  "type",
  "pwd",
  "cd",
  NULL,

};

struct DynamicArrays darr ;




// Parsing the commands. 

int parseCommand(char* input , char**args ){

  int arg_count = 0 ; 

  int i = 0 ; 
  int len = strlen(input);

  while(i < len && arg_count < MAX_ARGS - 1){


    while(i < len && input[i] == ' '){

        i++;

    }

    if(i >= len){

      break;

    }

    char arg_buf[MAX_COMMAND_SIZE]; 

    int buf_pos = 0 ; 
    
    bool inarg = false; 

    // Parsing the command till a space is found. 

    while(i < len && input[i] != ' '){



      if(input[i] == '\\'){

        i++;
        if(i < len){

          arg_buf[buf_pos++] = input[i++];
        }

      }



    else  if(input[i] == '\''){
        
        i++; 
  
      
//  Everything between quotes is treated as literal.
      while(i < len && input[i] != '\''){

        if(buf_pos < MAX_COMMAND_SIZE - 1){

          arg_buf[buf_pos++] = input[i++];

        }


      }
      
      if(i >= len){

        return -1; 
      }
        
      i++;
    }

    else if(input[i] == '"'){

      i++;

      while(i < len && input[i] != '"'){

        if(buf_pos < MAX_COMMAND_SIZE - 1){

          if(input[i] == '\\' && i + 1 < len  && (input[i + 1] == '\\' || input[i + 1] == '"')){

            i++;

          }

          arg_buf[buf_pos++] = input[i++];
        
        }


      }

      if(i >= len){
        return -1; 
      }
      i++; 

    }



      else{

        while(i < len && input[i] != ' ' && input[i] != '\'' && input[i] != '"' && input[i] != '\\'){
          
          arg_buf[buf_pos++] = input[i++];}
          
        

      }

        inarg = true; 

      }


    
    if(inarg){

      arg_buf[buf_pos++] = '\0'; 
      args[arg_count++] = strdup(arg_buf);
    }





  }

  args[arg_count] = NULL; 

  return arg_count;
}



void executecd(char* path){

  if(strcmp(path, "~") == 0){

    char* home = getenv("HOME");

    chdir(home);


    return;

  }

  int check = chdir(path);

  if(check == -1){

    printf("cd: %s: No such file or directory\n", path);
  }



}

bool isBuiltin(const char* command){

  if(strcmp(command , "type") == 0 || 

    strcmp(command, "exit") == 0 || 

    strcmp(command , "cd") == 0 ||
    
    strcmp(command , "echo") == 0 ||
    
    strcmp(command , "pwd") == 0 
 ){

  return true; 

 }

 return false; 

}
void executepwd(void){

  char* dir = getcwd(NULL , 1024);

  if(dir == NULL){

    printf("NOt able to print directory."); 

  }
  else{
    printf("%s\n",dir);
  }

  free(dir); 

}

bool checkValid(char *command){

  if(strcmp(command, "type") == 0 || 

    strcmp(command, "exit") == 0 ||

    strcmp(command, "echo") == 0 || 

    strcmp(command, "pwd") == 0  || 

    strcmp(command , "cd") == 0)
  {
      return true;
  }

   char* path = strdup(getenv("PATH"));




  char* currPath = strtok(path , ":");
  do{
      
    char tosearch[MAX_COMMAND_SIZE];

    sprintf(tosearch ,"%s/%s" , currPath, command);

    if(access(tosearch , X_OK) == 0){


      free(path);

      return true; 

    }




  }while(currPath = strtok(NULL, ":"));

  free(path);

  return false; 



}





void executeEcho(char** input , int args_length){


  for(int i = 1 ; i < args_length && input[i] != NULL; i++){

    printf("%s", input[i]);
  }



printf("\n");



}
void executeType(char* command ){




  if(strcmp(command,"type") == 0 || strcmp(command , "exit") == 0 || strcmp(command,"echo") == 0
    || strcmp(command,"pwd") == 0 || strcmp(command, "cd") == 0){

      printf("%s is a shell builtin\n", command);
      return;
  }

  char* path = strdup(getenv("PATH"));




  char* currPath = strtok(path , ":");
  do{
      
    char tosearch[MAX_COMMAND_SIZE];

    sprintf(tosearch ,"%s/%s" , currPath, command);

    if(access(tosearch , X_OK) == 0){

      printf("%s is %s\n", command , tosearch);

      free(path);

      return; 

    }




  }while(currPath = strtok(NULL, ":"));

  free(path);

  printf("%s: not found\n", command);

}

void executeBuiltIn( char** args , int args_length){




    if(strcmp(args[0] , "exit") == 0){

      exit(0);

  }

  else if(strcmp(args[0] , "echo") == 0){
    
    executeEcho(args , args_length);
    
  }
  else if(strcmp(args[0], "type") == 0){

    // Because strtok, was called on input, it still have that string tokenize. 

    char* toFindType = args[1]; 

    executeType(toFindType );

    

  }

  else if(strcmp(args[0],"pwd") == 0){


      executepwd();

  }
  else if(strcmp(args[0] , "cd") == 0){

    char* path = args[1]; 

    executecd(path);

  }
  


}








int execute(char** args , int args_length ){

 
int pipeIndex = 0 ; 

while(pipeIndex < args_length && strcmp(args[pipeIndex] , "|") != 0){
  
  pipeIndex++;

}


bool containsPipe = pipeIndex != args_length; 


char **leftCmd = args; 


char **rightCmd = NULL; 
int fd[2]; 

if(pipe(fd) == -1){
  return 3; 
}

if(containsPipe){
  args[pipeIndex] = NULL;
  rightCmd = &args[pipeIndex + 1] ;

}

if(containsPipe == false && isBuiltin(leftCmd[0])){

  executeBuiltIn(leftCmd , args_length);
  return 0; 

}



int pid1 = -1 , pid2 = -1 ; 

if(!isBuiltin(leftCmd[0])){

  pid1 = fork();
  if(pid1 == -1)
  {

    return 2; 

  }

if(pid1 == 0){
  if (containsPipe){

    dup2(fd[1] , STDOUT_FILENO);
    leftCmd[pipeIndex] = NULL;

  }

  close(fd[0]);
  close(fd[1]); 




  execvp(leftCmd[0], leftCmd); 




  }
}


if(isBuiltin(leftCmd[0]) && containsPipe ){

  int stdCopy = dup(STDOUT_FILENO); 

  int copyFd = dup2(fd[1] , STDOUT_FILENO);

  executeBuiltIn(leftCmd, pipeIndex); 

  dup2(stdCopy , copyFd); 


  close(stdCopy);





}

if(containsPipe){


  if(isBuiltin(rightCmd[0])){

    int stdIn = dup(STDIN_FILENO); 
    int copySt = dup2(fd[0] , STDIN_FILENO); 

    executeBuiltIn(rightCmd , args_length - pipeIndex); 

    dup2(stdIn , copySt);

    close(stdIn);

  }


else{

  pid2 = fork(); 

  if(pid2 < 0){

    return 2; 

  }


  if(pid2 == 0){

    dup2(fd[0] , STDIN_FILENO); 

    close(fd[0]);
    close(fd[1]);

    execvp(rightCmd[0], rightCmd); 
  

  }


}}

close(fd[0]);
close(fd[1]); 
if(pid1 != -1){
waitpid(pid1 , NULL , 0); }

if(pid2 != -1){
  waitpid(pid2 , NULL , 0);
}
  
  





  

  


  return 0;
  
  

}
void handleCommand(char **args , int args_length){

  char* command = args[0]; 
  int redirection_index = -1 ; 
  int new_fd = -1; 
  int copy_stdout = dup(STDOUT_FILENO);
  int copy_stderr = dup(STDERR_FILENO);
  bool isErr = false; 
  bool isAppend = false; 

  for(int i = 0 ; i < args_length ; i++){

    if(strcmp(args[i] , ">") == 0 || strcmp(args[i],"1>") == 0 || strcmp(args[i] , "2>") == 0 

    || strcmp(args[i] , ">>") == 0 || strcmp(args[i], "1>>") == 0  
    
    || strcmp(args[i] , "2>>") == 0){

      redirection_index = i ; 
      if(strcmp(args[i] , "2>") == 0 || strcmp(args[i], "2>>") == 0) {

        isErr = true; 

      }

      if(strcmp(args[i], "1>>") == 0 || strcmp(args[i], ">>") == 0 
      
        || strcmp(args[i], "2>>") == 0){

        isAppend = true; 

      }

       break; 

    }

  }

  if(redirection_index != -1){
    int flags = O_WRONLY | O_CREAT;
    if(isAppend){

      flags = flags | O_APPEND;


    }

    int file = open(args[redirection_index + 1] , flags, 0777);

    if(isErr){

      new_fd = dup2(file, STDERR_FILENO);


    }
    else{

      new_fd = dup2(file , STDOUT_FILENO);

    }
    close(file);
    args[redirection_index] = NULL; 
    args_length = redirection_index; 




  }

  if(!checkValid(command)){
  
      printf("%s: command not found\n", command);

      return;

  }
    
  

    
  execute(args , args_length);
  

  if(new_fd != -1){

    if(isErr){

      dup2(copy_stderr , new_fd); 


    }
    else{

      dup2(copy_stdout , new_fd); 
    }

    


  }

  close(copy_stderr);
  close(copy_stdout);



}
void shellStart(void){

  rl_attempted_completion_function = commandCompletion;

    while(true){      
      char* input = readline("$ ");
      char *args[MAX_ARGS] ; 
      int args_length = parseCommand(input , args); 



      handleCommand(args , args_length); 

      free(input);


  }
}

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);
  darr = fillExecutable("PATH");
  shellStart();

  freeStorage(darr);
  return 0;
}

char *executableMatch(const char* text , int state){

  static int commandLength , startIndex;


  const char* name; 
  if(state == 0){

    commandLength = strlen(text); 
    startIndex = 0; 

  }

  while(startIndex < darr.currentIndex){

    name = darr.array[startIndex++];
    if(strncmp(name , text , commandLength) == 0){

 
        return strdup(name); 
    }
  }

  return NULL; 

  

  

}
char **commandCompletion(const char* text , int start , int end){

  rl_attempted_completion_over = 1; 





  char** matches = rl_completion_matches(text , commandMatch);

  if(matches != NULL && matches[0] != NULL){
    return matches;
  }
  return rl_completion_matches(text , executableMatch);
}

char *commandMatch(const char* text , int state ){

  const char* name; 
  static int commandLength , commandIndex; 

  if(state == 0){

    commandLength = strlen(text);

    commandIndex = 0;  

  }

  while((name = builitInCommands[commandIndex++]) != NULL){

    if(strncmp(name , text, commandLength) == 0){

      return strdup(name);

    }

  }

  return NULL; 

}

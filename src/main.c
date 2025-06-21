#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

// Starts the shell.

#define MAX_COMMAND_SIZE 512
#define MIN_PATH_SIZE 1024
#define MAX_ARGS 64 


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

    while(i < len && input[i] != ' '){

      if(input[i] == '\''){
        
        i++; 

      
        while(i < len && input[i] != '\''){


          if(buf_pos < MAX_COMMAND_SIZE){

            arg_buf[buf_pos++] = input[i];

          }

          i++;


        }

        if(i >= len){

          return -1;

        }

        i++; 

        inarg = true; 

      }

      else{

        while(i < len && input[i] != ' ' && input[i] != '\''){

            arg_buf[buf_pos++] = input[i];


            i++;
        }

        inarg = true; 
        





      
        }




    

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

void execute(char** args , int args_length ){

  



  // Copying, so during counting, input is not tampered.  






 


  int pid = fork();

  if(pid == 0){  

    execvp(args[0] , args);

              
  } 

  else{

  wait(0);
  
  }







  



}





void executeEcho(char** input , int args_length){


  for(int i = 1 ; i < args_length; i++){

    printf("%s\n", input[i]);
  }







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





void handleCommand(char **args , int args_length){

  char* command = args[0]; 

  if(!checkValid(command)){
  
      printf("%s: command not found\n", command);

      return;

  }

  if(strcmp(command , "exit") == 0){
    exit(0);  
  }

  else if(strcmp(command , "echo") == 0){
    executeEcho(args , args_length);
  }
  else if(strcmp(command, "type") == 0){

    // Because strtok, was called on input, it still have that string tokenize. 

    char* toFindType = args[1]; 

    executeType(toFindType );

  }

  else if(strcmp(command,"pwd") == 0){


      executepwd();

  }
  else if(strcmp(command , "cd") == 0){

    char* path = args[1]; 

    executecd(path);

  }
  else {
    
    execute(args , args_length);
  


  }



}
void shellStart(void){

    while(true){
      printf("$ ");
      char input[MAX_COMMAND_SIZE];
      if(fgets(input, MAX_COMMAND_SIZE, stdin) == NULL){
        return;
      }

      char* p = strchr(input, '\n');
      if(p != NULL){
        *p = '\0';
      }




      char *args[MAX_ARGS] ; 
      int args_length = parseCommand(input , args); 

      handleCommand(args , args_length); 


  }
}

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  shellStart();



  




  return 0;
}

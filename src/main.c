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



void executecd(char* path){

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
int countArgs(char* input){

  int count = 1; 
  char * temp; 
  while(temp = strtok(NULL , " ")){

    count++;

  }
  return count; 

}

void execute(char* command , char* input ){

  

  char tempinput[MAX_COMMAND_SIZE];

  // Copying, so during counting, input is not tampered.  


  strcpy(tempinput , input);

  int numArgs = countArgs(tempinput);

  char* args[numArgs + 1]; 

  args[0] = strtok(input, " "); 
 

  for(int i = 1 ; i < numArgs ; i++){

    char* toinsert ; 
    if(toinsert = strtok(NULL, " ")){

      args[i] = toinsert; 

    }

  }
  int err = 0 ; 
  args[numArgs] = NULL; 
  int pid = fork();

  if(pid == 0){  

    execvp(command , args);

              
  } 

  else{

  wait(0);
  
  }







  



}
void executeEcho(char* input, int len){


      printf("%s\n" , input + len + 1);


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





void handleCommand(char* command , char* inputProcess ){

  if(!checkValid(command)){
  
      printf("%s: command not found\n", command);

      return;

  }

  if(strcmp(command , "exit") == 0){
    exit(0);  
  }

  else if(strcmp(command , "echo") == 0){
    executeEcho(inputProcess , strlen(command));
  }
  else if(strcmp(command, "type") == 0){

    // Because strtok, was called on input, it still have that string tokenize. 

    char* toFindType = strtok(NULL , " ");

    executeType(toFindType );

  }

  else if(strcmp(command,"pwd") == 0){


      executepwd();

  }
  else if(strcmp(command , "cd") == 0){

    char* path = strtok(NULL, " ");

    executecd(path);

  }
  else {
    
    execute(command, inputProcess );
  


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




    char inputProcess[MAX_COMMAND_SIZE] ; // Copy made, as strtok, can affect the original string. 

    strcpy(inputProcess , input);
    char * command = strtok(input , " ");

    handleCommand(command , inputProcess);

  }
}

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  shellStart();



  




  return 0;
}

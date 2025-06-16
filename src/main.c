#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

// Starts the shell.

#define MAX_COMMAND_SIZE 512

void executeEcho(char* input, int len){


      printf("%s\n" , input + len + 1);


}
void executeType(void){

  char* command = strtok(NULL , " ");

  if(strcmp(command,"type") == 0 || strcmp(command , "exit") == 0 || strcmp(command,"echo") == 0){
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

  printf("%s: not found\n", command);

}





void handleCommand(char* inputProcess){

  char* command = strtok(inputProcess , " ");

  if(strcmp(command , "exit") == 0){
    exit(0);  
  }

  else if(strcmp(command , "echo") == 0){
    executeEcho(inputProcess , strlen(command));
  }
  else if(strcmp(command, "type") == 0){
    executeType();
  }

  else {

    printf("%s: command not found\n" , command);


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

    handleCommand(inputProcess);

  }
}

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  // Uncomment this block to pass the first stage
  shellStart();







  return 0;
}

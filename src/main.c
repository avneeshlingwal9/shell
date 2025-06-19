#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>

// Starts the shell.

#define MAX_COMMAND_SIZE 512

char *searchCommand(char* command){

    char *path = strdup(getenv("PATH"));

    char* currpath = strtok(path , ":");
    char *tosearch  = (char*)malloc(MAX_COMMAND_SIZE); 

    do{


      sprintf(tosearch , "%s/%s" , currpath ,command);


      if(access(tosearch , X_OK) == 0){

        free(path);
        return tosearch ; 

   

      }




    }while(currpath = strtok(NULL, ":"));

    free(path); 
    free(tosearch);
    return NULL;

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
  strcpy(tempinput , input);
  int num = countArgs(tempinput);

  char* args[num + 1]; 

  char* path = searchCommand(command);





  args[0] = strtok(input, " "); 
 

  for(int i = 1 ; i < num ; i++){

    char* toinsert ; 
    if(toinsert = strtok(NULL, " ")){

      args[i] = toinsert; 

    }

  }
  args[num] = NULL; 
  int pid = fork();

  if(pid == 0)
{  execvp(command , args);}
else{
  wait(0);
}






  



}
void executeEcho(char* input, int len){


      printf("%s\n" , input + len + 1);


}
void executeType(char* command ){



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

  free(path);

  printf("%s: not found\n", command);

}





void handleCommand(char* command , char* inputProcess ){

  if(strcmp(command , "exit") == 0){
    exit(0);  
  }

  else if(strcmp(command , "echo") == 0){
    executeEcho(inputProcess , strlen(command));
  }
  else if(strcmp(command, "type") == 0){
    executeType(inputProcess );
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);

  // Uncomment this block to pass the first stage


  // Wait for user input
  while(true){
    printf("$ ");
    char input[100];
    if(fgets(input, 100, stdin) == NULL){
      break;
    }


    char *pos = strchr(input, '\n');
    if(pos != NULL){
      *pos = '\0';
    }

    char inputProcess[100] ; // Copy made, as strtok, can affect the original string. 

    strcpy(inputProcess , input);

    char* command; 
    command = strtok(inputProcess , " ");
    if(strcmp(command , "exit") == 0){
      break;
    }

    else if(strcmp(command , "echo") == 0){

      printf("%s\n" , input + strlen(command) + 1);

    }
    else{

    printf("%s: command not found\n" , input);}

  }
  return 0;
}

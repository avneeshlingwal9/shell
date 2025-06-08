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

    if(strcmp(input , "exit 0") == 0){
      break;
    }

    printf("%s: command not found\n" , input);
  }
  return 0;
}

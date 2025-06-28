#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
struct DynamicArrays{
    int currentIndex;
    int length; 
    char **array;


};

struct DynamicArrays createArray(int len){

    struct DynamicArrays arr; 

    arr.length = len ; 
    arr.currentIndex = 0; 

    arr.array = ( char**)malloc(sizeof( char*) * len); 

    if(arr.array == NULL){
        exit(1); 
    }

    
    return arr; 
}

void resizeArray(struct DynamicArrays* arr ){

    arr->array = (char**)realloc(arr->array , sizeof(char*)* arr->length * 2);

    if(arr->array == NULL){
        free(arr->array); 
        exit(1);
    }

    arr->length = 2 * arr->length; 


}

void freeStorage(struct DynamicArrays arr){

    for(int i = 0 ; i < arr.currentIndex; i++){
        free((char*)arr.array[i]);
    }

    free(arr.array);

    

}

void insertElement(struct DynamicArrays* arr , char* el){

    arr->array[arr->currentIndex] = el; 
    arr->currentIndex++; 

}

bool isFull(struct DynamicArrays arr){

    return arr.currentIndex + 1 == arr.length;
}


struct DynamicArrays fillExecutable(const char *var){

     struct DynamicArrays dArray = createArray(10);

    char *path = strdup(getenv(var));

    char * curr = strtok(path , ":");

    while(curr != NULL){

        DIR *dir = opendir(curr);

        struct dirent* curr_el = readdir(dir);

        while(curr_el != NULL){

            insertElement(&dArray , strdup(curr_el->d_name)); 

            curr_el = readdir(dir);

            if(isFull(dArray)){

                resizeArray(&dArray);

            }


        }

        closedir(dir); 


        curr = strtok(NULL , ":");





    }

    free(path);

    return dArray;
}

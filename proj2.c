#include<stdio.h>
#include<semaphore.h>

sem_t semaphore;

void validate(int val){
    if ((val<0) && (val >1000)){
        printf(stderr ,"Error input. Please insert number in range 0 - 1000.");
        exit(1);
    } 
}

int main(int argc, char **argv){

    //Zpracovani parametru
    int NO = atoi(argv[1]);
    int NH = atoi(argv[2]);
    int TI = atoi(argv[3]);
    int TB = atoi(argv[4]);

    //Validace vstupnich dat
    validate(TI); validate(TB);

    sem_init(&semaphore, 0, 1);
    printf("lmao\n");



    sem_destroy(&semaphore);
    return 0;
}
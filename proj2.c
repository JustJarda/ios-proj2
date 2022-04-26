#include<stdio.h>
#include<semaphore.h>

sem_t semaphore;

//validace obsahu dat
void validate(int val){
    if ((val<0) && (val >1000)){
        printf(stderr ,"Error input. Please insert number in range 0 - 1000.");
        exit(1);
    } 
}

//validace poctu argumentu
void argcount(int argc){
    if (argc != 5){
        fprintf(stderr, "Too many or few arguments\n");
    }
}

int main(int argc, char **argv){

    //Zpracovani parametru
    int NO = atoi(argv[1]); //nuber of oxygen
    int NH = atoi(argv[2]); //number of hydrogen
    int TI = atoi(argv[3]); //time[ms] that atom waits to add himself to queue
    int TB = atoi(argv[4]); //time[ms] needed for make 1 molecule

    //Validace vstupnich dat
    validate(TI); validate(TB);argcount(argc);

    

    sem_init(&semaphore, 0, 1);
    int id = fork();
    if (id==0){
        printf("lmao\n");
    }


    sem_destroy(&semaphore);
    return 0;
}
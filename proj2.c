#include<stdio.h>
#include<semaphore.h>

sem_t semaphore;

//validace obsahu dat
void validate(int val){
    if ((val<0) || (val >1000)){
        fprintf(stderr ,"Error input. Please insert number in range 0 - 1000.\n");
        exit(1);
    } 
}

//validace poctu argumentu
void argcount(int argc){
    if (argc != 5){
        fprintf(stderr, "Too many or few arguments\n");
        exit(1);
    }
}

int main(int argc, char **argv){

    argcount(argc);
    //Zpracovani parametru
    int NO = atoi(argv[1]); //nuber of oxygen
    int NH = atoi(argv[2]); //number of hydrogen
    int TI = atoi(argv[3]); //time[ms] that atom waits to add himself to queue
    int TB = atoi(argv[4]); //time[ms] needed for make 1 molecule

    //Validace vstupnich dat
    validate(TI); validate(TB);

    

    sem_init(&semaphore, 0, 1);
    int id = fork();
    if (id==0){
        printf("lmao\n");
    }else{
        printf("parent  hehe\n");
    }


    sem_destroy(&semaphore);
    return 0;
}
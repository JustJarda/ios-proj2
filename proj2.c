#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <sys/mman.h>
#include <pthread.h> 


#define rand_sleep(max) {if(max!=0) usleep((rand()%max+1)*1000);}
#define MMAP(ptr) {(ptr) = mmap(NULL, sizeof(*(ptr)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);}
#define UNMAP(ptr) {munmap((ptr), sizeof(*(ptr)));}


void process_H(int id, int IT);
void process_O(int id, int IT);

sem_t mutex;
sem_t oxyQueue;
sem_t hydroQueue;

sem_t barrier_mutex;
sem_t turnstile;
sem_t turnstile2;

sem_t output;

int *molekula_cnt;
int *molekula_release;

int *oxygen;
int *hydrogen;

FILE *out;

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

    sem_init(&mutex, 1, 1);

    sem_init(&oxyQueue, 1, 0);
    sem_init(&hydroQueue, 1, 0);

    sem_init(&barrier_mutex, 1, 1);
    sem_init(&turnstile, 1, 0);
    sem_init(&turnstile2, 1, 1);

    // premenne
    MMAP(molekula_cnt);
    MMAP(molekula_release);
    MMAP(oxygen);
    MMAP(hydrogen);

    *molekula_cnt = 1;
    *molekula_release = 0;
    *oxygen = 0;
    *hydrogen = 0;

    out = fopen("proj2.out", "w");
    setbuf(out, NULL);

    for(int i=1; i<= NH; i++){
        pid_t id = fork();
        if(id==0){
            process_H(i,TI);
        }
        else if(id<0){
			fprintf(stderr, "ERR FORKING\n");
			exit(1);
		}
    }

    for(int i=1; i<=NO; i++){
        pid_t id = fork();
        if(id==0){
            process_O(i,TI);
        }
        else if(id<0){
			fprintf(stderr, "ERR FORKING\n");
			exit(1);
		}
    }

    while(wait(NULL) > 0);
    // tu je parent

    sem_destroy(&mutex);
    sem_destroy(&oxyQueue);
    sem_destroy(&hydroQueue);

    sem_destroy(&barrier_mutex);
    sem_destroy(&turnstile);
    sem_destroy(&turnstile2);

    UNMAP(molekula_cnt);
    UNMAP(molekula_release);
    UNMAP(oxygen);
    UNMAP(hydrogen);

    return 0;


}

void process_H(int id, int IT){
    printf("H %d started\n",id);

    rand_sleep(IT);
    
    printf("H %d queued\n",id);



    printf("H %d creating molecule %d\n",id, *molekula_cnt);
    

    exit(1);
}

void process_O(int id, int IT){
    printf("O %d started\n",id);

    rand_sleep(IT);

    printf("O %d queued\n",id);


    printf("O %d creating molecule %d\n",id, *molekula_cnt);

    exit(1);
}
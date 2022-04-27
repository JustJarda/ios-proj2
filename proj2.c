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
void generator(int H_cnt,int O_cnt, int IT);
sem_t semaphore;
sem_t sem_start_H;
sem_t sem_start_O;

sem_t sem_molecule_O;
sem_t sem_molecule_H;

sem_t sem_release;

int *molekula_cnt;
int *molekula_release;

int *O_CNT;
int *H_CNT;


int *mol_idx;



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

    

    sem_init(&semaphore, 1, 1);

    sem_init(&sem_start_H, 1, 2);
    sem_init(&sem_start_O, 1, 1);

    sem_init(&sem_molecule_H, 1, 0);
    sem_init(&sem_molecule_O, 1, 0);
    sem_init(&sem_release, 1, 0);

    // premenne
    MMAP(molekula_cnt);
    MMAP(molekula_release);
    MMAP(mol_idx);
    MMAP(O_CNT);
    MMAP(H_CNT);

    *molekula_cnt = 0;
    *molekula_release = 0;

    *mol_idx = 0;

    *O_CNT = NO;
    *H_CNT = NH;

    int id = fork();
    if (id==0){
        
        generator(NH,NO,TI);
        exit(0);
        
    }

    while(*H_CNT > 1 && *O_CNT > 0 ){
        int test = 0;
        sem_getvalue(&sem_molecule_H, &test);
        printf("--%d\n",test);  
        if(*molekula_cnt == 3){
            *mol_idx++;
            
            sem_post(&sem_molecule_H);
            sem_post(&sem_molecule_H);
            sem_post(&sem_molecule_O);
        }
        if(*molekula_release == 3){
            *molekula_cnt = 0;
            *molekula_release = 0;
            sem_post(&sem_release);
            sem_post(&sem_release);
            sem_post(&sem_release);
        }
    }

    // tu je parent


    sem_destroy(&semaphore);
    sem_destroy(&sem_start_H);
    sem_destroy(&sem_start_O);

    sem_destroy(&sem_molecule_H);
    sem_destroy(&sem_molecule_O);
    sem_destroy(&sem_release);

    UNMAP(molekula_cnt);
    UNMAP(molekula_release);
    UNMAP(mol_idx);
    UNMAP(O_CNT);
    UNMAP(H_CNT);


    return 0;


}

void generator(int H_cnt,int O_cnt, int IT){
    for(int i=1; i<= H_cnt; i++){
        pid_t id = fork();
        if(id==0){
            process_H(i,IT);
        }
        else if(id<0){
			fprintf(stderr, "ERR FORKING\n");
			exit(1);
		}
    }

    for(int i=1; i<= O_cnt; i++){
        pid_t id = fork();
        if(id==0){
            process_O(i,IT);
        }
        else if(id<0){
			fprintf(stderr, "ERR FORKING\n");
			exit(1);
		}
    }
}

void process_H(int id, int IT){
    printf("H %d started\n",id);

    rand_sleep(IT);
    
    printf("H %d queued\n",id);

    sem_wait(&sem_start_H);
    *molekula_cnt++;

    sem_wait(&sem_molecule_H);
    *molekula_release++;

    printf("H %d creating molecule %d\n",id,mol_idx);
    
    sem_wait(&sem_release);

    sem_post(&sem_start_H);

    *H_CNT--;
    exit(1);
}

void process_O(int id, int IT){
    printf("O %d started\n",id);

    rand_sleep(IT);

    printf("O %d queued\n",id);

    sem_wait(&sem_start_O);
    *molekula_cnt++;
    //uz caka na vytvorenie molekuly

    sem_wait(&sem_molecule_O);
    *molekula_release++;

    printf("O %d creating molecule %d\n",id,mol_idx);
    
    sem_wait(&sem_release);

    sem_post(&sem_start_O);


    *O_CNT--;
    exit(1);
}
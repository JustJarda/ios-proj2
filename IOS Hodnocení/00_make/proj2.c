//Just include stuffs
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <time.h>
#include <sys/mman.h>
#include <stdarg.h>//pro my_print

//Just define stuffs
#define rand_sleep(max) {if(max!=0) usleep((rand()%max+1)*1000);}
#define MMAP(ptr) {(ptr) = mmap(NULL, sizeof(*(ptr)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);}
#define UNMAP(ptr) {munmap((ptr), sizeof(*(ptr)));}
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

//Just Deklarace funkcí
void process_H(int id, int IT);
void process_O(int id, int IT);
void my_printf(const char * format, ...);
void argcount(int argc);
void validate(int val);
int mol_count(int oxygen, int hydrogen);

//Just Definice promennych
sem_t *mutex;
sem_t *oxyQueue;
sem_t *hydroQueue;

sem_t *barrier_mutex;
sem_t *turnstile;
sem_t *turnstile2;

sem_t *output;

int *molekula_cnt;
int *max_mol;
int *ocekavany_pocet;
int *mol_three;

int *oxygen;
int *hydrogen;
int *count;
int *line;

FILE *out;

/*-------------------------------------------------------------------------------------
------------------------------------------ JUST MAIN -----------------------------------
---------------------------------------------------------------------------------------*/

int main(int argc, char **argv){

    argcount(argc);//Kontrola, zda pocet parametru sedi

    /***************Zpracovani parametru***********/
    int NO = atoi(argv[1]); //nuber of oxygen
        if (NO<=0){fprintf(stderr, "Not Enought atoms :(\n");exit(0);}
    int NH = atoi(argv[2]); //number of hydrogen
        if (NH<=0){fprintf(stderr, "Not Enought atoms :(\n");exit(0);}
    int TI = atoi(argv[3]); //time[ms] that atom waits to add himself to queue
    int TB = atoi(argv[4]); //time[ms] needed for make 1 molecule

    /******************Validace vstupnich dat*************/
    validate(TI); validate(TB);

    /*************** Inicialzace semaforu *****************/
    MMAP(mutex);
    sem_init(mutex, 1, 1);

    MMAP(oxyQueue);
    sem_init(oxyQueue, 1, 0);
    MMAP(hydroQueue);
    sem_init(hydroQueue, 1, 0);

    MMAP(barrier_mutex);
    sem_init(barrier_mutex, 1, 1);
    MMAP(turnstile);
    sem_init(turnstile, 1, 0);
    MMAP(turnstile2);
    sem_init(turnstile2, 1, 1);
    MMAP(output);
    sem_init(output, 1, 1);


    /********* Sdilene promenne ***********/
    MMAP(molekula_cnt);
    MMAP(oxygen);
    MMAP(hydrogen);
    MMAP(count);
    MMAP(line);
    MMAP(max_mol);
    MMAP(ocekavany_pocet);
    MMAP(mol_three)

    *molekula_cnt = 0;
    *oxygen = 0;
    *hydrogen = 0;
    *count = 0;
    *line = 1;
    *max_mol = 1;
    *ocekavany_pocet = (MIN(NO*2, NH)/2);
    *mol_three = 0;

    /************Vystupni soubor**************/
    out = fopen("proj2.out", "w");
    setbuf(out, NULL);

    /***********Tvoreni procesu**************/

    *ocekavany_pocet = mol_count(NO, NH);
    if (*ocekavany_pocet==0)
    {
        for (int i = 0; i < NH; i++)
        {
            my_printf("H %d: not enough O or H\n", i);
        }
        for (int i = 0; i < NO; i++)
        {
            my_printf("O %d: not enough H\n", i);
        }        
        exit(0);
    }
    
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

    /************** Destroy semaforu a pameti******************/
    while(wait(NULL) > 0);

    sem_destroy(mutex);
    UNMAP(mutex);
    sem_destroy(oxyQueue);
    UNMAP(oxyQueue);
    sem_destroy(hydroQueue);
    UNMAP(hydroQueue);

    sem_destroy(barrier_mutex);
    UNMAP(barrier_mutex);
    sem_destroy(turnstile);
    UNMAP(turnstile);
    sem_destroy(turnstile2);
    UNMAP(turnstile2);
    sem_destroy(output);
    UNMAP(output);

    UNMAP(molekula_cnt);
    UNMAP(oxygen);
    UNMAP(hydrogen);
    UNMAP(line);
    UNMAP(count);
    UNMAP(max_mol);
    UNMAP(ocekavany_pocet);
    UNMAP(mol_three);

    return 0;

}

/*----------------------------------------------------------------------------------
---------------------------------JUST END OF MAIN ----------------------------------
---------------------------------------------------------------------------------*/

/******************** JUST DEFINICE FUNKCI ***********************/


/****** Proces vodík *******/
void process_H(int id, int IT){
    srand(time(NULL) * getpid());
    my_printf("H %d: started\n",id);

    rand_sleep(IT);

    my_printf("H %d: going to queue\n",id);
    sem_wait(mutex);
    *hydrogen+=1;

        if ((*hydrogen>=2)&&(*oxygen>=1)){
            (*molekula_cnt)++;
            sem_post(hydroQueue);
            sem_post(hydroQueue);
            *hydrogen-=2;
            sem_post(oxyQueue);
            *oxygen-=1;
        }else{
            sem_post(mutex);
        }
    
    
    sem_wait(hydroQueue);

    //fprintf(stderr, "%d\n", *molekula_cnt);
    if (*molekula_cnt > *ocekavany_pocet)

    {
        my_printf("H %d: not enough O or H\n", id);
        sem_post(hydroQueue);
        exit(0);
    }

    my_printf("H %d: creating molecule %d\n",id, *molekula_cnt);

    //barier
    sem_wait(barrier_mutex);
        *count +=1;
        if(*count==3){
            sem_wait(turnstile2);
            sem_post(turnstile);

        }
    sem_post(barrier_mutex);


    sem_wait(turnstile);
    sem_post(turnstile);

    my_printf("H %d: molecule %d created\n", id, *molekula_cnt);

    sem_wait(barrier_mutex);
        *count-=1;
        if (*count == 0){
            sem_wait(turnstile);
            sem_post(turnstile2);
        }
    sem_post(barrier_mutex);

    sem_wait(turnstile2);

    sem_post(turnstile2);
    //end of barrier

    exit(0);
}

/********* Proces Kyslík *********/
void process_O(int id, int IT){
    srand(time(NULL) * getpid());
    my_printf("O %d: started\n",id);

    rand_sleep(IT);

    my_printf("O %d: going to queue\n",id);
    sem_wait(mutex);
    *oxygen+=1;
    if (*hydrogen>=2){
        (*molekula_cnt)++;
        sem_post(hydroQueue);
        sem_post(hydroQueue);
        *hydrogen-=2;
        sem_post(oxyQueue);
        *oxygen-=1;
    }else{
        sem_post(mutex);
    }
    
    
    sem_wait(oxyQueue);

    if (*molekula_cnt > *ocekavany_pocet)
    {
        my_printf("O %d: not enough H\n", id);
        sem_post(oxyQueue); // let next oxygen pass
        exit(0);
    }
    my_printf("O %d: creating molecule %d\n",id, *molekula_cnt);

    //barier
    sem_wait(barrier_mutex);
        *count +=1;
        if(*count==3){
            sem_wait(turnstile2);
            sem_post(turnstile);

        }
    sem_post(barrier_mutex);  

    sem_wait(turnstile);
    sem_post(turnstile);

    my_printf("O %d: molecule %d created\n", id, *molekula_cnt);

    sem_wait(barrier_mutex);
        *count-=1;
        if (*count == 0){
            sem_wait(turnstile);
            sem_post(turnstile2);
        }
    sem_post(barrier_mutex);

    sem_wait(turnstile2);
    sem_post(turnstile2);
    //end of barrier
    if (*molekula_cnt == *ocekavany_pocet) {
        // Signal to subsequent processes that we are over the limit
	//fprintf(stderr, "done\n");
        (*molekula_cnt)++; 
        sem_post(oxyQueue);
        sem_post(hydroQueue);
    }

    sem_post(mutex);

    exit(0);
}

/******* Vylepšený printf *******/
void my_printf(const char * format, ...)
{
    sem_wait(output);
    va_list args;
    va_start (args, format);
    fprintf(out, "%d: ", *line);
    (*line)++;
    vfprintf (out, format, args);
    va_end (args);
    sem_post(output);
}

//validace poctu argumentu
void argcount(int argc){
    if (argc != 5){
        fprintf(stderr, "Too many or few arguments\n");
        exit(1);
    }
}

//validace obsahu dat
void validate(int val){
    if ((val<0) || (val >1000)){
        fprintf(stderr ,"Error input. Please insert number in range 0 - 1000.\n");
        exit(1);
    } 
}

//pocet
int mol_count(int oxygen, int hydrogen)
{
	return ((2*oxygen) < hydrogen) ? oxygen : (hydrogen/2);
}


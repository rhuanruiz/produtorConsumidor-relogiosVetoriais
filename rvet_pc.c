/**
 * Compilação: mpicc -o rvet_pc rvet_pc.c -lpthread 
 * Execução:   mpiexec -n 3 ./rvet_pc
 * 
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mpi.h>     
#include <pthread.h>
#include <semaphore.h>
#define tamanho_fila 40  

sem_t esvazia;
sem_t enche;
pthread_mutex_t mutex;

sem_t esvazia2;
sem_t enche2;
pthread_mutex_t mutex2;

typedef struct Clock {
    int pid;
    int pid_dest;
    int p[3];
} Clock;

int i1 = 0, i2 = 0, j1 = 0, j2 = 0;
int pid;

Clock a_i[tamanho_fila];
Clock a_j[tamanho_fila];
Clock clock1;

void produtor1(Clock c){   
    
    Clock clock_p1 = c;
        
    sem_wait(&esvazia);
    pthread_mutex_lock(&mutex);
        
    a_i[i1] = clock_p1;
    i1 = (i1+1) % tamanho_fila;
        
    pthread_mutex_unlock(&mutex);
    sem_post(&enche);

}

void produtor2(Clock c){   
    
    Clock clock_p2 = c;

    sem_wait(&esvazia2);
    pthread_mutex_lock(&mutex2);
    
    a_j[i2] = clock_p2;
    i2 = (i2+1) % tamanho_fila;
    
    pthread_mutex_unlock(&mutex2);
    sem_post(&enche2);

}

Clock consumidor1(){   

    sem_wait(&enche);
    pthread_mutex_lock(&mutex);
        
    Clock clock_c1 = a_i[j1];
    j1 = (j1+1) % tamanho_fila;
    
    pthread_mutex_unlock(&mutex);
    sem_post(&esvazia);
    
    return clock_c1;
    
}

Clock consumidor2(){   
    
    sem_wait(&enche2);
    pthread_mutex_lock(&mutex2);
        
    Clock clock_c2 = a_j[j2];
    j2 = (j2+1) % tamanho_fila;
        
    pthread_mutex_unlock(&mutex2);
    sem_post(&esvazia2);
    
    return clock_c2;
    
}

void Event(int pid, Clock *clock){
   clock->p[pid]++;
}

void *Send (){
    
    while(1){
        
    Clock c = consumidor1();
    MPI_Send(c.p, 3, MPI_INT, c.pid_dest, 0, MPI_COMM_WORLD);
    
    }
    
}

void *Receive (){
    
    while(1){
        
    Clock clock = clock1; 
    
    MPI_Recv(clock.p, 3, MPI_INT, clock.pid_dest, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    int k = 0;
    while(k < 3){
        if(clock1.p[k] > clock.p[k]){
            clock.p[k] = clock1.p[k];
        }
    k++;
    }
    
    produtor2(clock);
    
    }
    
}

void process0(){
    
    pid = 0;
    
    clock1.p[0] = 0;
    clock1.p[1] = 0;
    clock1.p[2] = 0;
    
    pthread_t t1, t2;
    
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex2, NULL);
    
    sem_init(&esvazia, 0, tamanho_fila);
    sem_init(&enche, 0, 0);
    sem_init(&esvazia2, 0, tamanho_fila);
    sem_init(&enche2, 0, 0);
    
    pthread_create(&t1, NULL, (void*)Send, NULL);
    pthread_create(&t2, NULL, (void*)Receive, NULL);

    Event(pid, &clock1);
    
    Event(pid, &clock1);
    clock1.pid_dest = 1;
    produtor1(clock1);
   
    clock1 = consumidor2();
    Event(pid, &clock1);
   
    Event(pid, &clock1);
    clock1.pid_dest = 2;
    produtor1(clock1);
   
    clock1 = consumidor2();
    Event(pid, &clock1);
   
    Event(pid, &clock1);
    clock1.pid_dest = 1;
    produtor1(clock1);

    Event(pid, &clock1);
   
    printf("Process: %d, Clock: (%d, %d, %d)\n", 0, clock1.p[0], clock1.p[1], clock1.p[2]);
   
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex2);
    sem_destroy(&esvazia);
    sem_destroy(&enche);
    sem_destroy(&esvazia2);
    sem_destroy(&enche2); 

}

void process1(){

    pid = 1;
    
    clock1.p[0] = 0;
    clock1.p[1] = 0;
    clock1.p[2] = 0;
    
    pthread_t t1, t2;
    
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex2, NULL);
    
    sem_init(&esvazia, 0, tamanho_fila);
    sem_init(&enche, 0, 0);
    sem_init(&esvazia2, 0, tamanho_fila);
    sem_init(&enche2, 0, 0);
    
    pthread_create(&t1, NULL, (void*)Send, NULL);
    pthread_create(&t2, NULL, (void*)Receive, NULL);
   
    Event(pid, &clock1);
    clock1.pid_dest = 0;
    produtor1(clock1);
   
    clock1 = consumidor2();
    Event(pid, &clock1);
   
    clock1 = consumidor2();
    Event(pid, &clock1);

    printf("Process: %d, Clock: (%d, %d, %d)\n", 1, clock1.p[0], clock1.p[1], clock1.p[2]);
   
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex2);
    sem_destroy(&esvazia);
    sem_destroy(&enche);
    sem_destroy(&esvazia2);
    sem_destroy(&enche2); 
    
}

void process2(){
   
    pid = 2;
    
    clock1.p[0] = 0;
    clock1.p[1] = 0;
    clock1.p[2] = 0;
    
    pthread_t t1, t2;
    
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex2, NULL);
    
    sem_init(&esvazia, 0, tamanho_fila);
    sem_init(&enche, 0, 0);
    sem_init(&esvazia2, 0, tamanho_fila);
    sem_init(&enche2, 0, 0);
    
    pthread_create(&t1, NULL, (void*)Send, NULL);
    pthread_create(&t2, NULL, (void*)Receive, NULL);
   
    Event(pid, &clock1);
   
    Event(pid, &clock1);
    clock1.pid_dest = 0;
    produtor1(clock1);

    clock1 = consumidor2();
    Event(pid, &clock1);

    printf("Process: %d, Clock: (%d, %d, %d)\n", 2, clock1.p[0], clock1.p[1], clock1.p[2]);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex2);
    sem_destroy(&esvazia);
    sem_destroy(&enche); 
    sem_destroy(&esvazia2);
    sem_destroy(&enche2); 

}

int main(void) {

   int my_rank;    
   
   MPI_Init(NULL, NULL); 
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 

   if (my_rank == 0) { 
      process0();
   } else if (my_rank == 1) {  
      process1();
   } else if (my_rank == 2) {  
      process2();
   }

   MPI_Finalize(); 
   
   return 0;
   
}  
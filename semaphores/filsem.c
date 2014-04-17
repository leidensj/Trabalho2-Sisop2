#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define TEMPO_MAX 10

int n;
sem_t mutexStatus;
sem_t* garfos;
sem_t cadeiras;
char *status;

void atualizarStatus(char novoStatus, int id)
{
    sem_wait(&mutexStatus);
    *(status + id) = novoStatus;
    int i;
    for(i = 0; i < n; i++)
    {
        if(i == n-1)
            printf("%c\n", *(status+i));
        else
            printf("%c ", *(status+i));
    }
    sem_post(&mutexStatus);
}

void *filosofar(void *arg) 
{
    int id = *(int *) arg;
    int esquerda = id;
    int direita = (id + 1) % n;
    while(1)
    {        
        sleep(rand() % (TEMPO_MAX + 1));
        atualizarStatus('H', id);

        sem_wait(&cadeiras);

        sem_wait(garfos + esquerda);
        sem_wait(garfos + direita);

        atualizarStatus('E', id);
        sleep(rand() % (TEMPO_MAX + 1));
        atualizarStatus('T', id);
        sleep(rand() % (TEMPO_MAX + 1));

        sem_post(garfos + esquerda);
        sem_post(garfos + direita);

        sem_post(&cadeiras);
    }

}

int main(int argc, char *argv[]) 
{
    if(argc < 2)
    {
        printf("\nInforme o numero de filosofos.\n\n");
        exit(-1);
    }

    n = atoi(argv[1]);
    if(n <= 1)
    {
        printf("\nE preciso ter no minimo dois filosofos.\n\n");
        exit(-1);
    }

    sem_init(&mutexStatus, 0, 1);
    pthread_t filosofo[n];
    
    garfos = (sem_t *)malloc(n * sizeof(sem_t));
    status = (char *)malloc(n * sizeof(char));
    sem_init(&cadeiras, 0, n-1);
    
    int i, id[n];
    for(i = 0; i < n; i++)
    {
        sem_init(garfos+i, 0, 1);
        *(status + i) = 'T';
        id[i] = i;
    }
    for(i = 0; i < n; i++)
        pthread_create(filosofo+i, NULL, filosofar, id+i);
    while(1);
    //free(garfos);
    //free(status);
}

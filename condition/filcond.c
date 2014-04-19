#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define TEMPO_MAX 10

int n;
unsigned int* ugarfoLivre;
pthread_cond_t *garfoLivre;
pthread_mutex_t *mutex;
pthread_mutex_t mutexStatus;
char *status;

void atualizarStatus(char novoStatus, int id)
{
    pthread_mutex_lock(&mutexStatus);
    *(status + id) = novoStatus;
    int i;
    for(i = 0; i < n; i++)
    {
        if(i == n-1)
            printf("%c\n", *(status+i));
        else
            printf("%c ", *(status+i));
    }
    pthread_mutex_unlock(&mutexStatus);
}

void *filosofar(void *arg) 
{
    int id = *(int *) arg;
    int esquerda, direita;
    if(id == 0)
    {
        esquerda = 1;
        direita = 0;
    }
    else
    {
        esquerda = id;
        direita = (id + 1) % n;
    }

    sleep(rand() % (TEMPO_MAX + 1));
    while(1)
    {      
        atualizarStatus('H', id);  

        pthread_mutex_lock(mutex + esquerda);
        if(*(ugarfoLivre + esquerda))
            *(ugarfoLivre + esquerda) = 0;
        else
        {
            pthread_cond_wait(garfoLivre + esquerda, mutex + esquerda);
            *(ugarfoLivre + esquerda) = 0;
        }
            
        pthread_mutex_lock(mutex + direita);
        if(*(ugarfoLivre + direita))
            *(ugarfoLivre + direita) = 0;
        else
        {
            pthread_cond_wait(garfoLivre + direita, mutex + direita);
            *(ugarfoLivre + direita) = 0;
        }
            
        atualizarStatus('E', id);
        sleep(rand() % (TEMPO_MAX + 1));

        *(ugarfoLivre + esquerda) = 1;
        *(ugarfoLivre + direita) = 1;

        pthread_cond_signal(garfoLivre + esquerda);
        pthread_mutex_unlock(mutex + esquerda);
        pthread_cond_signal(garfoLivre + direita);
        pthread_mutex_unlock(mutex + direita);

        atualizarStatus('T', id);
        sleep(rand() % (TEMPO_MAX + 1));
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

    pthread_t filosofo[n];
    
    garfoLivre = (pthread_cond_t *)malloc(n * sizeof(pthread_cond_t));
    mutex = (pthread_mutex_t *)malloc(n * sizeof(pthread_mutex_t));
    status = (char *)malloc(n * sizeof(char));
    ugarfoLivre = (unsigned int*)malloc(n * sizeof(unsigned int));
    pthread_mutex_init(&mutexStatus, NULL);
    int i, id[n];
    for(i = 0; i < n; i++)
    {
        pthread_cond_init(garfoLivre + i, NULL);
        pthread_mutex_init(mutex + i, NULL);
        *(status + i) = 'T';
        id[i] = i;
        *(ugarfoLivre + i) = 1;
    }
    for(i = 0; i < n; i++)
        pthread_create(filosofo+i, NULL, filosofar, id+i);
        
    while(1);
}

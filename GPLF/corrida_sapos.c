/*
Elabore um programa que implemente uma corrida de sapos. O responsável pela corrida
dá a partida e, a partir daí, cada sapo tem comportamento autônomo e realiza um pulo
a cada segundo. Cada pulo pode saltar de 1 a 6 casas (como jogar um dado). Para saber
a quantidade de casas relativas a um pulo é gerado aleatoriamente um número N
pertencente ao intervalo [0,60] e o número de casas correspondente ao pulo é igual a
(N%6)+1, onde % significa o resto da divisão inteira. A corrida contará com 5 sapos
(numerados de 0 a 4), que irão pular 60 casas. O responsável toma conhecimento dos
pulos de cada sapo a cada segundo e, quando a corrida terminar, divulga a classificação.
Utilize threads para implementar o programa.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h> 
#include <pthread.h>

#define SAPOS 5  //sapos
#define RAND_LOWER_BOUND 0
#define RAND_UPPER_BOUND 60

// devolve um numero entre os limites
int limitedRand(int min, int max);

/*
 função executada por cada thread
 vai chamar a multmm para as linhas que for responsável
 recebe um thread_data como argumento
*/
void* work(void *workerArg);

int iteracoes[SAPOS];

int main(void)
{
    unsigned int status = 0;
    pthread_t threads[SAPOS];
    int sapo_id[SAPOS];
    

    srand(time(0));
    
    for(int i = 0; i < SAPOS; i++) {
        sapo_id[i] = i;
        iteracoes[i] = 0;
        pthread_create(&threads[i], NULL, work, (void *)&sapo_id[i]);
    }

    for (int i = 0; i < SAPOS; i++) {
        pthread_join(threads[i], (void*)&status);
    }
    int min,id;
    min = iteracoes[0];
    for (int i = 0; i < SAPOS; i++) {
        printf("Sapo %d levou %d iterações\n",i+1, iteracoes[i]);    
        if(iteracoes[i]<=min) {
            min = iteracoes[i];
            id = i;
        }
    }
    printf("vencedor sapo: %d\n",id);
    
    return 0;
}


void* work(void *workerArg) {
    int distanceJumped = 0;
    while(distanceJumped < 60) {
        int nextJump = (limitedRand(RAND_LOWER_BOUND,RAND_UPPER_BOUND) % 6) + 1;
        distanceJumped += nextJump;
        printf("Sapo %d andou %d e já caminhou %d\n",*(unsigned int*) workerArg,nextJump,distanceJumped);
        sleep(1);
        iteracoes[*(unsigned int*) workerArg]++;
    }

    return workerArg;
}

int limitedRand(int min, int max) 
{   
    return (rand() % (min - max + 1)) + min; 

}
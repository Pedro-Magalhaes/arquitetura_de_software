#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h> 
//vou definir o numero de filhos a serem criados
#define FILHOS 20
// tamanho do array com os elementos
#define ARRAY_SIZE 4096
#define RAND_LOWER_BOUND 0
#define RAND_UPPER_BOUND 400

// devolve um numero entre os limites
int limitedRand(int min, int max);

int main(void)
{
    int pid_filho, status;
    int i;
    int c[ARRAY_SIZE];
    int size = ARRAY_SIZE; //tamanho do vetor
    int passo, inicio = 0;

    passo = size / FILHOS;     //passo é o numero de posições que cada filho procura
    int sobra = size % FILHOS; //se há sobra, o ultimo filho vai procurar passo+sobra posições
    
    srand(time(0)); 
    
    int procurado = limitedRand(RAND_LOWER_BOUND,RAND_UPPER_BOUND);
    printf("Numero a ser buscado <%d> \n",procurado);
    // seed do rand é o tempo atual
    

    for (size_t i = 0; i < size; i++)
    {
        c[i] = limitedRand(RAND_LOWER_BOUND,RAND_UPPER_BOUND);
    }
    

    for (i = 0; i < FILHOS; i++)
    {
        inicio = i * passo; //posição inicial no vetor que cada fiho procura

        pid_filho = fork();
        if (pid_filho == 0)
        {
            break;
        }
            
    }

    if (pid_filho != 0)
    {
        for (int i = 0; i < FILHOS; i++)
        {
            wait(&status); /* wait for child to exit */
        }
    }
    else
    {
        pid_filho = getpid();
        int fim = inicio + passo; // inicio e passo é global para as threads e iniciado antes do fork
        int ocorrencias = 0;
        if (sobra && i == FILHOS - 1) //se o filho é o ultimo adiciono a sobra
        {
            fim = fim + sobra;
        }
        for (; inicio < fim; inicio++)
        {
            if (procurado == c[inicio])
            {
                ocorrencias++;
                printf("PID<%d>, Filho <%d> Encontrou na posição %d do vetor.\n", pid_filho, i, inicio);
            }
        }
        printf("PID<%d> fim da execução, filho numero:%d encontrou <%d> ocorrencias\n", pid_filho, i, ocorrencias);
        exit(-1);
    }

    return 0;
}


int limitedRand(int min, int max) 
{   
    return (rand() % (min - max + 1)) + min; 

}
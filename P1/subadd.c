/*
 Escreva em linguagem C um programa que defina um vetor
 de 1024 posições de tipo inteiro longo inicializados com o valor 10.
 Seu programa deve gerar duas threads, uma delas soma 1 aos elementos 
 de índice par e a outra subtrai 1 dos elementos de índice ímpar. 
 Use processamento vetorial para realizar a inicialização do vetor e as 
 operações sobre os vetores.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <immintrin.h>
#define N 8  // se for == 8 printa o resultado no vetor
#define ARRAY_STEP 4

long long int *A;
__m256i mask_all;

void* work(void *workerArg) {
    int initial_pos;
    int add_number = *((int*) workerArg); // -1 para sub, 1 para add
    __m256i mask_save;
    long long int *nxt_a = A;
    long unsigned int i = 0;
    __m256i vetor_operando = _mm256_set1_epi64x(1); //incremento / decremento
    
    if( add_number > 0 ) {//adiçao
      mask_save = _mm256_setr_epi64x(-1,0,-1,0); // pos par
    } else { // sub
      mask_save = _mm256_setr_epi64x(0,-1,0,-1); // // pos impar
    }

    for(i=0;i<N; i+=ARRAY_STEP, nxt_a+=ARRAY_STEP) {
      __m256i vec_a = _mm256_maskload_epi64(nxt_a,mask_all);
      __m256i r;
      if(add_number > 0){ // adiciona 1
          r = _mm256_add_epi64(vec_a,vetor_operando);
      } else { // subtrai 1
          r = _mm256_sub_epi64(vec_a,vetor_operando);
      }
      _mm256_maskstore_epi64(nxt_a,mask_save,r);
    }
    return workerArg;
}

int main(void) {
    pthread_t thread[2];
    int status;
    long long int *nxt_a;
    long unsigned int i = 0;
    int sub = -1;
    int add = 1;
    A =  (long long int*)aligned_alloc(64, N*sizeof(long long int));
    nxt_a = A;
    if (A == NULL) {
      printf("problema na alocação do vetor.");
      return 1;
    }
    mask_all = _mm256_set1_epi64x(-1); // usa todo vetor
    __m256i vec_a = _mm256_set1_epi64x(32);

    for(i=0;i<N; i+=ARRAY_STEP, nxt_a+=ARRAY_STEP) {
      /* Armazena elementos do vetor avx nos vetores em memória */
      _mm256_maskstore_epi64(nxt_a, mask_all ,vec_a);
    }

    pthread_create(&thread[0], NULL, work, &sub);
    pthread_create(&thread[1], NULL, work, &add);  

    pthread_join(thread[0], (void*)&status);
    pthread_join(thread[1], (void*)&status);
    
    if(N == 8) {
        for(i=0;i<N;i++) {
            printf(" %lld ",A[i]);
        }
        printf(" \n ");
    }
    return 0;
}
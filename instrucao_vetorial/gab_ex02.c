#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

int main (int argc, char *argv[]) {
  long unsigned int N;
  long unsigned int i = 0;

  //variáveis para tomada de tempo
  struct timeval ini, fim;
  float diff;

  // Verifica argumentos
  printf("Argumentos: %d\n", argc);
  for (i=0; i<argc; ++i)
    printf("argv[%ld] = %s\n", i, argv[i]);

  // Converte tamanho do vetor para inteiro
  N = atoi(argv[1]);

  /* declara vetores em memória */
  float *a =  (float*)aligned_alloc(32, N*sizeof(float));
  float *b = (float*)aligned_alloc(32, N*sizeof(float));
  float *c = (float*)aligned_alloc(32, N*sizeof(float));
  float *result = (float*)aligned_alloc(32, N*sizeof(float));

  if ((a == NULL) || (a == NULL) || (c == NULL) || (result == NULL)) {
    printf("%s: problema na alocação dos vetores em memoria\n", argv[0]);
    return 1;
  }

  // inicia tomada de tempo
  gettimeofday(&ini,NULL);

  /* Inicializa vetores sequencialmente */
  for (i=0;i<N;i++){
    a[i]=5.0;
    b[i]=2.0;
    c[i]=3.0;
  }

  /* Calcula a multiplicação e soma dos vetores em memória sequencialmente */
  for(i=0;i<N;i++){
    result[i] = a[i] * b[i] + c[i];

  }

  // Termina a tomada de tempo
  gettimeofday(&fim,NULL);
  diff=(fim.tv_sec - ini.tv_sec) * 1000.0f + (fim.tv_usec - ini.tv_usec) / 1000.0f;
  printf("tempo de execução: %f segundos\n",diff);

  /*
  for (i=0;i<N;i++)
    printf("result[%ld] = %f\n", i, result[i]);
  */

  return 0;
}
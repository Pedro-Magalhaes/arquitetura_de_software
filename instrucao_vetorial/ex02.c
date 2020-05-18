#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

int main (int argc, char *argv[]) {
  long unsigned int N;
  long unsigned int i = 0;
  char *eptr = NULL;

  // Verifica argumentos
  printf("Argumentos: %d\n", argc);
  for (i=0; i<argc; ++i)
    printf("argv[%ld] = %s\n", i, argv[i]);

  // Converte tamanho do vetor para inteiro
  N = atoi(argv[1]);

  /* declara vetores em memória */
  float *A =  (float*)aligned_alloc(32, N*sizeof(float));
  float *B = (float*)aligned_alloc(32, N*sizeof(float));
  float *C = (float*)aligned_alloc(32, N*sizeof(float));
  float *result = (float*)aligned_alloc(32, N*sizeof(float));

  if ((A == NULL) || (B == NULL) || (result == NULL)) {
    printf("%s: problema na alocação do vetor.", argv[0]);
    return 1;
  }

  /* Inicializa vetores avx */
  __m256 vec_a = _mm256_set1_ps(5.0f);
  __m256 vec_b  = _mm256_set1_ps(2.0f);
  __m256 vec_c  = _mm256_set1_ps(3.0f);
  __m256 vec_result = _mm256_set1_ps(0.0f);


  float *nxt_a = A;
  float *nxt_b = B;
  float *nxt_c = C;
  for(i=0;i<N; i+=8, nxt_a+=8, nxt_b+=8, nxt_c+=8) {

      /* Armazena elementos do vetor avx nos vetores em memória */
      _mm256_store_ps(nxt_a, vec_a);
      _mm256_store_ps(nxt_b, vec_b);
      _mm256_store_ps(nxt_c, vec_c);
  }
 

  /* Calcula a diferença dos vetores em memória via avx */
  nxt_a = A;
  nxt_b = B;
  nxt_c = C;
  float *nxt_result = result;

  clock_t inicial_vetorial = clock();
  
  for(i=0;i<N;i+=8, nxt_a+=8, nxt_b+=8, nxt_c+=8, nxt_result+=8){

      /* inicializa avx com 8 floats dos vetores nxt_a e nxt_b */
      __m256 vec_a = _mm256_load_ps(nxt_a);
      __m256 vec_b  = _mm256_load_ps(nxt_b);
      __m256 vec_c  = _mm256_load_ps(nxt_c);
      /* calcula a diferença em avx */
      __m256 vec_result = _mm256_fmadd_ps(vec_a, vec_b, vec_c);

      /* armazena resultado no vetor em memória */
      _mm256_store_ps(nxt_result, vec_result);

  }
  clock_t final_vetorial = clock();

  clock_t inicial_sequencial = clock();
  for(i=0;i<N;i++){
    result[i] = A[i] * B[i] + C[i];
  }
  clock_t final_sequencial = clock();
  
  long clocks_sequencial = final_sequencial - inicial_sequencial;
  long clocks_vetorial = final_vetorial - inicial_vetorial;

  printf("Clocks Sequencial: %ld\nClocks Vetorial: %ld\n",clocks_sequencial,clocks_vetorial);

  printf("Ganho: %lf\n",clocks_sequencial/(double)clocks_vetorial);

  return 0;
}
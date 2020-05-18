#include <immintrin.h>
#include <stdio.h>
#include <time.h>

int checkArray(float array[], int size, float expected, float tol) {
    for(int i = 0; i < size; i++) {
        if(array[i] < (expected - tol)) {
            return 0;
        }
    }
    return 1;
}

int main() {

    int size = 8 * 40000;
    float a[size];
    float b[size];
    float c[size];

    clock_t inicio;
    clock_t fim;

    for(int i = 0; i < size; i++) {
        a[i] = 8.0f;
        b[i] = 5.0f;
        c[i] = 0.0f;
    }

    /** SUBTRAÇÃO VETORIAL **/
    inicio = clock();
    for(int i = 0; i < size; i+=8) {
        __m256 ii = _mm256_load_ps(&a[i]);
        __m256 _2i = _mm256_load_ps(&b[i]);
        __m256 result = _mm256_sub_ps(ii, _2i);
        _mm256_store_ps(&c[i], result);
    }
    fim = clock();

    printf("Vetorial: %ld\n", fim - inicio);
    // checando se o vertor está correto
    if(checkArray(c,size,3.0f,0.01f) != 1) {
        printf("Vetor continha erros de subtração");
    }


    /** SUBTRAÇÃO SEQUENCIAL **/
    inicio = clock();
    for(int i = 0; i < size; i++){
        c[i] = a[i] - b[i];
    }
    fim = clock();

    printf("sequencial: %ld\n", fim - inicio);
    // checando se o vertor está correto
    if(checkArray(c,size,3.0f,0.01f) != 1) {
        printf("Vetor continha erros de subtração");
    }

    return 0;
}



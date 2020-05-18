#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <pthread.h>
#include <immintrin.h>

//vou definir o numero de filhos a serem criados
#define FILHOS 2
// tamanho default das matrizes array
#define DEFAULT_M_SIZE 8
#define STEP 8

typedef struct thread_data {
    int inicio;
    int fim;
} data;

typedef struct matrix_data {
    int linhas;
    int colunas;
    float **M;
} matrix;

// Global
matrix A;
matrix B;
matrix C;

/***** Funções principais *****/
// Efetua a multiplicação das matrizes preenchendo as linhas [m_inicial,m]
void multmm (int m, int m_inicial, int n, int q, float** A, float** B, float** C);
/*
 função executada por cada thread
 vai chamar a multmm para as linhas que for responsável
 recebe um thread_data como argumento
*/
void *work(void *workerArg);

/***** Funções auxiliares *****/

float* vetcria(int n, float initial_value);

void vetlibera(float* v);

void vetimprime (int n, int* v, char* format);
// Cria a matriz M[i,j] se rand != 0 a matriz eh preenchida por num aletatórios, else 0's
float** matcria (int m, int n, float initial_value);

void matlibera (int m, float** M);

void matimprime (int m, int n, float** A, char* format);


/***** Main *****/
int main(int argc, char* argv[]) {
    
    int pid_filho;
    void* status;
    int i;
    A.linhas = DEFAULT_M_SIZE; 
    int filhos = FILHOS; // numero de threads
    // se não tiver arg vamos usar valores defaut 
    // filhos = 2 | A.linhas = 8
    if (argc > 1) { 
        filhos = atoi(argv[1]);
        if (argc > 2) {
            A.linhas = atoi(argv[2]);
        }
        if (filhos > A.linhas) { // cada thread vai escrever em uma linha 
            filhos = A.linhas;
        }
    }
    
    A.colunas = A.linhas; // vamos usar somente matrizes quadradas
    A.M = matcria(A.linhas,A.colunas, 5.0f);
    B.linhas = A.colunas;
    B.colunas = B.linhas;
    B.M = matcria(B.linhas,B.colunas, 2.0f);

    // C[A_lines][B_columns]
    C.linhas = A.linhas;
    C.colunas = B.colunas;
    C.M = matcria(C.linhas,C.colunas, 0.0f); // C = AxB;

    int passo, inicio = 0;

    passo = A.linhas / filhos;     //passo é o numero de posições que cada filho procura
    int sobra = A.linhas % filhos; //se há sobra, o ultimo filho vai procurar passo+sobra posições

    data thread_data[filhos];
    pthread_t threads[filhos];
    
    for (int i = 0; i < filhos; i++) {        
        if( sobra != 0 && i == filhos-1) {
            passo += sobra; // pra sobra != 0 ultimo pega + sobra linhas
        }        
        thread_data[i].inicio = inicio;
        thread_data[i].fim = inicio + passo;
        pthread_create(&threads[i], NULL, work, (void *)&thread_data[i]);
        inicio += passo;
    }
    for (int i = 0; i < filhos; i++) {
        pthread_join(threads[i], &status);
    }    

    matlibera(A.linhas,A.M);
    matlibera(B.linhas,B.M);
    matlibera(C.linhas,C.M);
    return 0;
}
/***** END Main *****/

float* vetcria(int n, float initial_value) {
	float* novoVetor = NULL;
	int i;
	if(n > 0) {
		novoVetor = (float*)aligned_alloc(32, n*sizeof(float));
		if(novoVetor == NULL) {
			printf("Erro na alocacao do vetor (vetcria) saindo...\n");
			exit(-1);
		}
        __m256 vec = _mm256_set1_ps(initial_value);
		for (i=0; i < n; i+=STEP) {
            _mm256_store_ps(&novoVetor[i], vec);			
		}
	}
	
	return novoVetor;
}

void vetlibera(float* v) {
	free(v);
}

void vetimprime (int n, int* v, char* format) {
	int i;
	for( i = 0; i < n; i++) {
		printf(format,v[i]);
		printf("\n");
	}
}

// m == numero de linhas(i) |||| n == numero de colunas(j)
float** matcria (int m, int n, float initial_value) {
	float** novaMatriz = NULL;
	int i;
    if(n > 0) {
		novaMatriz = (float**)aligned_alloc(32, m*sizeof(float*));
		if(novaMatriz == NULL) {
			printf("Erro na alocacao do vetor de ponteiros (Matcria) saindo...\n");
			exit(-1);
        }
        for (i=0; i < m; i++) {
			novaMatriz[i] = vetcria(n,initial_value);
			if(novaMatriz[i] == NULL) {
				printf("Erro na alocacao do vetor (matcria) saindo...\n");
			exit(-1);
			}
		}
	}
	return novaMatriz;
}

void matlibera (int m, float** A) {
	int i;
	for( i = 0; i < m; i++ ) {
		vetlibera(A[i]);
	}
	free(A);
}


// m = linhas Matriz A, n = colunas matriz A, q = colunas Matriz B
void multmm (int m, int m_inicial, int n, int q, float** A, float** B, float** C) {
	int i,j,k;
	int aux = 0;
    // OBS: matriz quadrada tem m == n == q
    for( i = m_inicial; i < m; i++) { // linhas de A e C
	    for( j = 0; j < n; j++) { // numero de colunas da primeira matriz (A)
            __m256 vec_a = _mm256_set1_ps(A[i][j]); 
			for( k = 0; k < q; k+=STEP) { // numero de colunas da segunda matriz (B)
                __m256 vec_b  = _mm256_load_ps(&B[j][k]);
                __m256 vec_c  = _mm256_load_ps(&C[i][k]);
                __m256 vec_result = _mm256_fmadd_ps(vec_a, vec_b, vec_c);
                /* armazena resultado no vetor em memória */
                _mm256_store_ps(&C[i][k], vec_result);
			}
		}
	}
}

void matimprime (int m, int n, float** A, char* format) {
	int i,j;
	for ( i = 0; i < m; i++ ) {
		for ( j = 0; j < n; j++ ) {
			printf(format, A[i][j]);
			printf(" "); // ultimo elemento vai ser um espaço
		}
		printf("\n");
	}
}

void *work(void *workerArg) {
    data *my_data;
    my_data = (struct thread_data *) workerArg;
    multmm(my_data->fim,my_data->inicio,A.colunas,B.colunas,A.M,B.M,C.M);
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <malloc.h>
#include <math.h>
#include <pthread.h>

//vou definir o numero de filhos a serem criados
#define FILHOS 2
// tamanho default das matrizes array
#define DEFAULT_M_SIZE 2
#define RAND_LOWER_BOUND 0
#define RAND_UPPER_BOUND 10
#define TRUE 1
#define FALSE 0

typedef struct thread_data {
    int inicio;
    int fim;
} data;

typedef struct matrix_data {
    int linhas;
    int colunas;
    int **M;
} matrix;

// Global
matrix A;
matrix B;
matrix C;

/***** Funções principais *****/
// Efetua a multiplicação das matrizes preenchendo as linhas [m_inicial,m]
void multmm (int m, int m_inicial, int n, int q, int** A, int** B, int** C);
/*
 função executada por cada thread
 vai chamar a multmm para as linhas que for responsável
 recebe um thread_data como argumento
*/
void *work(void *workerArg);

/***** Funções auxiliares *****/
// devolve um numero entre os limites
int limitedRand(int min, int max);

int* vetcria(int n, int rand);

void vetlibera(int* v);

void vetimprime (int n, int* v, char* format);
// Cria a matriz M[i,j] se rand != 0 a matriz eh preenchida por num aletatórios, else 0's
int** matcria (int m, int n, int rand);

void matlibera (int m, int** M);

void matimprime (int m, int n, int** A, char* format);


/***** Main *****/
int main(int argc, char* argv[]) {
    
    int pid_filho;
    void* status;
    int i;
    A.linhas = DEFAULT_M_SIZE; 
    int filhos = FILHOS; // numero de threads

    // se não tiver arg vamos usar valores defaut 
    // filhos = 2 | A.linhas = 4
    if (argc > 1) { 
        filhos = atoi(argv[1]);
        if (argc > 2) {
            A.linhas = atoi(argv[2]);
        }
        if (filhos > A.linhas) { // cada thread vai escrever em uma linha 
            filhos = A.linhas;
        }
    }
    if(A.linhas < 4) { // debug seed 
        srand(1); 
    } else {
        srand(time(0)); 
    }
    
    A.colunas = A.linhas; // vamos usar somente matrizes quadradas
    A.M = matcria(A.linhas,A.colunas, TRUE);
    B.linhas = A.colunas;
    B.colunas = B.linhas;
    B.M = matcria(B.linhas,B.colunas, TRUE);

    // C[A_lines][B_columns]
    C.linhas = A.linhas;
    C.colunas = B.colunas;
    C.M = matcria(C.linhas,C.colunas, FALSE); // C = AxB;

    int passo, inicio = 0;

    passo = A.linhas / filhos;     //passo é o numero de posições que cada filho procura
    int sobra = A.linhas % filhos; //se há sobra, o ultimo filho vai procurar passo+sobra posições

    if(A.linhas < 4) {
        printf("****DEBUG****\n");
        printf("passo: %d\nSobra: %d\nthreads: %d\ndim_matrizes: %d\n",passo,sobra,filhos,A.colunas);
    }

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

    if(A.linhas < 4) {
        printf("****DEBUG****\n");
        printf("****MATRIZ A****\n");
        matimprime(A.linhas,A.colunas, A.M, "%3d");
        printf("****MATRIZ B****\n");
        matimprime(B.linhas,B.colunas, B.M, "%3d");
        printf("****MATRIZ RESULTADO****\n");
        matimprime(C.linhas,C.colunas, C.M, "%3d");
    }
    
    matlibera(A.linhas,A.M);
    matlibera(B.linhas,B.M);
    matlibera(C.linhas,C.M);

    return 0;
}
/***** END Main *****/



int limitedRand(int min, int max) 
{   
    return (rand() % (min - max + 1)) + min; 

}

int* vetcria(int n, int rand) {
	int* novoVetor = NULL;
	int i;
	if(n > 0) {
		novoVetor = (int*) malloc(sizeof(int) * n);
		if(novoVetor == NULL) {
			printf("Erro na alocacao do vetor (vetcria) saindo...\n");
			exit(-1);
		}
		for (i=0; i < n; i++) {
            if(rand != FALSE) {
                novoVetor[i] = limitedRand(RAND_LOWER_BOUND,RAND_UPPER_BOUND);
            } else {
                novoVetor[i] = 0;
            }
			
		}
	}
	
	return novoVetor;
}

void vetlibera(int* v) {
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
int** matcria (int m, int n, int rand) {
	int** novaMatriz = NULL;
	int i;
    if(n > 0) {
		novaMatriz = (int**) malloc(sizeof(int*) * m);
		if(novaMatriz == NULL) {
			printf("Erro na alocacao do vetor de ponteiros (Matcria) saindo...\n");
			exit(-1);
        }
        for (i=0; i < m; i++) {
			novaMatriz[i] = vetcria(n,rand);
			if(novaMatriz[i] == NULL) {
				printf("Erro na alocacao do vetor (matcria) saindo...\n");
			exit(-1);
			}
		}
	}
	return novaMatriz;
}

void matlibera (int m, int** A) {
	int i;
	for( i = 0; i < m; i++ ) {
		vetlibera(A[i]);
	}
	free(A);
}


// m = linhas Matriz A, n = colunas matriz A, q = colunas Matriz B
void multmm (int m, int m_inicial, int n, int q, int** A, int** B, int** C) {
	int i,j,k;
	int aux = 0;
	for( j = 0; j < q; j++) { // numero de colunas da segunda matriz (B)
		for( i = m_inicial; i < m; i++) { // linhas de A e C
			for( k = 0; k < n; k++) { // numero de colunas da primeira matriz (A)
				aux += A[i][k] * B[k][j];
			}
			C[i][j] = aux;
			aux = 0;
		}
	}
}

void matimprime (int m, int n, int** A, char* format) {
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
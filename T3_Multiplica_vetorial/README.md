# Multiplicação de matrizes

## Paralelismo com instruções vetoriais

A multiplicação de matrizes é feita linha por linha da matriz resultado, quando mais de uma thread é utilizada, as linhas são divididas entre as threads. Evitando assim uma condição de corrida de escrita entre as threads. Todas as matrizes devem ter dimensões multiplas de 8, as memórias são alocadas alinhadas em 32 bits

### Como é computada cada linha da matriz resultado

1. Computamos a primeira linha da matriz
![Image](./etapa1_multi.png?raw=true)
2. Fazemos o mesmo para segunda linha
![Image](./etapa2_multi.png?raw=true)
3. Repetimos até acabarem as linhas

### Exemplo da representação em uma matriz 3x3 com 3 threads

![Image](./representação_matriz.png?raw=true)

Nesse caso cada thread ficaria responsável pelo cálculo de uma linha

## Resultado

Cpu utilizada: Amd ryzen 7 2700 (8 núcleos e 16 threads)

![Image](./Figure_2_vetorial.png?raw=true)

A título de comparação, temos o resultado com o mesmo tipo de problema sem usar as intruções vetoriais
![Image](./Figure_1.png?raw=true)

Repare que os tempos foram reduzidos para praticamente a metade

## Conclusão

Podemos constatar que o tempo para calcular cada instância é significativamente reduzido ao usarmos mais threads usando instruções vetoriais para calcular o resultado da multiplicação. No entanto quando ultrapassamos o número de cores físicos do processador temos um retorno menor e quando ultrapassamos o numero de threads lógicas da CPU não há ter melhora. Podemos ver que com 32 threads praticamente não houve diferença para 16 threads. Imagino que se continuasse subindo o numero de threads acabaria ocorrendo uma diminuição de desempenho por causa da concorrência excessiva pela CPU.

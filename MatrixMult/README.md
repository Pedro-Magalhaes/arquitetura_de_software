# Multiplicação de matrizes

## Paralelismo

A multiplicação de matrizes é feita linha por linha da matriz resultado, quando mais de uma thread é utilizada, as linhas são divididas entre as threads. Evitando assim uma condição de corrida de escrita entre as threads.

### Exemplo matriz 3x3 com 3 threads

![Image](./representação_matriz.png?raw=true)

Nesse caso cada thread ficaria responsável pelo cálculo de uma linha

## Resultado

Cpu utilizada: Amd ryzen 7 2700 (8 núcleos e 16 threads)

![Image](./Figure_1.png?raw=true)

## Conclusão

Podemos constatar que o tempo para calcular cada instância é significativamente reduzido ao usarmos mais threads para calcular o resultado. No entanto quando ultrapassamos o número de cores físicos do processador temos um retorno menor e quando ultrapassamos o numero de threads lógicas da CPU não há ter melhora. Podemos ver que com 32 threads praticamente não houve diferença para 16 threads. Imagino que se continuasse subindo o numero de threads acabaria ocorrendo uma diminuição de desempenho por causa da concorrência excessiva pela CPU.

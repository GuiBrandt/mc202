# Tarefa 8 - Listas Legais

https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa08.html

## Introdução

David Neto (DN) gosta muito de observar listas de números, o suficiente para apelidar algumas delas como listas legais. Uma lista L é considerada legal por DN se, e somente se, para todo elemento x pertencente a L existirem exatamente x cópias de x. Por exemplo, a lista A = {3,2,1,2,3,3} e B = {} são listas legais, já a lista C = {1,2,2,3,4} não é uma lista legal.

Certo dia, DN recebeu uma lista muito grande de números que volta e meia crescia mais. Porém, ele estava insatisfeito com essa lista e gostaria de saber a menor quantidade de números que deveriam ser retirados da lista para que ela finalmente se tornasse uma lista legal. Como a lista de números pode acabar tendo mais elementos do que DN pode contar, ele pediu a sua ajuda para responder essa questão.

É obrigatório utilizar uma árvore binária balanceada para a resolução desta atividade. Porém, você pode escolher a árvore balanceada que desejar: rubro-negra, AVL, etc.

## Entrada

A primeira linha da entrada consiste de dois inteiros n e k que indicam, respectivamente, a quantidade de números inicialmente na lista de DN e o número de operações que ele realizará. A linha seguinte conterá os n inteiros da lista de DN. Cada uma das k linhas seguintes conterá uma operação que DN deseja realizar. Cada uma dessas operações tem uma das formas a seguir:

- 1 j, indicando que DN escolheu a operação 1 e deseja adicionar o inteiro j na sua lista atual;
- 2 m, indicando que DN escolheu a operação 2 e deseja saber quantas vezes o inteiro m aparece na lista;
- 3, indicando que DN escolheu a operação 3 e deseja saber a menor quantidade de números que devem ser retirados da lista atual para que ela se torne legal.

### Exemplo de entrada

```
5 5
123456789 987654321 1234567890 9987654321 5
2 123456789
2 987654321
2 1234567890
2 9987654321
3
```

### Restrições

Os elementos da lista possuem tamanho de no máximo 10^10

## Saída

Toda vez que as operações ‘2’ e ‘3’ aparecerem, deve-se imprimir um inteiro representando o resultado da operação e, após isso, uma quebra de linha.

### Exemplo de saida

```
1
1
1
1
5
```

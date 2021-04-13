# Tarefa 10 - Corretor

https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa10.html

## Introdução

David Neves (DN) está prestes a defender o seu mestrado e está correndo contra o tempo para entregar a versão final de sua dissertação. Como ele tem virado a noite escrevendo e muitas vezes dormido mal, DN acaba cometendo vários erros de digitação durante a escrita do texto.

DN precisa da sua ajuda para corrigir os erros de digitação, que podem ser esquecer de digitar um caractere, inserir um caracter a mais, ou digitar um caractere trocado.

Sua tarefa é fazer um programa em C com nome `corretor.c` que, dada uma lista de palavras que corresponde a um dicionário, indica o nível de correção de cada palavra do texto. Os níveis são:

- verde: a palavra está correta (isso é, está no dicionário);
- amarelo: a palavra possui exatamente um erro de digitação;
- vermelho: quando a palavra possui pelo menos dois erros de digitação.

## Entrada

A entrada é composta por várias linhas. A primeira linha contém dois inteiros  e  que indicam, respectivamente, o número de palavras no dicionário e o número de palavras do texto. Em seguida são dadas  palavras que compõem o dicionário e  linhas com as palavras que devem ser buscadas.

### Restrições:

- 5 ≤ n ≤ 100
- 3 ≤ q ≤ 100
- Todos os caracteres são minúsculos, as palavras possuem no máximo 25 caracteres e não possuem caracteres especiais.

### Exemplo de entrada

Observe que a palavra `intiera` possui dois caracteres trocados, `i` por `e` e `e` por `i`.

```
5 3
algoritmos
demonstracao
linear
inteira
exato
algoritmoss
intiera
linear
```

## Saída

A saída do seu programa deverá apresentar  linhas, onde a i-ésima linha contém o nível de correção da i-ésima palavra buscada: verde, amarelo ou vermelho.

### Exemplo de saída

```
amarelo
vermelho
verde
```

## Critérios

É obrigatório o uso de tabela de hashing nesta tarefa.

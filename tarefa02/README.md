# Tarefa 2 - Professor Carlos

https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa02.html

## Introdução

Carlos é um professor de Filosofia que adora saber informações de seus alunos, pois ele acha relevante para sua aula. Porém, com o passar dos anos, Carlos tem recebido cada vez mais turmas e as turmas estão cada vez maiores. Por isso, ele precisa da sua ajuda para continuar prestando uma aula de alta qualidade. Para isso, Carlos precisa de um sistema capaz de realizar algumas operações.

As operações que o professor Carlos deseja são:

1. Dado um inteiro j, deve-se retornar o nome do aluno mais novo da turma j.
2. Dado um inteiro j, deve-se retornar o sobrenome do aluno mais velho da turma j.
3. O nome do aluno mais velho dentre todas as turmas.
4. Descobrir o sobrenome do aluno mais novo dentre todas as turmas.
5. Dada uma string s, informar quantos alunos, podendo ser de qualquer turma, possuem s como substring em seu nome. Por exemplo, se s for "los", então o nome "Carlos" possui "los" como substring.
6. Dados j, nome, sobrenome, dia, mês, e ano de nascimento, deve-se adicionar um novo aluno contendo o nome, sobrenome, dia, mês e ano de nascimento, informados previamente, na j-ésima turma. Após adicionado, deve-se retornar quantos alunos a turma j possui no momento.
7. Dado j, deve-se remover o ultimo aluno adicionado na turma j. Após removido, deve-se retornar quantos alunos a turma j possui no momento.

Em caso de empate nos itens 1-4, deve-se informar o aluno com menor nome em ordem lexicográfica e, caso persista o empate, deve-se informar o aluno com menor sobrenome em ordem lexicográfica. Considere que a primeira turma possui índice 0, a segunda possui índice 1 e assim sucessivamente.

Sua tarefa é escrever um programa que, dadas as informações dos alunos e uma sequência de operações que Carlos deseja, realiza cada uma das operações devolvendo as informações solicitadas. Seu programa será dividido em três arquivos:

1. O arquivo de cabeçalho professor_carlos.h fornecido contém a interface de alguns tipos abstratos (TADs) que devem ser utilizados nessa tarefa. Você não precisa alterar esse arquivo.

2. As funções que implementam as operações sobre os TADs devem estar no no arquivo professor_carlos_funcoes.c, assim como outras funções auxiliarem que forem necessárias para implementar essas operações. Não adicione funções nesse arquivo que não sejam necessárias para a implementação dos TADs (e.g., não adicione funções de entrada e saída).

3. O arquivo professor_carlos_main.c deve conter apenas a função main() e funções auxiliares de entrada e saída para essa tarefa, caso seja necessário. Esse arquivo é apenas o “cliente” dos TADs.

Não é permitido a utilização de funções prontas, exceto as funções da biblioteca <stdio.h>.

## Entrada

A primeira linha da entrada consiste de dois inteiros n e k que indicam, respectivamente, o número de turmas que Carlos leciona e o número de operações que ele deseja realizar. Depois disso, para cada uma das  turmas, a entrada possui um inteiro m que indica quantos alunos a turma possui seguido de m linhas com os dados dos alunos. Cada aluno é representado em uma linha contendo nome, sobrenome, dia, mês e ano de nascimento.

Terminada a entrada dos dados dos alunos de todas as turmas, cada uma das próximas k linhas inicia com um inteiro X, indicando a operação escolhida por Carlos, de acordo com a enumeração acima além das outras informações necessárias para realizar a operação X como detalhado anteriormente.

### Exemplo de Entrada

No exemplo a seguir, a primeira turma possui 5 alunos e a segunda turma, 3 alunos.

```
2 3
5
xenia tullez 10 1 2007
roselia valiente 27 7 1988
roselia cotrim 28 12 1986
carina ferreyra 11 9 2016
eudes valiente 12 5 2008
3
virginia linares 16 8 2003
eudes linares 4 10 2007
feliciana bonilla 24 4 2020
1 1
6 1 esmeralda cipriano 25 12 2002
2 1
```

### Restrições
- n ≤ 50
- m ≤ 40
- k ≤ 50
- tamanho de s ≤ 5
- tamanho de nome ≤ 14
- tamanho de sobrenome ≤ 14
- em nenhum momento as turmas possuirão mais que 50 alunos

## Saída
A saída é composta por k linhas, cada uma contendo a resposta de uma das operações, na ordem em que foram descritas na entrada.

### Exemplo de Saida

```
feliciana
4
cipriano
```

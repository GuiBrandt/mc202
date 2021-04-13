# Tarefa 12 - Editor de Planilha

https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa12.html

## Introdução

O aplicativo de edição de planilha é um dos casos de uso mais importantes na história do computador pessoal. Enquanto ele realiza operações muito simples, como as de uma calculadora de mesa, as fórmulas armazenadas em suas células contêm referências a outras células, revelando assim a grande capacidade um computador. Isso permite que mesmo usuários com pouco ou nenhum conhecimento de programação automatizem tarefas e processem grandes volumes de dados.

Enquanto utilizar uma editor de planilhas possa ser simples, implementar um aplicativo de planilhas não é trivial. Como as fórmulas podem se referir a outras células, sempre que uma delas for alterada, é necessário atualizar todas as suas dependentes.

Isso é particularmente importante porque um usuário inexperiente pode acabar criando dependências cíclicas. Essa situação ocorre quando uma célula se refere a si mesma ou a alguma outra, mas essa outra depende da primeira! Nesses casos, é necessário detectar e mostrar o erro.

Para complicar ainda mais, nos aplicativos modernos (Google Sheet, Excel Online etc.), dezenas de usuários podem acessar e modificar planilhas simultaneamente. Por isso, é importante realizar cada operação na planilha tão eficientemente quanto possível. Isso é um problema que ainda hoje não foi solucionado em definitivo! Mas há várias estratégias para amenizar esse problema.

Entre as estratégias mais populares, estão as transformações operacionais. A ideia é simples. No primeiro acesso, toda a planilha é carregada na memória de um servidor. Depois, a cada operação de leitura ou de escrita, esse servidor manipula a planilha e devolve o novo valor da célula afetada.

Nesta tarefa, você deve realizar uma sequência de transformações operacionais.

## Formato da planilha

Nesta tarefa, uma planilha armazena apenas números inteiros. Células que começam com o caractere = correspondem a uma fórmula que consistem de expressões aritiméticas com inteiros, referências a outras células, operações + ou - e parênteses.

Assim como na maioria dos aplicativos de planilha, a referência de uma célula tem o formato `<coluna><linha>`, onde a coluna é uma letra de A até Z e a linha é um número inteiro começando de 1. Por exemplo, na planilha abaixo, a célula A2 contém o valor 4 e a célula B2 contém a fórmula ( A1 - ( B1 + C1 ) ), que se resolve para -4.

|   | A | B | C |
|---|---|---|---|
| 1 | 1	| 2	| 3 |
| 2 | 4	| = ( A1 - ( B1 + C1 ) ) | 6 |
| 3 | = C3 | 8 | = A3 |

As células A3 e C3 se referem uma a outra. Isso configura uma dependência cíclica. Assim, ao serem lidas, deve-se mostrar uma mensagem `#ERRO#`.

## Entrada e Saída

Cada arquivo de entrada `planilha*.in` contém uma sequência de operações e deve ser lido da entrada padrão (usando scanf() e similares). A entrada consiste de múltiplas linhas. A primeira linha contém o caminho relativo para o arquivo csv para ser usado com fopen(), o número de colunas e o número de linhas da planilha. Cada linha subsequente contém uma operação a ser executada na planilha. Há dois tipos de operação:

- Leitura: G <coordenada>: ler uma célula e calcular o valor atual.

- Escrita: S <coordenada> <valor>: atualizar uma célula e calcular o novo valor. É garantido que a célula a ser alterada contém um valor constante, e que o novo valor também será constante.

### Exemplo de CSV
Considere um arquivo de nome testes/planilha0.csv com o seguinte conteúdo:
```
 1 , 2 , 3
 4 ,= ( A1 - ( B1 + C1 ) ) , 6
= C3 , 8 ,= A3
```

### Exemplo de entrada
```
testes/planilha0.csv 3 3
G B2
S C1 10
G B2
G A3
```

### Exemplo de saída
```
B2: -4
C1: 3 -> 10
B2: -11
A3: #ERRO#
```

# Tarefa 5 - Calculadora de números gigantes

https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa05.html

## Introdução

Há muitos anos, Wilson Percival Higgsbury chegou ao Constant depois de ser enganado pelo seu rival Maxwell, como pode ser visto [aqui][wilson]. Wilson é um jovem e talentoso cientista. No início, tudo no Constant era misterioso, mas depois de 936 anos sobrevivendo (pois cada ano lá dentro tem 70 dias), ele já descobriu até as limitações da engine que criou o jogo.

No dia 65537 (depois de ter sobrevivido 65536 dias) Wilson percebeu que o calendário passou a marcar “1” (que seria o dia 0), como dá pra ver nas imagens.

[wilson]: https://www.youtube.com/watch?v=xN712L5NdE4&ab_channel=Don%27tStarve

![wilson-sadboys](https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/img/calculadora_dia.png)

Isso aconteceu porque o inteiro sem sinal que armazena a quantidade de dias tem apenas 16 bits. Assim, quando mais um dia foi somado, o número “deu a volta” e zerou, aparecendo “Dia 1”.

![overflow](https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/img/calculadora_overflow.png)

Agora, Wilson precisa que você o ajude a implementar uma calculadora que consiga realizar operações entre números muito grandes. Os números devem ser inteiros sem sinal e as operações serão soma, subtração e multiplicação. Como o nosso cientista quer desenvolver uma boa ferramenta, é importante começar com um projeto conciso e no futuro aprimorar com novas funcionalidades. Nessa primeira versão, a calculadora deve suportar soma, subtração e multiplicação de numeros inteiros bem grandes, mas a divisão e operações com números racionais só serão implementadas nas próximas versões.

### Critérios

As operações devem ser feitas com números armazenados na forma de lista ligada.

## Entrada

A entrada é composta por várias linhas. A primeira linha contém um inteiro  que indica a quantidade de casos de teste. Em seguida serão dadas  linhas, onde a primeira indicará a operação  a ser realizada e a segunda conterá dois números  e  separados por um espaço.

O código  representa uma das operações a seguir:

- +: soma
- -: subtração
- *: multiplicação
- /: divisão (opcional)

### Restrições
- 3 ≤ n ≤ 110
- 0 ≤ n_1, n_2 ≤ 10^24
- a subtração será sem sinal (resultado em módulo), por exemplo, |1 - 10| = 9
- a divisão será inteira, por exemplo, 7/3 = 2
- não haverá caso de teste com divisão por 0

### Exemplo de entrada
```
4
+
751796058999103512 894118966031226686
-
377631708972605051 46619653822740516
*
541644749181291169 747107029163164920
/
38531094373797137 575993551452481287
```

## Saída
A saída do seu programa deverá apresentar  linhas, onde cada linha conterá o resultado da operação realizada.

### Exemplo de saída
```
1645915025030330198
331012055149864535
404666599422662049824112340086591480
0
```

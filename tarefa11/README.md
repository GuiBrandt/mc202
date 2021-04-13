# Tarefa 11 - Pokemon Teclado & Mouse

https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa11.html

## Introdução

Red é um famoso treinador Pokémon, protagonista da primeira série de jogos lançada em 1996. Em uma realidade paralela, a mãe de Red deixou-o seguir o sonho de ser um mestre Pokémon, desde que ele mantivesse suas obrigações. Uma delas é fazer uma boa faculdade. Por isso, Red escolheu estudar Computação na Unicamp, onde além de uma educação de qualidade, ele teria acesso a muitos ginásios e pokestops.

![lugia](https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/img/caminho_lugia_menor.jpg)

O problema é que agora ele precisa organizar a sua rotina e quer maximizar a sua produtividade. Na sua jornada-para-se-tornar-um-mestre-pokemon, Red quer capturar os Pokémon disponíveis em Raids, aproveitando para pegar os itens das Pokestops no caminho. Atualmente, a prioridade é capturar um Lugia em alguma Raid por perto, mas ele não quer ficar muito tempo caminhando sem jogar.

Red descobriu um mapa cartesiano com todas as pokestops e Lugias da Unicamp! Para cada ponto de interesse, ele conhece as coordenadas x e y em metros a partir da origem do mapa, o Instituto de Computação. Agora, Red precisa de sua ajuda para planejar um caminho que sai da sua posição atual, passa por uma sequência de pokestops e termina em alguma Raid com um Lugia. Red gosta de andar e não se importa com a distância total que caminhará, mas gostaria de encontrar um percurso em que não fique muito tempo caminhando continuamente, sem parar para jogar.

A sua tarefa é encontrar um percurso da posição atual até um Lugia em que a maior distância entre duas paradas seja mínima.

## Critérios

O problema deve ser resolvido usando uma estrutura na forma de grafo.

## Entrada

A primeira linha contém as coordenadas  e  em que Red se encontra. Cada uma das linhas seguintes corresponde às coordenadas  e  de um ponto de interesse, que pode ser um Pokestop ou uma Raid com um Lugia, conforme o exemplo a seguir.

### Exemplo de entrada

```
-0.344 -0.208
8.585 9.022 pokestop
6.573 7.325 pokestop
-0.344 -0.208 pokestop
5.825 8.701 pokestop
0.659 -1.099 pokestop
-0.890 1.296 Lugia
0.789 2.238 pokestop
8.261 7.747 pokestop
0.020 0.388 pokestop
5.783 7.098 pokestop
7.885 8.058 pokestop
0.587 0.139 pokestop
```

## Saída

A saída deve conter o menor número inteiro de metros que limita a distância euclidiana entre duas paradas de um percurso entre a origem e um Lugia.

### Exemplo de saída

```
2
```

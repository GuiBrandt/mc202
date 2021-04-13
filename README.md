# MC202

Implementação dos laboratórios de MC202 no segundo semestre de 2020.

A lista de tarefas e seus enunciados está disponível em
https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/

# Índice

- [Tarefa 1](./tarefa01/README.md): Matriz densa
- [Tarefa 2](./tarefa02/README.md): Array
- [Tarefa 3](./tarefa03/README.md): Array, [algoritmo on-line de variância de Welford][welford]
- [Tarefa 4](./tarefa04/README.md): Matriz densa, backtracking
- [Tarefa 5](./tarefa05/README.md): Bignum (lista ligada) e operações, busca binária (divisão)
- [Tarefa 6](./tarefa06/README.md): Deque
- [Tarefa 7](./tarefa07/README.md): Árvore binária de busca simples
- [Tarefa 8](./tarefa08/README.md): Árvore splay aumentada (multi-set)
- [Tarefa 9](./tarefa09/README.md): Heap binário, [Burst Trie][burst-trie], originalmente usava um [Beap][beap]
- [Tarefa 10](./tarefa10/README.md): Tabela de hashing, hash [FNV-1a][fnv-1a], [BK-tree][bk-tree], distância de levenshtein (DP)
- [Tarefa 11](./tarefa11/README.md): Grafo completo (array dinâmico), Union-Find, algoritmo de Kruskal
- [Tarefa 12](./tarefa12/README.md): Grafo (com "ponteiros"), detecção de ciclos (busca em profundidade), [analisador sintático descendente recursivo][recursive-descent]

# Índice por ED

## Array
- Array: [Tarefa 2](./tarefa02/README.md), [Tarefa 3](./tarefa03/README.md)
- Grafo completo (Array dinâmico): [Tarefa 11](./tarefa11/README.md)
- Matriz densa: [Tarefa 1](./tarefa01/README.md), [Tarefa 4](./tarefa04/README.md)

## Lista ligada
- Bignum (com lista ligada): [Tarefa 5](./tarefa05/README.md)
- Deque (com lista ligada): [Tarefa 6](./tarefa06/README.md)

## Árvores
- Árvore binária de busca simples: [Tarefa 7](./tarefa07/README.md)
- Árvore Splay (multi-set, aumentada): [Tarefa 8](./tarefa08/README.md) (originalmente usava uma [multi-splay][msplay])
- [BK-tree]: [Tarefa 10](./tarefa10/README.md)
- [Burst Trie][burst-trie]: [Tarefa 9](./tarefa09/README.md)
- Heap binário: [Tarefa 9](./tarefa09/README.md) (originalmente usava um [beap][beap])

[msplay]: https://github.com/GuiBrandt/multi-splay
[burst-trie]: http://www.lindstaedt.com.br/estruturas/bursttries.pdf
[beap]: https://en.wikipedia.org/wiki/Beap
[bk-tree]: https://en.wikipedia.org/wiki/BK-tree

## Misc.
- Grafo (com "ponteiros"): [Tarefa 12](./tarefa12/README.md)
- Tabela de hashing: [Tarefa 10](./tarefa10/README.md)
- [Union-Find][union-find]: [Tarefa 11](./tarefa11/README.md)

[union-find]: https://en.wikipedia.org/wiki/Disjoint-set_data_structure

# Índice por Algoritmo

- Algoritmo de Kruskal: [Tarefa 11](./tarefa11/README.md)
- [Algoritmo on-line de variância de Welford][welford]: [Tarefa 3](./tarefa03/README.md)
- [Analisador sintático descendente recursivo][recursive-descent]: [Tarefa 12](./tarefa12/README.md)
- Backtracking: [Tarefa 4](./tarefa04/README.md)
- Busca binária: [Tarefa 5](./tarefa05/README.md)
- Detecção de ciclos (busca em profundidade): [Tarefa 12](./tarefa12/README.md)
- Distância de Levenshtein (DP): [Tarefa 10](./tarefa10/README.md)
- [Hash FNV-1a][fnv-1a]: [Tarefa 10](./tarefa10/README.md)

[welford]: https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford's_online_algorithm
[fnv-1a]: http://isthe.com/chongo/tech/comp/fnv/#FNV-1a
[recursive-descent]: https://en.wikipedia.org/wiki/Recursive_descent_parser

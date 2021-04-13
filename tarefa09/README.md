# Tarefa 9 - Dia de Uber

https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa09.html

## Introdução

Quem tem dificuldade financeira recorre a todo tipo de trabalho, por mais precário que seja. Sobreviver é a prioridade, principalmente em momentos de crise econômica. Nos dias atuais, são vários os trabalhos insalubres mediados por empresas de aplicativos que, ao se distanciarem do trabalhador(a), conseguem super explorá-lo(a) sem precisar garantir as condições mínimas de trabalho. Esse fenômeno é chamado de uberização.

Um exemplo são motoristas de aplicativos. Relatos contam jornadas de até 15 horas diárias para atingir uma modesta meta pessoal. Com a inexistência do vínculo empregatício, motoristas encontram dificuldades em exigir melhores condições de trabalho e precisam recorrer a táticas para lidar com a insegurança diária no trabalho. O repórter Carlos Messias trabalhou como motorista de Uber por um mês e relatou sua experiência na revista Veja São Paulo.

Neste trabalho, iremos simular a sua jornada diária. Mais especificamente, iremos gerenciar as múltiplas solicitações de corrida utilizando a estrutura de dados Heap. Apesar de somente atender a uma solicitação por vez, o seu algoritmo deverá organizar as várias solicitações em espera para minimizar o tempo de espera de um cliente (sempre que possível). Mesmo assim, clientes poderão cancelar suas solicitações. Ao fim do dia, serão apresentadas a Carlos as informações da jornada, incluindo o rendimento líquido obtido.

## Entrada
A entrada consiste de uma sequência de linhas (de tamanho indetermidado) que indicam as ações a serem realizadas no seu código. Cada linha começa com uma letra, representando uma ação diferente:

- A: O motorista aceitou a solicitação de um(a) cliente, deixando-o(a) em espera.
- F: O motorista finalizou a corrida atual.
- C: O(A) cliente indicado(a) cancelou sua solicitação.
- T: O motorista indicou o término da sua jornada e deseja ver o rendimento obtido.

Nas ações do tipo A, a linha conterá também:

- o nome do cliente,
- a avaliação do(a) cliente (i.e. o número de estrelas),
- a posição X e Y (em coordenadas cartesianas) do(a) cliente,
- a posição X e Y (em coordenadas cartesianas) do destino do(a) cliente.

O motorista atende as solicitações de corrida em ordem de avaliação do(a) cliente, da maior para a menor e não há empates. Quando uma ação do tipo F ocorrer, a solicitação de corrida corrente é finalizada e removida da lista de espera. Para calcular a distância entre dois pontos quaisquer neste trabalho, será considerada a distância de Manhattan, sempre medida em kilômetros.

Nas ações do tipo C, a linha conterá também o nome do(a) cliente indicado(a). Nesses momentos, a solicitação correspondente é removida da lista de espera e o motorista recebe uma taxa de compensação.

### Constantes

Todos os dias, o motorista inicia sua jornada em sua casa localizada no ponto (0,0) (i.e., na origem).

Os custos relacionados ao veículo são:

- Custo do aluguel por dia: 57,00 reais
- Consumo: 10 kilômetros por litro
- Custo da gasolina: 4,104 reais por litro

Os valores relacionados à Uber e à modalidade UberX são:

- Taxa da Uber (UberX): 25%
- Taxa de cancelamento: 7 reais
- Pagamento (UberX): 1,40 reais por kilômetro

### Restrições

O nome do programa deverá ser diadeuber.c e as restrições das variáveis são descritas a seguir:

- Nome de um(a) cliente contém até 15 caracteres ASCII sem espaço
- Número de ações é sempre menor que 500

### Exemplo de Entrada

```
A JUCIETE 4.43060231087548 -6 6 -5 -3
A SAVADOR 3.255343682611576 0 -5 3 -5
F
A KETLA 3.356431306756503 6 -7 -2 2
F
A DONOVAN 3.9743695236176784 -2 -4 0 5
A KRIS 3.729076408821234 0 -1 -1 1
F
F
F
T
```

## Saída

A saída consiste de um relatório das ações realizadas durante a jornada. Para ações do tipo A, deve-se atestar a inclusão da sua chamada à fila de espera. Quando ocorrer uma ação F, deve-se imprimir o nome do(a) passageiro(a) cuja corrida foi finalizada. Análogamente, quando um(a) cliente cancelar uma corrida, deve-se também indicar o seu nome. Finalmente, quando o motorista desejar finalizar o seu expediente, deve-se imprimir no relatório as seguintes informações na ordem indicada:

- Quantidade total de kilômetros rodados
- Rendimento bruto (i.e., sem despesas)
- Despesas
- Rendimento líquido (i.e., descontadas as despesas)

### Exemplo de Saida

```
Cliente JUCIETE foi adicionado(a)
Cliente SAVADOR foi adicionado(a)
A corrida de JUCIETE foi finalizada
Cliente KETLA foi adicionado(a)
A corrida de SAVADOR foi finalizada
Cliente DONOVAN foi adicionado(a)
Cliente KRIS foi adicionado(a)
A corrida de KETLA foi finalizada
A corrida de DONOVAN foi finalizada
A corrida de KRIS foi finalizada

Jornada finalizada. Aqui esta o seu rendimento de hoje
Km total: 80
Rendimento bruto: 61.60
Despesas: 89.83
Rendimento liquido: -43.63
```

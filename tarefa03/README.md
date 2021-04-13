# Tarefa 3 - Fake News

https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa03.html

## Introdução

No ano de 2018, o fenômeno das chamadas fake news ganhou força no Brasil e proliferou-se nos anos seguintes. A pandemia do coronavírus mostrou-se um ambiente fértil, pois pouco se sabe sobre a doença e muitas pessoas recorrem à internet desesperadas para encontrar respostas.

Nesta tarefa, você estará a cargo de analisar uma série de termos buscados na internet, para tentar encontrar a mais recente fake news. Para cada termo, há um histórico de relevância que mostra como o interesse por esse termo variou em um certo período. Assim, um termo considerado relevante por muito tempo pode ser um indicativo de fake news, ou de algum evento surpreendente.

Como existem milhões de termos buscados a cada minuto, é imprescindível ter alguma descrição resumida do histórico de relevância de cada um deles. Essa descrição permite comparar os vários termos, oferecendo uma visão global da tendência de pesquisas. Entre as maneiras de descrever resumidamente o histórico de relevância de um termo em um determinado período, está o uso de estatísticas:

- Máximo: Indica quanto o termo foi relevante no momento de maior interesse;
- Mínimo: Indica quanto o termo foi relevante no momento de maior desinteresse;
- Média: Indica a relevância típica do termo considerando todo o período;
- Desvio-Padrão: Indica quão abruptas foram as variações de interesse pelo termo.

Por exemplo, o desvio-padrão indica quão disperso estão os dados em relação à media e um valor de desvio-padrão alto pode ser um bom indicativo de atividade de robôs. Como se sabe, bots são ferramentas muito utilizadas para disseminação de fake news pois, através do falso engajamento, promovem-se os termos específicos para seu fim. Assim, nesta tarefa você deverá avaliar a qual categoria pertencente cada termo:

- Bot: Média >= 60 e desvio-padrão > 15;
- Evento surpreendente: Média >= 60 e desvio-padrão <= 15;
- Evento normal: Média < 60, máximo >= 80 e mínimo > 20;
- Evento local: Média < 60, máximo >= 80 e mínimo <= 20;
- Evento irrelevante: Média < 60 e máximo < 80.

## Critério
O seu programa deverá se chamar fakenews.c. Além disso, nesta tarefa, é vedado todo e qualquer tipo de dado não-primitivo (e.g., structs, vetores, etc.) alocado de maneira estática. Esta tarefa será avaliada automaticamente com o uso da ferramenta Valgrind.

## Dicas
Um termo não possui mais do que 25 caracteres, e sua relevância é medida por porcentagem (i.e [0.0,100.0]). Sugere-se o uso de double para evitar problemas decorrentes de precisão. Além disso, é garantido que sempre haverá pelo menos um termo a ser lido.

# Entrada
A entrada é constituída por várias linhas, como exemplificado abaixo. Na primeira, constam dois números inteiros, N e M, que indicam o número de termos extraídos e a quantidade de dias no histórico, respectivamente. Então, seguem 2N linhas cujas informações se estruturam na seguinte forma: para um termo em uma linha i, o seu histórico de M dias constará na linha seguinte (i.e., i+1).

### Exemplo de entrada

```
16 13
MarielleFranco
39.37846851847752 42.7713666163944 29.569761893653926 30.460029739590173 58.95244961323716 49.676776667379336 54.655638755519284 59.50703650825779 90.04029239451009 72.49467081907603 81.54803701108904 81.48518642704441 81.20660731211395
JamalKhashoggi
65.63422571794575 61.969402302072766 28.879193218139484 30.43416290926318 41.6801378484669 31.810188490994904 51.36553692866911 59.974117396455426 26.418834626615073 64.2482288768939 46.382211009271835 22.773362399138563 62.427375838004
Netflix
48.878682350092696 44.78368373154002 54.563955893880845 64.7122444144054 78.13469788226854 18.41268457980099 25.70360322199901 78.0342826700369 79.54352631729871 39.54564712203303 52.64067250391825 41.72692067934106 67.23821338993672
JadeBarbosa
53.86282179529801 45.72901471852829 68.62930294129958 66.00484093884023 33.71107863483573 59.9907590705559 69.18392020711751 78.89426512483553 56.7707214222573 71.6002701828208 80.01096975833076 28.18668359974744 40.749436245146235
EdSheeran
49.02763684295281 30.877373642921388 73.27909661635366 83.59435486361276 58.36291804623636 61.11480885955934 31.828232135468696 93.95933967533297 87.91585284414109 91.66817981148165 29.216551698914614 30.468153070478923 65.43458181657857
Thanos
41.34706851645633 41.15108587689882 81.45850765417418 73.10845211102516 22.125264012586896 33.61267420248714 44.140050012293244 32.90849229963287 81.62100186241193 17.705730182124483 53.877367670443526 15.868206368500108 41.09641245195334
ReiLeao
38.224849082578174 80.83201609940504 74.05361712975201 76.0639217821535 26.237098856361076 57.816762911150356 60.36328757774285 46.18463298799993 51.14987611696647 65.31065698767534 74.03061015416061 70.95891582445256 77.08007582504509
DeuOnda
61.40961019356536 63.66933729589508 66.44757743857534 92.77001253385019 85.31075982896463 67.39397037196113 70.42434793043509 42.9765391820883 36.02429475735195 48.202295395310614 84.41520759852456 40.52571065365358 64.9877516040233
Anitta
27.30082896243401 50.1853228703274 85.25609747133964 29.605810964621217 22.645752417834732 56.82029471480563 44.94390670387402 30.616551961294583 39.0086117851541 55.42058184337046 78.10486678744525 54.31109888761602 35.24038477620849
RockinRio
29.70501368123857 65.58236001048928 18.252203903536767 42.20675067471663 52.065930508724115 38.30972389462896 30.442807237277304 80.02079001064881 52.550096483774574 34.33892303787865 37.77094618727364 82.35043187663551 10.788527838745708
RodaViva
69.3991260254192 40.56543255218003 67.79628926626106 56.600428953073646 49.946027153434905 84.50944884313637 68.29511196143444 84.35476751227677 77.71631640527559 59.48979636857999 90.01047968337576 55.4484873670782 46.125483931272356
FIFA16
83.43978997320441 43.71024204081357 38.71343167046329 41.16464836357171 57.95532494759723 66.9036742277765 82.57492134007931 84.20962019611804 38.325648710193576 60.073415624886984 52.2108571759032 84.0932610163059 60.18500669224045
JeanineAnez
53.15053114751034 85.86568930372208 90.9483785015327 46.99941071622334 44.6437443462146 69.15073686072029 46.37972014785721 39.485846514263706 77.85949131665714 97.06869950778193 97.54234902353762 51.37116042384015 79.86398187705555
Russia
62.169146078575395 73.98985862052027 41.771628751853754 80.02339592803712 75.35131531855177 45.585942651014264 33.067518999395325 37.93021322911648 50.526752558097 91.66382055479164 47.46616892506467 70.76496171581996 60.38647283393558
Facebook
63.97111214647395 73.96442979666216 70.0046658776831 88.63632544509134 32.863697234404036 25.552815986372803 25.617355720606813 86.26895475295566 43.47914729066585 67.96204509644295 86.09190794934688 92.15665093999212 87.74611462140275
Franca
58.31430820453798 86.30167186926755 44.13655208009486 76.2901054084678 44.2267503237578 54.69984176145037 82.46234452177242 52.24822274917943 62.337908334268704 45.93844607943285 44.616739811628875 24.69410461018673 55.34217956029667
```

## Saida

A saída também contém várias linhas (ilustrado abaixo). Para cada termo, deve-se imprimir as estatísticas calculadas a partir de seu histórico na seguinte ordem: máximo, mínimo, média e desvio-padrão. Após imprimir os dados particulares de cada termo, deve-se imprimir as categorias, e o número de elementos que ela contém, e os termos que as constituem.

### Exemplo de saída

```
MarielleFranco 90.04 29.57 59.37 19.78
JamalKhashoggi 65.63 22.77 45.69 15.55
Netflix 79.54 18.41 53.38 18.89
JadeBarbosa 80.01 28.19 57.95 16.07
EdSheeran 93.96 29.22 60.52 23.66
Thanos 81.62 15.87 44.62 21.44
ReiLeao 80.83 26.24 61.41 16.15
DeuOnda 92.77 36.02 63.43 17.07
Anitta 85.26 22.65 46.88 18.44
RockinRio 82.35 10.79 44.18 20.93
RodaViva 90.01 40.57 65.40 15.07
FIFA16 84.21 38.33 61.04 17.25
JeanineAnez 97.54 39.49 67.72 20.69
Russia 91.66 33.07 59.28 17.43
Facebook 92.16 25.55 64.95 23.91
Franca 86.30 24.69 56.28 16.60

RESULTADO:
Bot (7): EdSheeran ReiLeao DeuOnda RodaViva FIFA16 JeanineAnez Facebook
Surpreendente (0):
Normal (5): MarielleFranco JadeBarbosa Anitta Russia Franca
Local (2): Thanos RockinRio
Irrelevante (2): JamalKhashoggi Netflix
```

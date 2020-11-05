/**
 * @file tokens.h
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Definições do ADT para os tokens do mensageiro.
 * 
 * @see https://www.ic.unicamp.br/~lehilton/mc202ab/tarefas/tarefa07.html
 */

/**
 * @brief Estrutura opaca para a representação interna do saco de cartões
 *        numerados do mensageiro. 
 */
struct saco_internal;

/**
 * @brief Estrutura para o saco de cartões numerados. 
 */
typedef struct {
    struct saco_internal* internal;
} saco;

/**
 * @brief Tamanho máximo para a mensagem de um único cartão antes de quaisquer
 *        trocas.
 */
#define TOKEN_MSG_MAXLEN 5

/**
 * @brief Constrói um saco de cartões vazio.
 * 
 * @return um saco de cartões vazio.
 */
saco make_saco();

/**
 * @brief Adiciona um cartão ao conjunto do mensageiro.
 * 
 * Essa função pode terminar o programa com erro em caso de falta de memória.
 * 
 * @param s ponteiro para o saco de cartões.
 * @param num código numérico do cartão.
 * @param msg texto no verso do cartão.
 */
void add_token(saco* s, int num, const char msg[TOKEN_MSG_MAXLEN + 1]);

/**
 * @brief Executa a operação de troca de tríade de cartões.
 * 
 * Esta função pode terminar o programa com erro em caso de falta de memória.
 * 
 * Esta função assume que o valor de soma passado é válido, i.e., existem três
 * cartões cuja soma é igual ao valor dado. Caso contrário, seu comportamento é
 * indeterminado. 
 * 
 * @param s ponteiro para o saco de cartões.
 * @param soma soma desejada para os cartões.
 */
void troca_triade(saco* s, int soma);

/**
 * @brief Escreve a mensagem obtida ao ler os cartões do saco em ordem
 *        numérica. 
 * 
 * @param s ponteiro para o saco de cartões.
 */
void print_mensagem(const saco* s);

/**
 * @brief Libera todos os recursos alocados para um conjunto de cartões. 
 * 
 * @param s ponteiro para o saco de cartões.
 */
void destroy(saco* s);

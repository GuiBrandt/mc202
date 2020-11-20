/**
 * @file map.h
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Definição do ADT de dicionário.
 */

/**
 * @brief Estrutura opaca de dicionário. 
 */
typedef struct map map;

/**
 * @brief Aloca e inicializa um dicionário vazio.
 * 
 * Em caso de falha, esta função termina o programa com código de erro -1.
 * 
 * @return ponteiro para o dicionário alocado.
 */
map* make_map();

/**
 * @brief Adiciona um nome e um valor associado no dicionário.
 * 
 * Em caso de falha, esta função termina o programa com código de erro -1.
 * 
 * @param m o dicionário.
 * @param name o nome. Deve conter exclusivamente letras maiúsculas e sem
 *             acentos.
 * @param rating o valor associado ao nome.
 */
void map_add(map* m, const char* name, double rating);

/**
 * @brief Obtém o valor associado a um nome no dicionário.
 * 
 * Essa função assume que o nome existe no dicionário. Caso contrário, seu
 * comportamento é indeterminado.
 * 
 * @param m o dicionário.
 * @param name o nome. Deve conter exclusivamente letras maiúsculas e sem
 *             acentos.
 * 
 * @return o valor.
 */
double map_get(map* m, const char* name);

/**
 * @brief Libera os recursos alocados para um dicionário.
 * 
 * @param m o dicionário.
 */
void destroy_map(map* m);

/**
 * @file map.h
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Definição do ADT de dicionário.
 */

/**
 * @brief Tipo de chave usada no mapa. 
 */
typedef const char* mkey_t;

/**
 * @brief Tipo de valor armazenado no mapa. 
 */
typedef int mvalue_t;

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
 * @brief Adiciona um nome e um valor associado no dicionário. Caso o nome já
 *        exista no dicionário, atualiza o valor.
 * 
 * Em caso de falha, esta função termina o programa com código de erro -1.
 * 
 * @param m o dicionário.
 * @param name o nome. Deve conter exclusivamente letras maiúsculas e sem
 *             acentos.
 * @param value o valor associado ao nome.
 */
void map_set(map* m, mkey_t name, mvalue_t value);

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
mvalue_t map_get(const map* m, mkey_t name);

/**
 * @brief Libera os recursos alocados para um dicionário.
 * 
 * @param m o dicionário.
 */
void destroy_map(map* m);

/**
 * @file expressions.h
 * @author Guilherme G. Brandt (235970)
 * 
 * @brief Definição do ADT de expressão de célula da planilha.
 * 
 * Uma expressão inclui tanto valores numéricos simples como expressões
 * complexas envolvendo operações aritméticas e referências a células na
 * planilha.
 */

#ifndef __EXPRESSIONS_H__
#define __EXPRESSIONS_H__

#include <stddef.h>

/**
 * @brief Estrutura de referência a célula na planilha. 
 */
typedef struct reference {
    char col;   // A-Z
    size_t row;
} reference_t;

/**
 * @brief Estrutura opaca de expressão de célula. 
 */
typedef struct expression expression_t;

/**
 * @brief Tipo de função de resolução de dependência.
 * 
 * @param ctx contexto passado para a função eval_expr.
 * @param ref referência a ser resolvida.
 * 
 * @returns uma expressão de célula ou NULL caso a referência seja inválida.
 */
typedef const expression_t* (*resolve_fn_t)(void* ctx, reference_t ref);

/**
 * @brief Constrói uma expressão simples de valor constante.
 * 
 * @param value valor da expressão.
 * 
 * @return uma expressão para a qual eval_expr sempre retorna o valor dado.
 */
expression_t* const_expr(int value);

/**
 * @brief Constrói uma expressão de célula a partir de uma string.
 * 
 * A string deve seguir a seguinte sintaxe (ignorando espaços em branco):
 * 
 * <str>        ::= <integer> | "=" <expr>
 * <expr>       ::= <value> | <arithmetic>
 * <value>      ::= <integer> | <reference> | <parens>
 * <arithmetic> ::= <expr> ("+" | "-") <expr>
 * <integer>    ::= "-"? <number>
 * <reference>  ::= [A-Z] <number>
 * <parens>     ::= "(" <expr> ")"
 * <number>     ::= [0-9]+
 * 
 * @param str a string.
 * 
 * @return a expressão correspondente à string dada.
 */
expression_t* parse_expr(const char* str);

/**
 * @brief Calcula o valor de uma expressão dada uma função de resolução de
 *        referências e um contexto.
 * 
 * Caso a função retorne NULL em uma chamada, a expressão é considerada
 * inválida e retorna INT_MIN.
 * 
 * @param expr a expressão.
 * @param context contexto passado para a função de resolução de referência.
 * @param resolve a função.
 * 
 * @return o valor da expressão dados os valores fornecidos pela função.
 */
int eval_expr(const expression_t* expr, void* context, resolve_fn_t resolve);

/**
 * @brief Enumera as dependências de uma expressão.
 * 
 * O buffer de saída recebe as referências externas contidas na expressão.
 * 
 * @param expr a expressão.
 * @param buf_len tamanho do buffer de saída.
 * @param buf buffer de saída.
 * 
 * @return número de dependências escritas no buffer de saída.
 */
int expr_dependencies(
    const expression_t* expr,
    size_t buf_len,
    reference_t* buf
);

/**
 * @brief Libera os recursos alocados para uma expressão.
 * 
 * @param expr a expressão.
 */
void destroy_expr(expression_t* expr);

#endif // __EXPRESSIONS_H__

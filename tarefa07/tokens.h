struct saco_internal;

typedef struct {
    struct saco_internal* internal;
} saco;

typedef enum {
    TOKEN_SIMPLES,
    TOKEN_COMPOSTO
} tipo_token;

typedef struct {
    int num;
    tipo_token tipo;
    union {
        char msg[6];
        saco componentes;
    };
} token;

void add_token(saco* s, token t);
void troca_triade(saco* s, int soma);
void print_mensagem(saco* s);

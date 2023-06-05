/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
 
typedef enum {false, true} bool;
 
/* Tamanho dos campos dos registros */
/* Campos de tamanho fixo */
#define TAM_DATE 9
#define TAM_DATETIME 13
#define TAM_INT_NUMBER 5
#define TAM_FLOAT_NUMBER 14
#define TAM_ID_CURSO 9
#define TAM_ID_USUARIO 12
#define TAM_TELEFONE 12
#define QTD_MAX_CATEGORIAS 3
 
/* Campos de tamanho variável (tamanho máximo) */
#define TAM_MAX_NOME 45
#define TAM_MAX_TITULO 52
#define TAM_MAX_INSTITUICAO 52
#define TAM_MAX_MINISTRANTE 51
#define TAM_MAX_EMAIL 45
#define TAM_MAX_CATEGORIA 21
 
#define MAX_REGISTROS 1000
#define TAM_REGISTRO_USUARIO (TAM_ID_USUARIO+TAM_MAX_NOME+TAM_MAX_EMAIL+TAM_FLOAT_NUMBER+TAM_TELEFONE)
#define TAM_REGISTRO_CURSO (TAM_ID_CURSO+TAM_MAX_TITULO+TAM_MAX_INSTITUICAO+TAM_MAX_MINISTRANTE+TAM_DATE+TAM_INT_NUMBER+TAM_FLOAT_NUMBER+QTD_MAX_CATEGORIAS*TAM_MAX_CATEGORIA+1)
#define TAM_REGISTRO_INSCRICAO (TAM_ID_CURSO+TAM_ID_USUARIO+TAM_DATETIME+1+TAM_DATETIME-4)
#define TAM_ARQUIVO_USUARIOS (TAM_REGISTRO_USUARIO * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_CURSOS (TAM_REGISTRO_CURSO * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_INSCRICOES (TAM_REGISTRO_INSCRICAO * MAX_REGISTROS + 1)
 
#define TAM_RRN_REGISTRO 4
#define TAM_CHAVE_USUARIOS_IDX (TAM_ID_USUARIO + TAM_RRN_REGISTRO - 1)
#define TAM_CHAVE_CURSOS_IDX (TAM_ID_CURSO + TAM_RRN_REGISTRO - 1)
#define TAM_CHAVE_INSCRICOES_IDX (TAM_ID_USUARIO + TAM_ID_CURSO + TAM_RRN_REGISTRO - 2)
#define TAM_CHAVE_TITULO_IDX (TAM_MAX_TITULO + TAM_ID_CURSO - 2)
#define TAM_CHAVE_DATA_CURSO_USUARIO_IDX (TAM_DATETIME + TAM_ID_CURSO + TAM_ID_USUARIO - 3)
#define TAM_CHAVE_CATEGORIAS_SECUNDARIO_IDX (TAM_MAX_CATEGORIA - 1)
#define TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX (TAM_ID_CURSO - 1)
 
#define TAM_ARQUIVO_USUARIOS_IDX (1000 * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_CURSOS_IDX (1000 * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_INSCRICOES_IDX (1000 * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_TITULO_IDX (1000 * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_DATA_CURSO_USUARIO_IDX (1000 * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_CATEGORIAS_IDX (1000 * MAX_REGISTROS + 1)
 
/* Mensagens padrões */
#define SUCESSO                          "OK\n"
#define RRN_NOS                          "Nos percorridos:"
#define RRN_REGS_PRIMARIOS               "Registros primários percorridos:"
#define RRN_REGS_SECUNDARIOS             "Registros secundários percorridos:"
#define INDICE_CRIADO                    "Indice %s criado com sucesso!\n"
#define AVISO_NENHUM_REGISTRO_ENCONTRADO "AVISO: Nenhum registro encontrado\n"
#define ERRO_OPCAO_INVALIDA              "ERRO: Opcao invalida\n"
#define ERRO_MEMORIA_INSUFICIENTE        "ERRO: Memoria insuficiente\n"
#define ERRO_PK_REPETIDA                 "ERRO: Ja existe um registro com a chave %s\n"
#define ERRO_REGISTRO_NAO_ENCONTRADO     "ERRO: Registro nao encontrado\n"
#define ERRO_SALDO_NAO_SUFICIENTE        "ERRO: Saldo insuficiente\n"
#define ERRO_CATEGORIA_REPETIDA          "ERRO: O curso %s ja possui a categoria %s\n"
#define ERRO_VALOR_INVALIDO              "ERRO: Valor invalido\n"
#define ERRO_ARQUIVO_VAZIO               "ERRO: Arquivo vazio\n"
#define ERRO_NAO_IMPLEMENTADO            "ERRO: Funcao %s nao implementada\n"
 
/* Registro de Usuario */
typedef struct {
    char id_usuario[TAM_ID_USUARIO];
    char nome[TAM_MAX_NOME];
    char email[TAM_MAX_EMAIL];
    char telefone[TAM_TELEFONE];
    double saldo;
} Usuario;
 
/* Registro de Curso */
typedef struct {
    char id_curso[TAM_ID_CURSO];
    char titulo[TAM_MAX_TITULO];
    char instituicao[TAM_MAX_INSTITUICAO];
    char ministrante[TAM_MAX_MINISTRANTE];
    char lancamento[TAM_DATE];
    int carga;
    double valor;
    char categorias[QTD_MAX_CATEGORIAS][TAM_MAX_CATEGORIA];
} Curso;
 
/* Registro de Inscricao */
typedef struct {
    char id_curso[TAM_ID_CURSO];
    char id_usuario[TAM_ID_USUARIO];
    char data_inscricao[TAM_DATETIME];
    char status;
    char data_atualizacao[TAM_DATETIME];        
} Inscricao;
 
 
/*----- Registros dos índices -----*/
 
/* Struct para índice de lista invertida */
typedef struct {
    char* chave;
    int proximo_indice;
} inverted_list_node;
 
/* Struct para um nó de Árvore-B */
typedef struct {
    int this_rrn;
    int qtd_chaves;
    char **chaves; // ponteiro para o começo do campo de chaves no arquivo de índice respectivo
    bool folha;
    int *filhos; // vetor de int para o RRN dos nós filhos (DEVE SER DESALOCADO APÓS O USO!!!)
} btree_node;
 
/* Variáveis globais */
/* Arquivos de dados */
char ARQUIVO_USUARIOS[TAM_ARQUIVO_USUARIOS];
char ARQUIVO_CURSOS[TAM_ARQUIVO_CURSOS];
char ARQUIVO_INSCRICOES[TAM_ARQUIVO_INSCRICOES];
 
/* Ordem das Árvores-B */
int btree_order = 3; // valor padrão
 
/* Índices */
/* Struct para os parâmetros de uma lista invertida */
typedef struct {
    // Ponteiro para o arquivo de índice secundário
    char *arquivo_secundario;
 
    // Ponteiro para o arquivo de índice primário
    char *arquivo_primario;
 
    // Quantidade de registros de índice secundário
    unsigned qtd_registros_secundario;
 
    // Quantidade de registros de índice primário
    unsigned qtd_registros_primario;
 
    // Tamanho de uma chave secundária nesse índice
    unsigned tam_chave_secundaria;
 
    // Tamanho de uma chave primária nesse índice
    unsigned tam_chave_primaria;
 
    // Função utilizada para comparar as chaves do índice secundário.
    // Igual às funções de comparação do bsearch e qsort.
    int (*compar)(const void *key, const void *elem);
} inverted_list;
 
/* Struct para os parâmetros de uma Árvore-B */
typedef struct {
    // RRN da raiz
    int rrn_raiz;
 
    // Ponteiro para o arquivo de índice
    char *arquivo;
 
    // Quantidade de nós no arquivo de índice
    unsigned qtd_nos;
 
    // Tamanho de uma chave nesse índice
    unsigned tam_chave;
 
    // Função utilizada para comparar as chaves do índice.
    // Igual às funções de comparação do bsearch e qsort.
    int (*compar)(const void *key, const void *elem);
} btree;
 
typedef struct {
    char chave_promovida[TAM_CHAVE_TITULO_IDX + 1]; // TAM_CHAVE_TITULO_IDX é a maior chave possível
    int filho_direito;
} promovido_aux;
 
/* Arquivos de índices */
char ARQUIVO_USUARIOS_IDX[TAM_ARQUIVO_USUARIOS_IDX];
char ARQUIVO_CURSOS_IDX[TAM_ARQUIVO_CURSOS_IDX];
char ARQUIVO_INSCRICOES_IDX[TAM_ARQUIVO_INSCRICOES_IDX];
char ARQUIVO_TITULO_IDX[TAM_ARQUIVO_TITULO_IDX];
char ARQUIVO_DATA_CURSO_USUARIO_IDX[TAM_ARQUIVO_DATA_CURSO_USUARIO_IDX];
char ARQUIVO_CATEGORIAS_SECUNDARIO_IDX[TAM_ARQUIVO_CATEGORIAS_IDX];
char ARQUIVO_CATEGORIAS_PRIMARIO_IDX[TAM_ARQUIVO_CATEGORIAS_IDX];
 
/* Comparam a chave (key) com cada elemento do índice (elem).
 * Funções auxiliares para o buscar e inserir chaves em Árvores-B.
 * Note que, desta vez, as funções comparam chaves no formato de strings, não structs.
 * key é a chave do tipo string que está sendo buscada ou inserida. elem é uma chave do tipo string da struct btree_node.
 *
 * Dica: busque sobre as funções strncmp e strnlen, muito provavelmente vai querer utilizá-las no código.
 * */
int order_usuarios_idx(const void *key, const void *elem);
int order_cursos_idx(const void *key, const void *elem);
int order_inscricoes_idx(const void *key, const void *elem);
int order_titulo_idx(const void *key, const void *elem);
int order_data_curso_usuario_idx(const void *key, const void *elem);
int order_categorias_idx(const void *key, const void *elem);
 
btree usuarios_idx = {
    .rrn_raiz = -1,
    .arquivo = ARQUIVO_USUARIOS_IDX,
    .qtd_nos = 0,
    .tam_chave = TAM_CHAVE_USUARIOS_IDX,
    .compar = order_usuarios_idx,
};
 
btree cursos_idx = {
    .rrn_raiz = -1,
    .arquivo = ARQUIVO_CURSOS_IDX,
    .qtd_nos = 0,
    .tam_chave = TAM_CHAVE_CURSOS_IDX,
    .compar = order_cursos_idx,
};
 
btree inscricoes_idx = {
    .rrn_raiz = -1,
    .arquivo = ARQUIVO_INSCRICOES_IDX,
    .qtd_nos = 0,
    .tam_chave = TAM_CHAVE_INSCRICOES_IDX,
    .compar = order_inscricoes_idx,
};
 
btree titulo_idx = {
    .rrn_raiz = -1,
    .arquivo = ARQUIVO_TITULO_IDX,
    .qtd_nos = 0,
    .tam_chave = TAM_CHAVE_TITULO_IDX,
    .compar = order_titulo_idx,
};
 
btree data_curso_usuario_idx = {
    .rrn_raiz = -1,
    .arquivo = ARQUIVO_DATA_CURSO_USUARIO_IDX,
    .qtd_nos = 0,
    .tam_chave = TAM_CHAVE_DATA_CURSO_USUARIO_IDX,
    .compar = order_data_curso_usuario_idx,
};
 
inverted_list categorias_idx = {
    .arquivo_secundario = ARQUIVO_CATEGORIAS_SECUNDARIO_IDX,
    .arquivo_primario = ARQUIVO_CATEGORIAS_PRIMARIO_IDX,
    .qtd_registros_secundario = 0,
    .qtd_registros_primario = 0,
    .tam_chave_secundaria = TAM_CHAVE_CATEGORIAS_SECUNDARIO_IDX,
    .tam_chave_primaria = TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX,
    .compar = order_categorias_idx,
};
 
/* Contadores */
unsigned qtd_registros_usuarios = 0;
unsigned qtd_registros_cursos = 0;
unsigned qtd_registros_inscricoes = 0;
 
/* Funções de geração determinística de números pseudo-aleatórios */
uint64_t prng_seed;
 
void prng_srand(uint64_t value) {
    prng_seed = value;
}
 
uint64_t prng_rand() {
    // https://en.wikipedia.org/wiki/Xorshift#xorshift*
    uint64_t x = prng_seed; // O estado deve ser iniciado com um valor diferente de 0
    x ^= x >> 12; // a
    x ^= x << 25; // b
    x ^= x >> 27; // c
    prng_seed = x;
    return x * UINT64_C(0x2545F4914F6CDD1D);
}
 
/**
 * Gera um <a href="https://en.wikipedia.org/wiki/Universally_unique_identifier">UUID Version-4 Variant-1</a>
 * (<i>string</i> aleatória) de 36 caracteres utilizando o gerador de números pseudo-aleatórios
 * <a href="https://en.wikipedia.org/wiki/Xorshift#xorshift*">xorshift*</a>. O UUID é
 * escrito na <i>string</i> fornecida como parâmetro.<br />
 * <br />
 * Exemplo de uso:<br />
 * <code>
 * char chave_aleatoria[37];<br />
 * new_uuid(chave_aleatoria);<br />
 * printf("chave aleatória: %s&#92;n", chave_aleatoria);<br />
 * </code>
 *
 * @param buffer String de tamanho 37 no qual será escrito
 * o UUID. É terminado pelo caractere <code>\0</code>.
 */
void new_uuid(char buffer[37]) {
    uint64_t r1 = prng_rand();
    uint64_t r2 = prng_rand();
 
    sprintf(buffer, "%08x-%04x-%04lx-%04lx-%012lx", (uint32_t)(r1 >> 32), (uint16_t)(r1 >> 16), 0x4000 | (r1 & 0x0fff), 0x8000 | (r2 & 0x3fff), r2 >> 16);
}
 
/* Funções de manipulação de data */
time_t epoch;
 
#define YEAR0                   1900
#define EPOCH_YR                1970
#define SECS_DAY                (24L * 60L * 60L)
#define LEAPYEAR(year)          (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year)          (LEAPYEAR(year) ? 366 : 365)
 
#define TIME_MAX                2147483647L
 
long _dstbias = 0;                  // Offset for Daylight Saving Time
long _timezone = 0;                 // Difference in seconds between GMT and local time
 
const int _ytab[2][12] = {
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};
 
struct tm *gmtime_r(const time_t *timer, struct tm *tmbuf) {
    // based on http://www.jbox.dk/sanos/source/lib/time.c.html
    time_t time = *timer;
    unsigned long dayclock, dayno;
    int year = EPOCH_YR;
 
    dayclock = (unsigned long) time % SECS_DAY;
    dayno = (unsigned long) time / SECS_DAY;
 
    tmbuf->tm_sec = dayclock % 60;
    tmbuf->tm_min = (dayclock % 3600) / 60;
    tmbuf->tm_hour = dayclock / 3600;
    tmbuf->tm_wday = (dayno + 4) % 7; // Day 0 was a thursday
    while (dayno >= (unsigned long) YEARSIZE(year)) {
        dayno -= YEARSIZE(year);
        year++;
    }
    tmbuf->tm_year = year - YEAR0;
    tmbuf->tm_yday = dayno;
    tmbuf->tm_mon = 0;
    while (dayno >= (unsigned long) _ytab[LEAPYEAR(year)][tmbuf->tm_mon]) {
        dayno -= _ytab[LEAPYEAR(year)][tmbuf->tm_mon];
        tmbuf->tm_mon++;
    }
    tmbuf->tm_mday = dayno + 1;
    tmbuf->tm_isdst = 0;
    return tmbuf;
}
 
time_t mktime(struct tm *tmbuf) {
    // based on http://www.jbox.dk/sanos/source/lib/time.c.html
    long day, year;
    int tm_year;
    int yday, month;
    /*unsigned*/ long seconds;
    int overflow;
    long dst;
 
    tmbuf->tm_min += tmbuf->tm_sec / 60;
    tmbuf->tm_sec %= 60;
    if (tmbuf->tm_sec < 0) {
        tmbuf->tm_sec += 60;
        tmbuf->tm_min--;
    }
    tmbuf->tm_hour += tmbuf->tm_min / 60;
    tmbuf->tm_min = tmbuf->tm_min % 60;
    if (tmbuf->tm_min < 0) {
        tmbuf->tm_min += 60;
        tmbuf->tm_hour--;
    }
    day = tmbuf->tm_hour / 24;
    tmbuf->tm_hour= tmbuf->tm_hour % 24;
    if (tmbuf->tm_hour < 0) {
        tmbuf->tm_hour += 24;
        day--;
    }
    tmbuf->tm_year += tmbuf->tm_mon / 12;
    tmbuf->tm_mon %= 12;
    if (tmbuf->tm_mon < 0) {
        tmbuf->tm_mon += 12;
        tmbuf->tm_year--;
    }
    day += (tmbuf->tm_mday - 1);
    while (day < 0) {
        if(--tmbuf->tm_mon < 0) {
            tmbuf->tm_year--;
            tmbuf->tm_mon = 11;
        }
        day += _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon];
    }
    while (day >= _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon]) {
        day -= _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon];
        if (++(tmbuf->tm_mon) == 12) {
            tmbuf->tm_mon = 0;
            tmbuf->tm_year++;
        }
    }
    tmbuf->tm_mday = day + 1;
    year = EPOCH_YR;
    if (tmbuf->tm_year < year - YEAR0) return (time_t) -1;
    seconds = 0;
    day = 0;                      // Means days since day 0 now
    overflow = 0;
 
    // Assume that when day becomes negative, there will certainly
    // be overflow on seconds.
    // The check for overflow needs not to be done for leapyears
    // divisible by 400.
    // The code only works when year (1970) is not a leapyear.
    tm_year = tmbuf->tm_year + YEAR0;
 
    if (TIME_MAX / 365 < tm_year - year) overflow++;
    day = (tm_year - year) * 365;
    if (TIME_MAX - day < (tm_year - year) / 4 + 1) overflow++;
    day += (tm_year - year) / 4 + ((tm_year % 4) && tm_year % 4 < year % 4);
    day -= (tm_year - year) / 100 + ((tm_year % 100) && tm_year % 100 < year % 100);
    day += (tm_year - year) / 400 + ((tm_year % 400) && tm_year % 400 < year % 400);
 
    yday = month = 0;
    while (month < tmbuf->tm_mon) {
        yday += _ytab[LEAPYEAR(tm_year)][month];
        month++;
    }
    yday += (tmbuf->tm_mday - 1);
    if (day + yday < 0) overflow++;
    day += yday;
 
    tmbuf->tm_yday = yday;
    tmbuf->tm_wday = (day + 4) % 7;               // Day 0 was thursday (4)
 
    seconds = ((tmbuf->tm_hour * 60L) + tmbuf->tm_min) * 60L + tmbuf->tm_sec;
 
    if ((TIME_MAX - seconds) / SECS_DAY < day) overflow++;
    seconds += day * SECS_DAY;
 
    // Now adjust according to timezone and daylight saving time
    if (((_timezone > 0) && (TIME_MAX - _timezone < seconds)) || 
        ((_timezone < 0) && (seconds < -_timezone))) {
        overflow++;
    }
    seconds += _timezone;
 
    if (tmbuf->tm_isdst) {
        dst = _dstbias;
    } else {
        dst = 0;
    }
 
    if (dst > seconds) overflow++;        // dst is always non-negative
    seconds -= dst;
 
    if (overflow) return (time_t) -1;
 
    if ((time_t) seconds != seconds) return (time_t) -1;
    return (time_t) seconds;
}
 
bool set_time(char *date) {
    // http://www.cplusplus.com/reference/ctime/mktime/
    struct tm tm_;
 
    if (strlen(date) == TAM_DATETIME-1 && sscanf(date, "%4d%2d%2d%2d%2d", &tm_.tm_year, &tm_.tm_mon, &tm_.tm_mday, &tm_.tm_hour, &tm_.tm_min) == 5) {
        tm_.tm_year -= 1900;
        tm_.tm_mon -= 1;
        tm_.tm_sec = 0;
        tm_.tm_isdst = -1;
        epoch = mktime(&tm_);
        return true;
    }
    return false;
}
 
void tick_time() {
    epoch += prng_rand() % 864000; // 10 dias
}
 
/**
 * Escreve a <i>data</i> atual no formato <code>AAAAMMDD</code> em uma <i>string</i>
 * fornecida como parâmetro.<br />
 * <br />
 * Exemplo de uso:<br />
 * <code>
 * char timestamp[TAM_DATE];<br />
 * current_date(timestamp);<br />
 * printf("data atual: %s&#92;n", timestamp);<br />
 * </code>
 *
 * @param buffer String de tamanho <code>TAM_DATE</code> no qual será escrita
 * a <i>timestamp</i>. É terminado pelo caractere <code>\0</code>.
 */
void current_date(char buffer[TAM_DATE]) {
    // http://www.cplusplus.com/reference/ctime/strftime/
    // http://www.cplusplus.com/reference/ctime/gmtime/
    // AAAA MM DD
    // %Y   %m %d
    struct tm tm_;
    if (gmtime_r(&epoch, &tm_) != NULL)
        strftime(buffer, TAM_DATE, "%Y%m%d", &tm_);
}
 
/**
 * Escreve a <i>data</i> e a <i>hora</i> atual no formato <code>AAAAMMDDHHMM</code> em uma <i>string</i>
 * fornecida como parâmetro.<br />
 * <br />
 * Exemplo de uso:<br />
 * <code>
 * char timestamp[TAM_DATETIME];<br />
 * current_datetime(timestamp);<br />
 * printf("data e hora atual: %s&#92;n", timestamp);<br />
 * </code>
 *
 * @param buffer String de tamanho <code>TAM_DATETIME</code> no qual será escrita
 * a <i>timestamp</i>. É terminado pelo caractere <code>\0</code>.
 */
void current_datetime(char buffer[TAM_DATETIME]) {
    // http://www.cplusplus.com/reference/ctime/strftime/
    // http://www.cplusplus.com/reference/ctime/gmtime/
    // AAAA MM DD HH MM
    // %Y   %m %d %H %M
    struct tm tm_;
    if (gmtime_r(&epoch, &tm_) != NULL)
        strftime(buffer, TAM_DATETIME, "%Y%m%d%H%M", &tm_);
}
 
/* Remove comentários (--) e caracteres whitespace do começo e fim de uma string */
void clear_input(char *str) {
    char *ptr = str;
    int len = 0;
 
    for (; ptr[len]; ++len) {
        if (strncmp(&ptr[len], "--", 2) == 0) {
            ptr[len] = '\0';
            break;
        }
    }
 
    while(len-1 > 0 && isspace(ptr[len-1]))
        ptr[--len] = '\0';
 
    while(*ptr && isspace(*ptr))
        ++ptr, --len;
 
    memmove(str, ptr, len + 1);
}
 
 
/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */
 
/* Cria o índice respectivo */
void criar_usuarios_idx();
void criar_cursos_idx();
void criar_inscricoes_idx();
void criar_titulo_idx();
void criar_data_curso_usuario_idx();
void criar_categorias_idx();
 
/* Exibe um registro com base no RRN */
bool exibir_usuario(int rrn);
bool exibir_curso(int rrn);
bool exibir_inscricao(int rrn);
 
/* Exibe um registro com base na chave de um btree_node */
bool exibir_btree_usuario(char *chave);
bool exibir_btree_curso(char *chave);
bool exibir_btree_inscricao(char *chave);
bool exibir_btree_titulo(char *chave);
bool exibir_btree_data_curso_usuario(char *chave);
 
/* Recupera do arquivo o registro com o RRN informado
 * e retorna os dados nas structs Usuario, Curso e Inscricao */
Usuario recuperar_registro_usuario(int rrn);
Curso recuperar_registro_curso(int rrn);
Inscricao recuperar_registro_inscricao(int rrn);
 
/* Escreve em seu respectivo arquivo na posição informada (RRN) */
void escrever_registro_usuario(Usuario u, int rrn);
void escrever_registro_curso(Curso j, int rrn);
void escrever_registro_inscricao(Inscricao c, int rrn);
 
/* Funções principais */
void cadastrar_usuario_menu(char* id_usuario, char* nome, char* email, char* telefone);
void cadastrar_telefone_menu(char* id_usuario, char* telefone);
void remover_usuario_menu(char *id_usuario);
void cadastrar_curso_menu(char* titulo, char* instituicao, char* ministrante, char* lancamento, int carga, double valor);
void adicionar_saldo_menu(char* id_usuario, double valor);
void inscrever_menu(char *id_curso, char* id_usuario);
void cadastrar_categoria_menu(char* titulo, char* categoria);
void atualizar_status_inscricoes_menu(char* id_usuario, char* titulo, char status);
 
/* Busca */
void buscar_usuario_id_menu(char *id_usuario);
void buscar_curso_id_menu(char *id_curso);
void buscar_curso_titulo_menu(char *titulo);
 
/* Listagem */
void listar_usuarios_id_menu();
void listar_cursos_categorias_menu(char *categoria);
void listar_inscricoes_periodo_menu(char *data_inicio, char *data_fim);
 
/* Liberar espaço */
void liberar_espaco_menu();
 
/* Imprimir arquivos de dados */
void imprimir_arquivo_usuarios_menu();
void imprimir_arquivo_cursos_menu();
void imprimir_arquivo_inscricoes_menu();
 
/* Imprimir índices primários */
void imprimir_usuarios_idx_menu();
void imprimir_cursos_idx_menu();
void imprimir_inscricoes_idx_menu();
 
/* Imprimir índices secundários */
void imprimir_titulo_idx_menu();
void imprimir_data_curso_usuario_idx_menu();
void imprimir_categorias_secundario_idx_menu();
void imprimir_categorias_primario_idx_menu();
 
/* Funções de manipulação de Lista Invertida */
/**
 * Responsável por inserir duas chaves (chave_secundaria e chave_primaria) em uma Lista Invertida (t).<br />
 * Atualiza os parâmetros dos índices primário e secundário conforme necessário.<br />
 * As chaves a serem inseridas devem estar no formato correto e com tamanho t->tam_chave_primario e t->tam_chave_secundario.<br />
 * O funcionamento deve ser genérico para qualquer Lista Invertida, adaptando-se para os diferentes parâmetros presentes em seus structs.<br />
 *
 * @param chave_secundaria Chave a ser buscada (caso exista) ou inserida (caso não exista) no registro secundário da Lista Invertida.
 * @param chave_primaria Chave a ser inserida no registro primário da Lista Invertida.
 * @param t Ponteiro para a Lista Invertida na qual serão inseridas as chaves.
 */
void inverted_list_insert(char *chave_secundaria, char *chave_primaria, inverted_list *t);
 
/**
 * Responsável por buscar uma chave no índice secundário de uma Lista invertida (T). O valor de retorno indica se a chave foi encontrada ou não.
 * O ponteiro para o int result pode ser fornecido opcionalmente, e conterá o índice inicial das chaves no registro primário.<br />
 * <br />
 * Exemplos de uso:<br />
 * <code>
 * // Exemplo 1. A chave encontrada deverá ser retornada e o caminho não deve ser informado.<br />
 * ...<br />
 * int result;<br />
 * bool found = inverted_list_secondary_search(&result, false, categoria, &categorias_idx);<br />
 * ...<br />
 * <br />
 * // Exemplo 2. Não há interesse na chave encontrada, apenas se ela existe, e o caminho não deve ser informado.<br />
 * ...<br />
 * bool found = inverted_list_secondary_search(NULL, false, categoria, &categorias_idx);<br />
 * ...<br />
 * <br />
 * // Exemplo 3. Há interesse no caminho feito para encontrar a chave.<br />
 * ...<br />
 * int result;<br />
 * bool found = inverted_list_secondary_search(&result, true, categoria, &categorias_idx);<br />
 * </code>
 *
 * @param result Ponteiro para ser escrito o índice inicial (primeira ocorrência) das chaves do registro primário. É ignorado caso NULL.
 * @param exibir_caminho Indica se o caminho percorrido deve ser impresso.
 * @param chave_secundaria Chave a ser buscada.
 * @param t Ponteiro para o índice do tipo Lista invertida no qual será buscada a chave.
 * @return Indica se a chave foi encontrada.
 */
bool inverted_list_secondary_search(int *result, bool exibir_caminho, char *chave_secundaria, inverted_list *t);
 
/**
 * Responsável por percorrer o índice primário de uma Lista invertida (T). O valor de retorno indica a quantidade de chaves encontradas.
 * O ponteiro para o vetor de strings result pode ser fornecido opcionalmente, e será populado com a lista de todas as chaves encontradas.
 * O ponteiro para o inteiro indice_final também pode ser fornecido opcionalmente, e deve conter o índice do último campo da lista encadeada 
 * da chave primaria fornecida (isso é útil na inserção de um novo registro).<br />
 * <br />
 * Exemplos de uso:<br />
 * <code>
 * // Exemplo 1. As chaves encontradas deverão ser retornadas e tanto o caminho quanto o indice_final não devem ser informados.<br />
 * ...<br />
 * char chaves[TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX][MAX_REGISTROS];<br />
 * int qtd = inverted_list_primary_search(chaves, false, indice, NULL, &categorias_idx);<br />
 * ...<br />
 * <br />
 * // Exemplo 2. Não há interesse nas chaves encontradas, apenas no indice_final, e o caminho não deve ser informado.<br />
 * ...<br />
 * int indice_final;
 * int qtd = inverted_list_primary_search(NULL, false, indice, &indice_final, &categorias_idx);<br />
 * ...<br />
 * <br />
 * // Exemplo 3. Há interesse nas chaves encontradas e no caminho feito.<br />
 * ...<br />
 * char chaves[TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX][MAX_REGISTROS];<br />
 * int qtd = inverted_list_primary_search(chaves, true, indice, NULL, &categorias_idx);<br />
 * ...<br />
 * <br />
 * </code>
 *
 * @param result Ponteiro para serem escritas as chaves encontradas. É ignorado caso NULL.
 * @param exibir_caminho Indica se o caminho percorrido deve ser impresso.
 * @param indice Índice do primeiro registro da lista encadeada a ser procurado.
 * @param indice_final Ponteiro para ser escrito o índice do último registro encontrado (cujo campo indice é -1). É ignorado caso NULL.
 * @param t Ponteiro para o índice do tipo Lista invertida no qual será buscada a chave.
 * @return Indica a quantidade de chaves encontradas.
 */
int inverted_list_primary_search(char result[][TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX], bool exibir_caminho, int indice, int *indice_final, inverted_list *t);
 
/**
 * Responsável por buscar uma chave (k) dentre os registros secundários de uma Lista Invertida de forma eficiente.<br />
 * O valor de retorno deve indicar se a chave foi encontrada ou não.
 * O ponteiro para o int result pode ser fornecido opcionalmente, e conterá o índice no registro secundário da chave encontrada.<br />
 *
 * @param result Ponteiro para ser escrito o índice nos registros secundários da chave encontrada. É ignorado caso NULL.
 * @param exibir_caminho Indica se o caminho percorrido deve ser impresso.
 * @param chave Chave a ser buscada na Lista Invertida.
 * @param t Ponteiro para o índice da Lista Invertida no qual será buscada a chave.
 * @return Indica se a chave foi encontrada.
 */
bool inverted_list_binary_search(int* result, bool exibir_caminho, char *chave, inverted_list *t);
 
/* Funções de manipulação de Árvores-B */
/**
 * Responsável por inserir uma chave (k) em uma Árvore-B (T). Atualiza os parâmetros da Árvore-B conforme necessário.<br />
 * A chave a ser inserida deve estar no formato correto e com tamanho t->tam_chave.<br />
 * O funcionamento deve ser genérico para qualquer Árvore-B, considerando que os únicos parâmetros que se alteram entre
 * as árvores é o t->tam_chave.<br />
 * <br />
 * Exemplo de uso:<br />
 * <code>
 * ...<br />
 * char usuario_str[TAM_CHAVE_USUARIOS_IDX + 1];<br />
 * sprintf(usuario_str, "%s%04d", id_usuario, rrn_usuario);<br />
 * btree_insert(usuario_str, &usuarios_idx);<br />
 * ...<br />
 * </code>
 *
 * @param chave Chave a ser inserida na Árvore-B.
 * @param t Ponteiro para o índice do tipo Árvore-B no qual será inserida a chave.
 */
void btree_insert(char *chave, btree *t);
 
/**
 * Função auxiliar de inserção de uma chave (k) em uma Árvore-B (T). Atualiza os parâmetros da Árvore-B conforme necessário.<br />
 * Esta é uma função recursiva. Ela recebe o RRN do nó a ser trabalhado sobre.<br />
 *
 * @param chave Chave a ser inserida na Árvore-B.
 * @param rrn RRN do nó no qual deverá ser processado.
 * @param t Ponteiro para o índice do tipo Árvore-B no qual será inserida a chave.
 * @return Retorna uma struct do tipo promovido_aux que contém a chave promovida e o RRN do filho direito.
 */
promovido_aux btree_insert_aux(char *chave, int rrn, btree *t);
 
/**
 * Função auxiliar para dividir um nó de uma Árvore-B (T). Atualiza os parâmetros conforme necessário.<br />
 *
 * @param promo Uma struct do tipo promovido_aux que contém a chave a ser inserida e o RRN do seu filho direito.
 * @param node Ponteiro para nó que deve ser dividido. 
 * @param i O índice da posição onde a chave a ser inserida deve estar.
 * @param t Ponteiro para o índice do tipo Árvore-B no qual será inserida a chave.
 * @return Retorna uma struct do tipo promovido_aux que contém a chave promovida e o RRN do filho direito.
 */
promovido_aux btree_divide(promovido_aux promo, btree_node *node, int i, btree *t);
 
/**
 * Responsável por remover uma chave (k) de uma Árvore-B (T). Atualiza os parâmetros da Árvore-B conforme necessário.<br />
 * A chave a ser removida deve estar no formato correto e com tamanho t->tam_chave.<br />
 * O funcionamento deve ser genérico para qualquer Árvore-B, considerando que os únicos parâmetros que se alteram entre
 * as árvores é o t->tam_chave.<br />
 * <br />
 * Exemplo de uso:<br />
 * <code>
 * ...<br />
 * char usuario_str[TAM_CHAVE_USUARIOS_IDX + 1];<br />
 * sprintf(usuario_str, "%s%04d", id_usuario, rrn_usuario);<br />
 * btree_delete(usuario_str, &usuarios_idx);<br />
 * ...<br />
 * </code>
 *
 * @param chave Chave a ser removida da Árvore-B.
 * @param t Ponteiro para o índice do tipo Árvore-B do qual será removida a chave.
 */
void btree_delete(char *chave, btree *t);
 
/**
 * Função auxiliar de remoção de uma chave (k) de uma Árvore-B (T). Atualiza os parâmetros da Árvore-B conforme necessário.<br />
 * Esta é uma função recursiva. Ela recebe o RRN do nó a ser trabalhado sobre.<br />
 *
 * @param chave Chave a ser removida da Árvore-B.
 * @param rrn RRN do nó no qual deverá ser processado.
 * @param t Ponteiro para o índice do tipo Árvore-B do qual será removida a chave.
 * @return Indica se a remoção deixou o nó que foi processado com menos chaves que o mínimo necessário.
 */
bool btree_delete_aux(char *chave, int rrn, btree *t);
 
/**
 * Função auxiliar para redistribuir ou concatenar nós irmãos adjacentes à esquerda e à direita de um nó pai em uma Árvore-B (T). 
 * Atualiza os parâmetros conforme necessário.<br />
 *
 * @param node Ponteiro para nó pai dos nós irmãos adjacentes que deve ser redistribuidos ou concatenados. 
 * @param i O índice da posição no nó pai onde se encontra a chave separadora dos nós irmãos adjacentes.
 * @param t Ponteiro para o índice do tipo Árvore-B no qual serão redistribuídos ou concatenados os nós irmãos adjacentes.
 * @return Indica se a redistribuição ou concatenação deixou o nó pai com menos chaves que o mínimo necessário.
 */
bool btree_borrow_or_merge(btree_node *node, int i, btree *t);
 
/**
 * Responsável por buscar uma chave (k) em uma Árvore-B (T). O valor de retorno indica se a chave foi encontrada ou não.
 * O ponteiro para a string result pode ser fornecido opcionalmente, e conterá o resultado encontrado.<br />
 * Esta é uma função recursiva. O parâmetro rrn recebe a raíz da Árvore-B na primeira chamada, e nas chamadas
 * subsequentes é o RRN do filho de acordo com o algoritmo fornecido.<br />
 * Comportamento de acordo com as especificações do PDF sobre Árvores-B e suas operações.<br />
 * <br />
 * Exemplos de uso:<br />
 * <code>
 * // Exemplo 1. A chave encontrada deverá ser retornada e o caminho não deve ser informado.<br />
 * ...<br />
 * char result[TAM_CHAVE_USUARIOS_IDX + 1];<br />
 * bool found = btree_search(result, false, id_usuario, usuarios_idx.rrn_raiz, &usuarios_idx);<br />
 * ...<br />
 * <br />
 * // Exemplo 2. Não há interesse na chave encontrada, apenas se ela existe, e o caminho não deve ser informado.<br />
 * ...<br />
 * bool found = btree_search(NULL, false, id_usuario, usuarios_idx.rrn_raiz, &usuarios_idx);<br />
 * ...<br />
 * <br />
 * // Exemplo 3. Busca por uma chave de tamanho variável (específico para o caso de buscas por chaves PIX).<br />
 * ...<br />
 * char titulo_str[TAM_MAX_TITULO];<br />
 * strcpy(titulo_str, titulo);<br />
 * strpadright(titulo_str, '#', TAM_MAX_TITULO - 1);<br />
 * bool found = btree_search(NULL, false, titulo_str, titulo_idx.rrn_raiz, &titulo_idx);<br />
 * ...<br />
 * <br />
* // Exemplo 4. Há interesse no caminho feito para encontrar a chave.<br />
  * ...<br />
 * char result[TAM_CHAVE_USUARIOS_IDX + 1];<br />
 * printf(RRN_NOS);<br />
 * bool found = btree_search(result, true, id_usuario, usuarios_idx.rrn_raiz, &usuarios_idx);<br />
 * printf("\n");<br />
 * </code>
 *
 * @param result Ponteiro para ser escrita a chave encontrada. É ignorado caso NULL.
 * @param exibir_caminho Indica se o caminho percorrido deve ser impresso.
 * @param chave Chave a ser buscada na Árvore-B.
 * @param rrn RRN do nó no qual deverá ser processado. É o RRN da raíz da Árvore-B caso seja a primeira chamada.
 * @param t Ponteiro para o índice do tipo Árvore-B no qual será buscada a chave.
 * @return Indica se a chave foi encontrada.
 */
bool btree_search(char *result, bool exibir_caminho, char *chave, int rrn, btree *t);
 
/**
 * Responsável por buscar uma chave (k) dentro do nó de uma Árvore-B (T) de forma eficiente. O valor de retorno indica se a chave foi encontrada ou não.
 * O ponteiro para o int result pode ser fornecido opcionalmente, e indica o índice da chave encontrada (caso tenha sido encontrada) 
 * ou o índice do filho onde esta chave deve estar (caso não tenha sido encontrada).<br />
 *
 * @param result Ponteiro para ser escrito o índice da chave encontrada ou do filho onde ela deve estar. É ignorado caso NULL.
 * @param exibir_caminho Indica se o caminho percorrido deve ser impresso.
 * @param chave Chave a ser buscada na Árvore-B.
 * @param node Ponteiro para o nó onde a busca deve ser feita.
 * @param t Ponteiro para o índice do tipo Árvore-B no qual será buscada a chave.
 * @return Indica se a chave foi encontrada.
 */
bool btree_binary_search(int *result, bool exibir_caminho, char* chave, btree_node* node, btree* t);
 
/**
 * Função para percorrer uma Árvore-B (T) em ordem.<br />
 * Os parâmetros chave_inicio e chave_fim podem ser fornecidos opcionalmente, e contém o intervalo do percurso.
 * Caso chave_inicio e chave_fim sejam NULL, o índice inteiro é percorrido.
 * Esta é uma função recursiva. O parâmetro rrn recebe a raíz da Árvore-B na primeira chamada, e nas chamadas
 * subsequentes é o RRN do filho de acordo com o algoritmo de percursão em ordem.<br />
 * <br />
 * Exemplo de uso:<br />
 * <code>
 * // Exemplo 1. Intervalo não especificado.
 * ...<br />
 * bool imprimiu = btree_print_in_order(NULL, NULL, exibir_btree_usuario, usuarios_idx.rrn_raiz, &usuarios_idx);
 * ...<br />
 * <br />
 * // Exemplo 2. Imprime as transações contidas no intervalo especificado.
 * ...<br />
 * bool imprimiu = btree_print_in_order(data_inicio, data_fim, exibir_btree_data_curso_usuario, data_curso_usuario_idx.rrn_raiz, &data_curso_usuario_idx);
 * ...<br />
 * </code>
 *
 * @param chave_inicio Começo do intervalo. É ignorado caso NULL.
 * @param chave_fim Fim do intervalo. É ignorado caso NULL.
 * @param exibir Função utilizada para imprimir uma chave no índice. É informada a chave para a função.
 * @param rrn RRN do nó no qual deverá ser processado.
 * @param t Ponteiro para o índice do tipo Árvore-B no qual será inserida a chave.
 * @return Indica se alguma chave foi impressa.
 */
bool btree_print_in_order(char *chave_inicio, char *chave_fim, bool (*exibir)(char *chave), int rrn, btree *t);
 
/**
 * Função interna para ler um nó em uma Árvore-B (T).<br />
 *
 * @param no No a ser lido da Árvore-B.
 * @param t Árvore-B na qual será feita a leitura do nó.
 */
btree_node btree_read(int rrn, btree *t);
 
/**
 * Função interna para escrever um nó em uma Árvore-B (T).<br />
 *
 * @param no No a ser escrito na Árvore-B.
 * @param t Árvore-B na qual será feita a escrita do nó.
 */
void btree_write(btree_node no, btree *t);
 
/**
 * Função interna para alocar o espaço necessário dos campos chaves (vetor de strings) e filhos (vetor de inteiros) da struct btree_node.<br />
 *
 * @param t Árvore-B base para o qual será alocado um struct btree_node.
 */
btree_node btree_node_malloc(btree *t);
 
/**
 * Função interna para liberar o espaço alocado dos campos chaves (vetor de strings) e filhos (vetor de inteiros) da struct btree_node.<br />
 *
 * @param t Árvore-B base para o qual será liberado o espaço alocado para um struct btree_node.
 */
void btree_node_free(btree_node no);
 
/**
 * Função interna para calcular o tamanho em bytes de uma Árvore-B.<br />
 *
 * @param t Árvore-B base para o qual será calculado o tamanho.
 */
int btree_register_size(btree *t);
 
/**
 * Preenche uma string str com o caractere pad para completar o tamanho size.<br />
 *
 * @param str Ponteiro para a string a ser manipulada.
 * @param pad Caractere utilizado para fazer o preenchimento à direita.
 * @param size Tamanho desejado para a string.
 */
char* strpadright(char *str, char pad, unsigned size);
 
/**
 * Converte uma string str para letras maiúsculas.<br />
 *
 * @param str Ponteiro para a string a ser manipulada.
 */
char* strupr(char *str);
 
/**
 * Converte uma string str para letras minúsculas.<br />
 *
 * @param str Ponteiro para a string a ser manipulada.
 */
char* strlower(char *str);
 
/* <<< COLOQUE AQUI OS DEMAIS PROTÓTIPOS DE FUNÇÕES, SE NECESSÁRIO >>> */
void btree_insert_node_key(promovido_aux promo, btree_node *no, btree *t);

 
/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
 
int main() {
    // variáveis utilizadas pelo interpretador de comandos
    char input[500];
    uint64_t seed = 2;
    char datetime[TAM_DATETIME] = "202103181430"; // UTC 18/03/2021 14:30:00
    char id_usuario[TAM_ID_USUARIO];
    char nome[TAM_MAX_NOME];
    char email[TAM_MAX_EMAIL];
    char telefone[TAM_TELEFONE];
    char id_curso[TAM_ID_CURSO];
    char titulo[TAM_MAX_TITULO];
    char instituicao[TAM_MAX_INSTITUICAO];
    char ministrante[TAM_MAX_MINISTRANTE];
    char lancamento[TAM_DATE];
    char categoria[TAM_MAX_CATEGORIA];
    int carga;
    double valor;
    char data_inicio[TAM_DATETIME];
    char data_fim[TAM_DATETIME];
    char status;
 
    scanf("SET BTREE_ORDER %d;\n", &btree_order);
 
    scanf("SET ARQUIVO_USUARIOS TO '%[^']';\n", ARQUIVO_USUARIOS);
    int temp_len = strlen(ARQUIVO_USUARIOS);
    qtd_registros_usuarios = temp_len / TAM_REGISTRO_USUARIO;
    ARQUIVO_USUARIOS[temp_len] = '\0';
 
    scanf("SET ARQUIVO_CURSOS TO '%[^']';\n", ARQUIVO_CURSOS);
    temp_len = strlen(ARQUIVO_CURSOS);
    qtd_registros_cursos = temp_len / TAM_REGISTRO_CURSO;
    ARQUIVO_CURSOS[temp_len] = '\0';
 
    scanf("SET ARQUIVO_INSCRICOES TO '%[^']';\n", ARQUIVO_INSCRICOES);
    temp_len = strlen(ARQUIVO_INSCRICOES);
    qtd_registros_inscricoes = temp_len / TAM_REGISTRO_INSCRICAO;
    ARQUIVO_INSCRICOES[temp_len] = '\0';
 
    // inicialização do gerador de números aleatórios e função de datas
    prng_srand(seed);
    putenv("TZ=UTC");
    set_time(datetime);
 
    criar_usuarios_idx();
    criar_cursos_idx();
    criar_inscricoes_idx();
    criar_titulo_idx();
    criar_data_curso_usuario_idx();
    criar_categorias_idx();
 
    while (1) {
        fgets(input, 500, stdin);
        printf("%s", input);
        clear_input(input);
 
        if (strcmp("", input) == 0)
            continue; // não avança o tempo nem imprime o comando este seja em branco
 
        /* Funções principais */
        if (sscanf(input, "INSERT INTO usuarios VALUES ('%[^']', '%[^']', '%[^']', '%[^']');", id_usuario, nome, email, telefone) == 4)
            cadastrar_usuario_menu(id_usuario, nome, email, telefone);
        else if (sscanf(input, "INSERT INTO usuarios VALUES ('%[^']', '%[^']', '%[^']');", id_usuario, nome, email) == 3)
            { strcpy(telefone, ""); strpadright(telefone, '*', TAM_TELEFONE-1); cadastrar_usuario_menu(id_usuario, nome, email, telefone); }
        else if (sscanf(input, "UPDATE usuarios SET telefone = '%[^']' WHERE id_usuario = '%[^']';", telefone, id_usuario) == 2)
            cadastrar_telefone_menu(id_usuario, telefone);
        else if (sscanf(input, "DELETE FROM usuarios WHERE id_usuario = '%[^']';", id_usuario) == 1)
            remover_usuario_menu(id_usuario);
        else if (sscanf(input, "INSERT INTO cursos VALUES ('%[^']', '%[^']', '%[^']', '%[^']', %d, %lf);", titulo, instituicao, ministrante, lancamento, &carga, &valor) == 6)
            cadastrar_curso_menu(titulo, instituicao, ministrante, lancamento, carga, valor);
        else if (sscanf(input, "UPDATE usuarios SET saldo = saldo + %lf WHERE id_usuario = '%[^']';", &valor, id_usuario) == 2)
            adicionar_saldo_menu(id_usuario, valor);
        else if (sscanf(input, "INSERT INTO inscricoes VALUES ('%[^']', '%[^']');", id_curso, id_usuario) == 2)
            inscrever_menu(id_curso, id_usuario);
        else if (sscanf(input, "UPDATE cursos SET categorias = array_append(categorias, '%[^']') WHERE titulo = '%[^']';", categoria, titulo) == 2)
            cadastrar_categoria_menu(titulo, categoria);
        else if (sscanf(input, "UPDATE inscricoes SET status = '%c' WHERE id_curso = (SELECT id_curso FROM cursos WHERE titulo = '%[^']') AND id_usuario = '%[^']';", &status, titulo, id_usuario) == 3)
            atualizar_status_inscricoes_menu(id_usuario, titulo, status);
 
        /* Busca */
        else if (sscanf(input, "SELECT * FROM usuarios WHERE id_usuario = '%[^']';", id_usuario) == 1)
            buscar_usuario_id_menu(id_usuario);
        else if (sscanf(input, "SELECT * FROM cursos WHERE id_curso = '%[^']';", id_curso) == 1)
            buscar_curso_id_menu(id_curso);
        else if (sscanf(input, "SELECT * FROM cursos WHERE titulo = '%[^']';", titulo) == 1)
            buscar_curso_titulo_menu(titulo);
 
        /* Listagem */
        else if (strcmp("SELECT * FROM usuarios ORDER BY id_usuario ASC;", input) == 0)
            listar_usuarios_id_menu();
        else if (sscanf(input, "SELECT * FROM cursos WHERE '%[^']' = ANY (categorias) ORDER BY id_curso ASC;", categoria) == 1)
            listar_cursos_categorias_menu(categoria);
        else if (sscanf(input, "SELECT * FROM inscricoes WHERE data_inscricao BETWEEN '%[^']' AND '%[^']' ORDER BY data_inscricao ASC;", data_inicio, data_fim) == 2)
            listar_inscricoes_periodo_menu(data_inicio, data_fim);
 
        /* Liberar espaço */
        else if (strcmp("VACUUM usuarios;", input) == 0)
            liberar_espaco_menu();
 
        /* Imprimir arquivos de dados */
        else if (strcmp("\\echo file ARQUIVO_USUARIOS", input) == 0)
            imprimir_arquivo_usuarios_menu();
        else if (strcmp("\\echo file ARQUIVO_CURSOS", input) == 0)
            imprimir_arquivo_cursos_menu();
        else if (strcmp("\\echo file ARQUIVO_INSCRICOES", input) == 0)
            imprimir_arquivo_inscricoes_menu();
        
        /* Imprimir índices primários */
        else if (strcmp("\\echo index usuarios_idx", input) == 0)
            imprimir_usuarios_idx_menu();
        else if (strcmp("\\echo index cursos_idx", input) == 0)
            imprimir_cursos_idx_menu();
        else if (strcmp("\\echo index inscricoes_idx", input) == 0)
            imprimir_inscricoes_idx_menu();
 
        /* Imprimir índices secundários */
        else if (strcmp("\\echo index titulo_idx", input) == 0)
            imprimir_titulo_idx_menu();
        else if (strcmp("\\echo index data_curso_usuario_idx", input) == 0)
            imprimir_data_curso_usuario_idx_menu();
        else if (strcmp("\\echo index categorias_secundario_idx", input) == 0)
            imprimir_categorias_secundario_idx_menu();
        else if (strcmp("\\echo index categorias_primario_idx", input) == 0)
            imprimir_categorias_primario_idx_menu();
 
        /* Liberar memória eventualmente alocada e encerrar programa */
        else if (strcmp("\\q", input) == 0)
            { return 0; }
        else if (sscanf(input, "SET SRAND %lu;", &seed) == 1)
            { prng_srand(seed); printf(SUCESSO); continue; }
        else if (sscanf(input, "SET TIME '%[^']';", datetime) == 1)
            { if (set_time(datetime)) printf(SUCESSO); else printf(ERRO_VALOR_INVALIDO); continue; }
        else
            printf(ERRO_OPCAO_INVALIDA);
 
        tick_time();
    }
}
 
/* ========================================================================== */
 
/* Cria o índice primário usuarios_idx */
void criar_usuarios_idx() {
    char usuario_str[TAM_CHAVE_USUARIOS_IDX + 1];
    for (unsigned i = 0; i < qtd_registros_usuarios; ++i) {
        Usuario u = recuperar_registro_usuario(i);
 
        sprintf(usuario_str, "%s%04d", u.id_usuario, i);
        btree_insert(usuario_str, &usuarios_idx);
    }
    printf(INDICE_CRIADO, "usuarios_idx");
}
 
/* Cria o índice primário cursos_idx */
void criar_cursos_idx() {
    char curso_str[TAM_CHAVE_CURSOS_IDX + 1];
    for (int i = 0; i < qtd_registros_cursos; i++) {
        Curso c = recuperar_registro_curso(i);
 
        sprintf(curso_str, "%s%04d", c.id_curso, i);
        btree_insert(curso_str, &cursos_idx);
    }
 
    printf(INDICE_CRIADO, "cursos_idx");
}
 
/* Cria o índice primário inscricoes_idx */
void criar_inscricoes_idx() {
    char inscricao_str[TAM_CHAVE_INSCRICOES_IDX + 1];
    for (int i = 0; i < qtd_registros_inscricoes; i++) {
        Inscricao ins = recuperar_registro_inscricao(i);
 
        sprintf(inscricao_str, "%s%s%04d", ins.id_curso, ins.id_usuario, i);
        btree_insert(inscricao_str, &inscricoes_idx);
    }
    
    printf(INDICE_CRIADO, "inscricoes_idx");
}
 
/* Cria o índice secundário titulo_idx */
void criar_titulo_idx() {
    char titulo_str[TAM_CHAVE_TITULO_IDX + 1], aux[TAM_MAX_TITULO + 1];
    for (int i = 0; i < qtd_registros_cursos; i++) {
        Curso c = recuperar_registro_curso(i);
 
        strcpy(aux, c.titulo);
        strupr(aux);
        strpadright(aux, '#', TAM_MAX_TITULO - 1);
 
        sprintf(titulo_str, "%s%s", aux, c.id_curso);
        btree_insert(titulo_str, &titulo_idx);
    }
    
    printf(INDICE_CRIADO, "titulo_idx");
}
 
/* Cria o índice secundário data_curso_usuario_idx */
void criar_data_curso_usuario_idx() {
    char data_curso_usuario_str[TAM_CHAVE_DATA_CURSO_USUARIO_IDX + 1];
    for (int i = 0; i < qtd_registros_inscricoes; i++) {
        Inscricao ins = recuperar_registro_inscricao(i);
 
        sprintf(data_curso_usuario_str, "%s%s%s", ins.data_inscricao, ins.id_curso, ins.id_usuario);
        btree_insert(data_curso_usuario_str, &data_curso_usuario_idx);
    }
 
    printf(INDICE_CRIADO, "data_curso_usuario_idx");
}
 
/* Cria os índices (secundário e primário) de categorias_idx */
void criar_categorias_idx() {
    for (int i = 0; i < qtd_registros_cursos; i++) {
        Curso c = recuperar_registro_curso(i);
 
        for (int i = 0; i < 3; i++) { // 3 representa a quantidade máxima de categorias permitida.
            if (c.categorias[i][0] != '\0')
                inverted_list_insert(c.categorias[i], c.id_curso, &categorias_idx);
        }
    }
    
    printf(INDICE_CRIADO, "categorias_idx");
}
 
 
/* Exibe um usuario dado seu RRN */
bool exibir_usuario(int rrn) {
    if (rrn < 0)
        return false;
 
    Usuario u = recuperar_registro_usuario(rrn);
 
    printf("%s, %s, %s, %s, %.2lf\n", u.id_usuario, u.nome, u.email, u.telefone, u.saldo);
    return true;
}
 
/* Exibe um curso dado seu RRN */
bool exibir_curso(int rrn) {
    if (rrn < 0)
        return false;
 
    Curso j = recuperar_registro_curso(rrn);
 
    printf("%s, %s, %s, %s, %s, %d, %.2lf\n", j.id_curso, j.titulo, j.instituicao, j.ministrante, j.lancamento, j.carga, j.valor);
    return true;
}
 
/* Exibe uma inscricao dado seu RRN */
bool exibir_inscricao(int rrn) {
    if (rrn < 0)
        return false;
 
    Inscricao c = recuperar_registro_inscricao(rrn);
 
    printf("%s, %s, %s, %c, %s\n", c.id_curso, c.id_usuario, c.data_inscricao, c.status, c.data_atualizacao);
 
    return true;
}
 
bool exibir_btree_usuario(char *chave) {
    int rrn = atoi(chave + TAM_ID_USUARIO - 1);
    return exibir_usuario(rrn);
}
 
bool exibir_btree_curso(char *chave) {
    int rrn = atoi(chave + TAM_ID_CURSO - 1);
    return exibir_curso(rrn);
}
 
bool exibir_btree_inscricao(char *chave) {
    int rrn = atoi(chave + TAM_ID_USUARIO + TAM_ID_CURSO - 2);
    return exibir_inscricao(rrn);
}
 
bool exibir_btree_titulo(char *chave) {
    char result[TAM_CHAVE_CURSOS_IDX + 1];
    
    if (btree_search(result, true, chave + TAM_MAX_TITULO - 1, cursos_idx.rrn_raiz, &cursos_idx)) {
        printf("\n");
        return exibir_btree_curso(result);
    }
    else {
        printf("\n");
        return false;
    }
}
 
bool exibir_btree_data_curso_usuario(char *chave) {
    char result[TAM_CHAVE_INSCRICOES_IDX + 1];
 
    if (btree_search(result, false, chave + TAM_DATETIME - 1, inscricoes_idx.rrn_raiz, &inscricoes_idx))
        return exibir_btree_inscricao(result);
    else
        return false;
}
 
/* Recupera do arquivo de usuários o registro com o RRN
 * informado e retorna os dados na struct Usuario */
Usuario recuperar_registro_usuario(int rrn) {
    Usuario u;
    char temp[TAM_REGISTRO_USUARIO + 1], *p;
    strncpy(temp, ARQUIVO_USUARIOS + (rrn * TAM_REGISTRO_USUARIO), TAM_REGISTRO_USUARIO);
    temp[TAM_REGISTRO_USUARIO] = '\0';
 
    p = strtok(temp, ";");
    strcpy(u.id_usuario, p);
    p = strtok(NULL, ";");
    strcpy(u.nome, p);
    p = strtok(NULL, ";");
    strcpy(u.email, p);
    p = strtok(NULL, ";");
    strcpy(u.telefone, p);
    p = strtok(NULL, ";");
    u.saldo = atof(p);
    p = strtok(NULL, ";");
 
    return u;
}
 
/* Recupera do arquivo de cursos o registro com o RRN
 * informado e retorna os dados na struct Curso */
Curso recuperar_registro_curso(int rrn) {
    Curso c; 
    char temp[TAM_REGISTRO_CURSO + 1], *p, *cat;
    int qtd_categorias = 0;
    
    strncpy(temp, ARQUIVO_CURSOS + (rrn * TAM_REGISTRO_CURSO), TAM_REGISTRO_CURSO);
    temp[TAM_REGISTRO_CURSO] = '\0';
 
    p = strtok(temp, ";");
    strcpy(c.id_curso, p);
    p = strtok(NULL, ";");
    strcpy(c.titulo, p);
    p = strtok(NULL, ";");
    strcpy(c.instituicao, p);
    p = strtok(NULL, ";");
    strcpy(c.ministrante, p);
    p = strtok(NULL, ";");
    strcpy(c.lancamento, p);
    p = strtok(NULL, ";");
    c.carga = atoi(p);
    p = strtok(NULL, ";");
    c.valor = atof(p);
    
    p = strtok(NULL, ";");
    if (p[0] != '#') {
        // Descobre quantas categorias o curso possui.
        qtd_categorias += 1;
        for (int i = 0; i < strlen(p); i++) {
            if (p[i] == '|')
                qtd_categorias += 1;
        }
 
        if (qtd_categorias == 1) {
            strcpy(c.categorias[0], p);
 
            c.categorias[1][0] = '\0';
            c.categorias[2][0] = '\0';
        }
        else { // Caso em que o curso tem mais que uma categoria.
            cat = strtok(p, "|");
            strcpy(c.categorias[0], cat);
    
            cat = strtok(NULL, "|");
            if (cat)
                strcpy(c.categorias[1], cat);
            else
                c.categorias[1][0] = '\0';
            
            cat = strtok(NULL, "|");
            if (cat)
                strcpy(c.categorias[2], cat);
            else
                c.categorias[1][0] = '\0';
        }
    }
    else  { // Caso em que o curso não possui categorias.
        c.categorias[0][0] = '\0';
        c.categorias[1][0] = '\0';
        c.categorias[2][0] = '\0';
    }
    p = strtok(NULL, ";");
 
    return c;
}
 
/* Recupera do arquivo de inscricoes o registro com o RRN
 * informado e retorna os dados na struct Inscricao */
Inscricao recuperar_registro_inscricao(int rrn) {
    Inscricao ins;
    char temp[TAM_REGISTRO_INSCRICAO + 1];
    
    strncpy(temp, ARQUIVO_INSCRICOES + (rrn * TAM_REGISTRO_INSCRICAO), TAM_REGISTRO_INSCRICAO);
    temp[TAM_REGISTRO_INSCRICAO] = '\0';
 
    strncpy(ins.id_curso, temp, TAM_ID_CURSO);
    ins.id_curso[TAM_ID_CURSO - 1] = '\0';
    
    strncpy(ins.id_usuario, temp + TAM_ID_CURSO - 1, TAM_ID_USUARIO);
    ins.id_usuario[TAM_ID_USUARIO - 1] = '\0';
    
    strncpy(ins.data_inscricao, temp + TAM_ID_CURSO + TAM_ID_USUARIO - 2, TAM_DATETIME);
    ins.data_inscricao[TAM_DATETIME - 1] = '\0';
    
    strncpy(&ins.status, temp + TAM_ID_CURSO + TAM_ID_USUARIO + TAM_DATETIME - 3, 1); // 1 = tamanho do status.
    
    strncpy(ins.data_atualizacao, temp + TAM_ID_CURSO + TAM_ID_USUARIO + TAM_DATETIME - 2, TAM_DATETIME);
    ins.data_atualizacao[TAM_DATETIME - 1] = '\0';
 
    return ins;
}
 
 
/* Escreve no arquivo de usuários na posição informada (RRN)
 * os dados na struct Usuario */
void escrever_registro_usuario(Usuario u, int rrn) {
    char temp[TAM_REGISTRO_USUARIO + 1], p[100];
    temp[0] = '\0'; p[0] = '\0';
 
    strcpy(temp, u.id_usuario);
    strcat(temp, ";");
    strcat(temp, u.nome);
    strcat(temp, ";");
    strcat(temp, u.email);
    strcat(temp, ";");
    strcat(temp, u.telefone);
    strcat(temp, ";");
    sprintf(p, "%013.2lf", u.saldo);
    strcat(temp, p);
    strcat(temp, ";");
 
    strpadright(temp, '#', TAM_REGISTRO_USUARIO);
 
    strncpy(ARQUIVO_USUARIOS + rrn*TAM_REGISTRO_USUARIO, temp, TAM_REGISTRO_USUARIO);
}
 
/* Escreve no arquivo de cursos na posição informada (RRN)
 * os dados na struct Curso */
void escrever_registro_curso(Curso j, int rrn) {
    char temp[TAM_REGISTRO_CURSO + 1], p[100];
    temp[0] = '\0';
    p[0] = '\0';
 
    strcpy(temp, j.id_curso);
    strcat(temp, ";");
    strcat(temp, j.titulo);
    strcat(temp, ";");
    strcat(temp, j.instituicao);
    strcat(temp, ";");
    strcat(temp, j.ministrante);
    strcat(temp, ";");
    strcat(temp, j.lancamento);
    strcat(temp, ";");
    sprintf(p, "%04d", j.carga);
    strcat(temp, p);
    strcat(temp, ";");
    sprintf(p, "%013.2lf", j.valor);
    strcat(temp, p);
    strcat(temp, ";");
 
    if (j.categorias[0][0] != '\0')
        strcat(temp, j.categorias[0]);
    
    if (j.categorias[1][0] != '\0') {
        strcat(temp, "|");        
        strcat(temp, j.categorias[1]);
    }
 
    if (j.categorias[2][0] != '\0') {
        strcat(temp, "|");        
        strcat(temp, j.categorias[2]);
        strcat(temp, "|");
    }
 
    strcat(temp, ";");
    
    strpadright(temp, '#', TAM_REGISTRO_CURSO);
    
    strncpy(ARQUIVO_CURSOS + rrn * TAM_REGISTRO_CURSO, temp, TAM_REGISTRO_CURSO);
}
 
/* Escreve no arquivo de inscricoes na posição informada (RRN)
 * os dados na struct Inscricao */
void escrever_registro_inscricao(Inscricao c, int rrn) {
    char temp[TAM_REGISTRO_INSCRICAO + 1];
    temp[0] = '\0';
 
    strcpy(temp, c.id_curso);
    strcat(temp, c.id_usuario);
    strcat(temp, c.data_inscricao);
    if (c.status == 'A')
        strcat(temp, "A");
    else if (c.status == 'I')
        strcat(temp, "I");
    else
        strcat(temp, "C");
    strcat(temp, c.data_atualizacao);
 
    strncpy(ARQUIVO_INSCRICOES + rrn * TAM_REGISTRO_INSCRICAO, temp, TAM_REGISTRO_INSCRICAO);
}
 
 
/* Funções principais */
void cadastrar_usuario_menu(char *id_usuario, char *nome, char *email, char *telefone) {
    if (btree_search(NULL, false, id_usuario, usuarios_idx.rrn_raiz, &usuarios_idx))
        printf(ERRO_PK_REPETIDA, id_usuario);
    else {     
        Usuario u;
        char buffer[TAM_CHAVE_USUARIOS_IDX + 1];
        buffer[0] = '\0';
        
        strcpy(u.nome, nome);
        strcpy(u.id_usuario, id_usuario);
        strcpy(u.email, email);
        strcpy(u.telefone, telefone);
        u.saldo = 0;
      
        // Atualiza o arquivo de usuários e o índice.
        escrever_registro_usuario(u, qtd_registros_usuarios);
        sprintf(buffer, "%s%04d", id_usuario, qtd_registros_usuarios);
        btree_insert(buffer, &usuarios_idx);
 
        qtd_registros_usuarios += 1;
        
        printf(SUCESSO);
    }
}
 
void cadastrar_telefone_menu(char* id_usuario, char* telefone) {
    char result[TAM_CHAVE_USUARIOS_IDX + 1];
    int cont = 0, iArq;
    
    if (btree_search(result, false, id_usuario, usuarios_idx.rrn_raiz, &usuarios_idx)) {
        int rrn;
        rrn = atoi(result + TAM_ID_USUARIO - 1);
        
        // Posiciona iArq no local correto para atualizar o telefone.
        for (iArq = rrn * TAM_REGISTRO_USUARIO; cont < 3; iArq++) {
            if (ARQUIVO_USUARIOS[iArq] == ';')
                cont += 1;
        }
 
        // Escreve no arquivo de usuários o novo telefone.
        for (int j = 0; j < 11; j++, iArq++) {
            ARQUIVO_USUARIOS[iArq] = telefone[j];
        }
                
        printf(SUCESSO);
    }
    else
        printf(ERRO_REGISTRO_NAO_ENCONTRADO);
}
 
void remover_usuario_menu(char *id_usuario) {
    char result[TAM_CHAVE_USUARIOS_IDX + 1];
 
    if (btree_search(result, false, id_usuario, usuarios_idx.rrn_raiz, &usuarios_idx)) {
        int rrn;
        rrn = atoi(result + TAM_ID_USUARIO - 1);
        
        ARQUIVO_USUARIOS[rrn * TAM_REGISTRO_USUARIO] = '*';
        ARQUIVO_USUARIOS[rrn * TAM_REGISTRO_USUARIO + 1] = '|';            
        btree_delete(result, &usuarios_idx); // Remove o usuário do índice.
                
        printf(SUCESSO);
    }
    else
        printf(ERRO_REGISTRO_NAO_ENCONTRADO);
}
 
void cadastrar_curso_menu(char *titulo, char *instituicao, char *ministrante, char* lancamento, int carga, double valor) {
    char aux[TAM_MAX_TITULO + 1]; // aux é utilizado para deixar o título em letra maiúscula.
    strcpy(aux, titulo);
    strupr(aux);
    
    if (btree_search(NULL, false, aux, titulo_idx.rrn_raiz, &titulo_idx)) 
        printf(ERRO_PK_REPETIDA, titulo);
    else {
        Curso c;
        char buffer[TAM_CHAVE_TITULO_IDX + 1]; TAM_CHAVE_CURSOS_IDX;
        buffer[0] = '\0';
        
        sprintf(c.id_curso, "%08d", qtd_registros_cursos);
        strcpy(c.titulo, titulo);
        strcpy(c.instituicao, instituicao);
        strcpy(c.ministrante, ministrante);
        strcpy(c.lancamento, lancamento);
        c.carga = carga;
        c.valor = valor;
 
        for (int i = 0; i < 3; i++)
            c.categorias[i][0] = '\0';
 
        escrever_registro_curso(c, qtd_registros_cursos); // Atualiza o arquivo de cursos.
        
        // Atualiza os índices necessários. 
        sprintf(buffer, "%s%04d", c.id_curso, qtd_registros_cursos);
        btree_insert(buffer, &cursos_idx);
 
        strcpy(buffer, aux);
        strpadright(buffer, '#', TAM_MAX_TITULO - 1);
        strcat(buffer, c.id_curso);
        btree_insert(buffer, &titulo_idx);
        
        qtd_registros_cursos += 1;
 
        printf(SUCESSO);
    }
}
 
void adicionar_saldo_menu(char *id_usuario, double valor) {
    double saldoAtual;
    int cont = 0, iArq;
    char p[TAM_FLOAT_NUMBER], result[TAM_CHAVE_USUARIOS_IDX + 1];
    p[0] = '\0';
    
    if (btree_search(result, false, id_usuario, usuarios_idx.rrn_raiz, &usuarios_idx)) {
        if (valor <= 0) {
            printf(ERRO_VALOR_INVALIDO);
        }
        else {
            int rrn;
            rrn = atoi(result + TAM_ID_USUARIO - 1);
            
            // Coloca iArq na posição correta.
            for (iArq = rrn * TAM_REGISTRO_USUARIO; cont < 4; iArq++) {
                if (ARQUIVO_USUARIOS[iArq] == ';')
                    cont += 1;
            }
 
            // Obtém o saldo atual do usuário.
            for (int i = 0; i < TAM_FLOAT_NUMBER - 1; i++) {
                p[i] = ARQUIVO_USUARIOS[iArq + i];
            }
            saldoAtual = atof(p);
 
            // Adiciona o valor ao saldo atual.
            saldoAtual += valor;
            sprintf(p, "%013.2lf", saldoAtual);
 
            for (int i = 0; i < TAM_FLOAT_NUMBER - 1; i++, iArq++) {
                ARQUIVO_USUARIOS[iArq] = p[i];
            }
                
            printf(SUCESSO);
        }
    }
    else
        printf(ERRO_REGISTRO_NAO_ENCONTRADO);
}
 
 
void inscrever_menu(char *id_curso, char *id_usuario) {
    char resultUsuario[TAM_CHAVE_USUARIOS_IDX + 1], resultCurso[TAM_CHAVE_CURSOS_IDX + 1];
    char inscricao_str[TAM_CHAVE_DATA_CURSO_USUARIO_IDX + 1];
    
    if (!btree_search(resultUsuario, false, id_usuario, usuarios_idx.rrn_raiz, &usuarios_idx) || !btree_search(resultCurso, false, id_curso, cursos_idx.rrn_raiz, &cursos_idx))
        printf(ERRO_REGISTRO_NAO_ENCONTRADO); // Usuário e/ou curso não existem.
    else {
        strcpy(inscricao_str, id_curso);
        strcat(inscricao_str, id_usuario);
        
        if (!btree_search(NULL, false, inscricao_str, inscricoes_idx.rrn_raiz, &inscricoes_idx)) {
            Inscricao ins;
            int rrnUsuario, rrnCurso;
            Usuario u;
            Curso c;
            char buffer[14]; // buffer é utilizado para obter a data atual.
 
            rrnUsuario = atoi(resultUsuario + TAM_ID_USUARIO - 1);
            rrnCurso = atoi(resultCurso + TAM_ID_CURSO - 1);
            u = recuperar_registro_usuario(rrnUsuario);
            c = recuperar_registro_curso(rrnCurso);
 
            if (u.saldo < c.valor)
                printf(ERRO_SALDO_NAO_SUFICIENTE);
            else {
                u.saldo = u.saldo - c.valor;
                escrever_registro_usuario(u, rrnUsuario);
                
                current_datetime(buffer);
                strncpy(ins.data_inscricao, buffer, 13);
                strncpy(ins.data_atualizacao, buffer, 13);
                strcpy(ins.id_usuario, id_usuario);
                strcpy(ins.id_curso, id_curso);
                ins.status = 'A';
    
                escrever_registro_inscricao(ins, qtd_registros_inscricoes);
    
                // Atualiza índice primário das inscrições.
                sprintf(inscricao_str, "%s%s%04d", ins.id_curso, ins.id_usuario, qtd_registros_inscricoes);
                btree_insert(inscricao_str, &inscricoes_idx);
 
                // Atualiza índice secundário das datas das inscrições.
                sprintf(inscricao_str, "%s%s%s", ins.data_inscricao, ins.id_curso, ins.id_usuario);
                btree_insert(inscricao_str, &data_curso_usuario_idx);
 
                qtd_registros_inscricoes += 1;
            
                printf(SUCESSO);
            }
        }
        else // Inscrição já foi realizada.
            printf(ERRO_PK_REPETIDA, strcat(id_curso, id_usuario));
    }
}
 
 
void cadastrar_categoria_menu(char* titulo, char* categoria) {
    int resultInvList = -1, rrn;
    char resultTitulo[TAM_CHAVE_TITULO_IDX + 1], aux[TAM_MAX_TITULO]; // aux é utilizado para deixar o título em letra maiúscula.
 
    strcpy(aux, titulo);
    strupr(aux);
 
    if (btree_search(resultTitulo, false, aux, titulo_idx.rrn_raiz, &titulo_idx)) {
        char id_curso[TAM_ID_CURSO], resultCurso[TAM_CHAVE_CURSOS_IDX], rrnInvList[TAM_RRN_REGISTRO + 1];
        
        strncpy(id_curso, resultTitulo + TAM_MAX_TITULO - 1, TAM_ID_CURSO - 1);
        btree_search(resultCurso, false, id_curso, cursos_idx.rrn_raiz, &cursos_idx);

        inverted_list_secondary_search(&resultInvList, false, categoria, &categorias_idx);

        // Verifrica se o curso já possui a categoria fornecida.
        while (resultInvList != -1) {
            if (!strncmp(id_curso, categorias_idx.arquivo_primario + (resultInvList * (TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX + TAM_RRN_REGISTRO)), TAM_ID_CURSO - 1)) {
                printf(ERRO_CATEGORIA_REPETIDA, titulo, categoria);
                break;
            }
            
            strncpy(rrnInvList, categorias_idx.arquivo_primario + (resultInvList * (TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX + TAM_RRN_REGISTRO)) + TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX, TAM_RRN_REGISTRO);
            resultInvList = atoi(rrnInvList); 
        }
        
        if (resultInvList == -1) { // Curso não possui a categoria.
            rrn = atoi(resultCurso + TAM_ID_CURSO - 1);
            Curso c = recuperar_registro_curso(rrn);
 
            // Atualiza o registro com a nova categoria.
            for (int i = 0; i < 3; i++) {
                if (c.categorias[i][0] == '\0') {
                    strcpy(c.categorias[i], categoria);
                    escrever_registro_curso(c, rrn);
                    break;
                }
            }
            
            strncpy(id_curso, resultTitulo + TAM_MAX_TITULO - 1, TAM_ID_CURSO - 1);
            id_curso[TAM_ID_CURSO - 1] = '\0';
            inverted_list_insert(categoria, id_curso, &categorias_idx); // Atualiza o índice categorias_idx.
            
            printf(SUCESSO);
        }
    }
    else
        printf(ERRO_REGISTRO_NAO_ENCONTRADO);
}
 
 
void atualizar_status_inscricoes_menu(char *id_usuario, char *titulo, char status) {
    char aux[TAM_MAX_TITULO], buffer[14], result[TAM_CHAVE_TITULO_IDX + 1]; // aux é utilizado para deixar o título em letra maiúscula e buffer para obter a data atual.
    char inscricao_str[TAM_CHAVE_INSCRICOES_IDX + 1];
 
    strcpy(aux, titulo);
    strupr(aux);
 
    if (btree_search(result, false, aux, titulo_idx.rrn_raiz, &titulo_idx)) {
        char id_curso[TAM_ID_CURSO + 1];
        
        strcpy(id_curso, result + strlen(result) - 8);
        strcpy(inscricao_str, id_curso);
        strcat(inscricao_str, id_usuario);
        
        if (btree_search(result, false, inscricao_str, inscricoes_idx.rrn_raiz, &inscricoes_idx)) {
            Inscricao ins;
            int rrn;
 
            rrn = atoi(result + TAM_ID_CURSO + TAM_ID_USUARIO - 2);
            
            // Atualiza o arquivo de inscrições.
            ins = recuperar_registro_inscricao(rrn);
            ins.status = status;
            current_datetime(buffer);
            strncpy(ins.data_atualizacao, buffer, 13);
            escrever_registro_inscricao(ins, rrn);
            
            printf(SUCESSO);
        }
        else
            printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    }
    else
        printf(ERRO_REGISTRO_NAO_ENCONTRADO);
}
 
 
/* Busca */
void buscar_usuario_id_menu(char *id_usuario) {
    char result[TAM_CHAVE_USUARIOS_IDX + 1];
 
    printf(RRN_NOS);
    
    if (btree_search(result, true, id_usuario, usuarios_idx.rrn_raiz, &usuarios_idx)) {
        printf("\n");
        exibir_btree_usuario(result);
    }
    else {
        printf("\n");
        printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    }
}
 
void buscar_curso_id_menu(char *id_curso) {
    char result[TAM_CHAVE_CURSOS_IDX + 1];
 
    printf(RRN_NOS);
    
    if (btree_search(result, true, id_curso, cursos_idx.rrn_raiz, &cursos_idx)) {
        printf("\n");
        exibir_btree_curso(result);
    }
    else {
        printf("\n");
        printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    }
}
 
void buscar_curso_titulo_menu(char *titulo) {
    char result[TAM_CHAVE_TITULO_IDX + 1], aux[TAM_MAX_TITULO + 1]; // aux é utilizado para deixar o título em letra maiúscula.
 
    strcpy(aux, titulo);
    strupr(aux);
 
    printf(RRN_NOS);
    
    if (btree_search(result, true, aux, titulo_idx.rrn_raiz, &titulo_idx)) {
        printf("\n");
        printf(RRN_NOS);
        exibir_btree_titulo(result);
    }
    else {
        printf("\n");
        printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    }
}
 
 
/* Listagem */
void listar_usuarios_id_menu() {
    if (!btree_print_in_order(NULL, NULL, exibir_btree_usuario, usuarios_idx.rrn_raiz, &usuarios_idx))
        printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
}
 
void listar_cursos_categorias_menu(char *categoria) {
    int resultSecondary, qtd_chaves;
    char aux[TAM_MAX_CATEGORIA], resultPrimary[qtd_registros_cursos][TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX]; // aux é a categoria em letra maiúscula.

    strcpy(aux, categoria);
    strupr(aux);
    
    if (qtd_registros_cursos != 0 && inverted_list_secondary_search(&resultSecondary, true, aux, &categorias_idx)) {
        qtd_chaves = inverted_list_primary_search(resultPrimary, true, resultSecondary, NULL, &categorias_idx);
        qsort(resultPrimary, qtd_chaves, sizeof(char) * TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX, order_cursos_idx);

        for (int i = 0; i < qtd_chaves; i++) {
            char result[TAM_CHAVE_CURSOS_IDX + 1], rrn[TAM_RRN_REGISTRO + 1];
            btree_search(result, false, resultPrimary[i], cursos_idx.rrn_raiz, &cursos_idx);
            strncpy(rrn, result + TAM_ID_CURSO - 1, TAM_RRN_REGISTRO);

            exibir_curso(atoi(rrn));
        }
    }
    else
        printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
}
 
void listar_inscricoes_periodo_menu(char *data_inicio, char *data_fim) {
    if (!btree_print_in_order(data_inicio, data_fim, exibir_btree_data_curso_usuario, data_curso_usuario_idx.rrn_raiz, &data_curso_usuario_idx))
        printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);    
}
 
 
/* Liberar espaço */
void liberar_espaco_menu() {
    char ARQUIVO_AUX[TAM_ARQUIVO_USUARIOS], aux[TAM_REGISTRO_USUARIO + 1], usuario_str[TAM_CHAVE_USUARIOS_IDX + 1];
    int iArq = 0, qtd_usuarios_nao_removidos = 0;
 
    // Remove os registros marcados como excluídos.
    for (int i = 0; i < qtd_registros_usuarios; i++) {
        iArq = i * TAM_REGISTRO_USUARIO;
 
        if (ARQUIVO_USUARIOS[iArq] != '*' && ARQUIVO_USUARIOS[iArq + 1] != '|') {
            if (qtd_usuarios_nao_removidos == 0)
                strncpy(ARQUIVO_AUX, ARQUIVO_USUARIOS + iArq, TAM_REGISTRO_USUARIO);
            else {
                strncpy(aux, ARQUIVO_USUARIOS + iArq, TAM_REGISTRO_USUARIO);
                strcat(ARQUIVO_AUX, aux);
            }
 
            qtd_usuarios_nao_removidos += 1;
        }
    }
 
    strcpy(ARQUIVO_USUARIOS, ARQUIVO_AUX);
    qtd_registros_usuarios = qtd_usuarios_nao_removidos;
 
    // Atualiza o índice.
    usuarios_idx.qtd_nos = 0;
 
    for (int i = 0; i < TAM_ARQUIVO_USUARIOS_IDX; i++)
        usuarios_idx.arquivo[i] = '\0';
    
    for (int i = 0; i < qtd_registros_usuarios; i++) {
        Usuario u = recuperar_registro_usuario(i);
 
        sprintf(usuario_str, "%s%04d", u.id_usuario, i);
        btree_insert(usuario_str, &usuarios_idx);
    }
 
    printf(SUCESSO);
}
 
 
/* Imprimir arquivos de dados */
void imprimir_arquivo_usuarios_menu() {
    if (qtd_registros_usuarios == 0)
        printf(ERRO_ARQUIVO_VAZIO);
    else
        printf("%s\n", ARQUIVO_USUARIOS);
}
 
void imprimir_arquivo_cursos_menu() {
    if (qtd_registros_cursos == 0)
        printf(ERRO_ARQUIVO_VAZIO);
    else
        printf("%s\n", ARQUIVO_CURSOS);
}
 
void imprimir_arquivo_inscricoes_menu() {
    if (qtd_registros_inscricoes == 0)
        printf(ERRO_ARQUIVO_VAZIO);
    else
        printf("%s\n", ARQUIVO_INSCRICOES);
}
 
 
/* Imprimir índices primários */
void imprimir_usuarios_idx_menu() {
    if (usuarios_idx.qtd_nos == 0)
        printf(ERRO_ARQUIVO_VAZIO);
    else
        printf("%s\n", ARQUIVO_USUARIOS_IDX);
}
 
void imprimir_cursos_idx_menu() {
    if (cursos_idx.qtd_nos == 0)
        printf(ERRO_ARQUIVO_VAZIO);
    else
        printf("%s\n", ARQUIVO_CURSOS_IDX);
}
 
void imprimir_inscricoes_idx_menu() {
    if (inscricoes_idx.qtd_nos == 0)
        printf(ERRO_ARQUIVO_VAZIO);
    else
        printf("%s\n", ARQUIVO_INSCRICOES_IDX);
}
 
 
/* Imprimir índices secundários */
void imprimir_titulo_idx_menu() {
    if (titulo_idx.qtd_nos == 0)
        printf(ERRO_ARQUIVO_VAZIO);
    else
        printf("%s\n", ARQUIVO_TITULO_IDX);
}
 
void imprimir_data_curso_usuario_idx_menu() {
    if (data_curso_usuario_idx.qtd_nos == 0)
        printf(ERRO_ARQUIVO_VAZIO);
    else
        printf("%s\n", ARQUIVO_DATA_CURSO_USUARIO_IDX);
}
 
void imprimir_categorias_secundario_idx_menu() {
    if (categorias_idx.qtd_registros_secundario == 0)
        printf(ERRO_ARQUIVO_VAZIO);
    else    
        printf("%s\n", ARQUIVO_CATEGORIAS_SECUNDARIO_IDX);
}
 
void imprimir_categorias_primario_idx_menu() {
    if (categorias_idx.qtd_registros_primario == 0)
        printf(ERRO_ARQUIVO_VAZIO);
    else
        printf("%s\n", ARQUIVO_CATEGORIAS_PRIMARIO_IDX);
}
 
 
/* Função de comparação entre chaves do índice usuarios_idx */
int order_usuarios_idx(const void *key, const void *elem) {
    return strncmp(key, elem, TAM_ID_USUARIO - 1);
}
 
/* Função de comparação entre chaves do índice cursos_idx */
int order_cursos_idx(const void *key, const void *elem) {
    return strncmp(key, elem, TAM_ID_CURSO - 1);
}
 
/* Função de comparação entre chaves do índice inscricoes_idx */
int order_inscricoes_idx(const void *key, const void *elem) {
    return strncmp(key, elem, TAM_ID_USUARIO + TAM_ID_CURSO - 2);
}
 
/* Função de comparação entre chaves do índice titulo_idx */
int order_titulo_idx(const void *key, const void *elem) {
    if (strlen(key) < strlen(elem))
        return strncmp(key, elem, strlen(key));
    else
        return strncmp(key, elem, strlen(elem));
}
 
/* Funções de comparação entre chaves do índice data_curso_usuario_idx */
int order_data_curso_usuario_idx(const void *key, const void *elem) {
    return strncmp(key, elem, TAM_DATETIME - 1);
}
 
/* Função de comparação entre chaves do índice secundário de categorias_idx */
int order_categorias_idx(const void *key, const void *elem) {
    if (strlen(key) < strlen(elem))
        return strncmp(key, elem, strlen(key) - 1);
    else
        return strncmp(key, elem, strlen(elem) - 1);
}
 
 
/* Funções de manipulação de Lista Invertida */
void inverted_list_insert(char *chave_secundaria, char *chave_primaria, inverted_list *t) {
    int result;
    char primaria_str[TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX + TAM_RRN_REGISTRO + 1];
    char aux[TAM_MAX_CATEGORIA]; // aux é a chave_secundaria em letra maiúscula.
    primaria_str[0] = '\0';
    
    strcpy(aux, chave_secundaria);
    strupr(aux);
    sprintf(primaria_str, "%s%s", chave_primaria, "-001");
    
    strncat(t->arquivo_primario, primaria_str, TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX + TAM_RRN_REGISTRO);

    if (inverted_list_secondary_search(&result, false, aux, t)) {
        char rrn[TAM_RRN_REGISTRO + 1];
 
        while (true) {
            strncpy(rrn, t->arquivo_primario + (result * (TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX + TAM_RRN_REGISTRO)) + TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX, TAM_RRN_REGISTRO);
            rrn[TAM_RRN_REGISTRO] = '\0';
            
            if (atoi(rrn) == -1)
                break;
            else
                result = atoi(rrn);                    
        }
        
        sprintf(rrn, "%04d", t->qtd_registros_primario);
        strncpy(t->arquivo_primario + result * (TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX + TAM_RRN_REGISTRO) + TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX, rrn, TAM_RRN_REGISTRO);
        
    }
    else {
        char secundario_str[TAM_CHAVE_CATEGORIAS_SECUNDARIO_IDX + TAM_RRN_REGISTRO + 1];
 
        strpadright(aux, '#', TAM_MAX_CATEGORIA - 1);
        sprintf(secundario_str, "%s%04d", aux, t->qtd_registros_primario);
        strcat(t->arquivo_secundario, secundario_str);
        
        t->qtd_registros_secundario += 1;
        qsort(t->arquivo_secundario, t->qtd_registros_secundario, TAM_CHAVE_CATEGORIAS_SECUNDARIO_IDX + TAM_RRN_REGISTRO, t->compar);
    }
 
    t->qtd_registros_primario += 1;
}
 
bool inverted_list_secondary_search(int *result, bool exibir_caminho, char *chave_secundaria, inverted_list *t) {
    int resultBinaria;
    char indiceInicial[TAM_RRN_REGISTRO + 1];
    
    if (inverted_list_binary_search(&resultBinaria, exibir_caminho, chave_secundaria, t)) {
        strncpy(indiceInicial, t->arquivo_secundario + resultBinaria * (TAM_CHAVE_CATEGORIAS_SECUNDARIO_IDX + TAM_RRN_REGISTRO) + TAM_CHAVE_CATEGORIAS_SECUNDARIO_IDX, TAM_RRN_REGISTRO);
        *result = atoi(indiceInicial);
        return true;
    }
    else
        return false;
}
 
int inverted_list_primary_search(char result[][TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX], bool exibir_caminho, int indice, int *indice_final, inverted_list *t) {
    int qtd_chaves = 0;
    char prox_indice[TAM_RRN_REGISTRO + 1];
    
    if (exibir_caminho) {
        printf(RRN_REGS_PRIMARIOS);
        printf(" %d", indice);
    }

    if (result)
        strncpy(result[qtd_chaves], t->arquivo_primario + (indice * (TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX + TAM_RRN_REGISTRO)), TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX);
    qtd_chaves += 1;
    
    strncpy(prox_indice, t->arquivo_primario + (indice * (TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX + TAM_RRN_REGISTRO)) + TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX, TAM_RRN_REGISTRO);

    while (atoi(prox_indice) != -1) {
        indice = atoi(prox_indice);

        if (result)
            strncpy(result[qtd_chaves], t->arquivo_primario + (indice * (TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX + TAM_RRN_REGISTRO)), TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX);
        
        strncpy(prox_indice, t->arquivo_primario + (indice * (TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX + TAM_RRN_REGISTRO)) + TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX, TAM_RRN_REGISTRO);       
        qtd_chaves += 1;
        
        if (exibir_caminho)
            printf(" %d", indice);
    }

    if (indice_final)
        *indice_final = indice;
    
    if (exibir_caminho)
        printf("\n");
    
    return qtd_chaves;
}
 
bool inverted_list_binary_search(int* result, bool exibir_caminho, char *chave, inverted_list *t) {
    int menor = 0, maior = t->qtd_registros_secundario - 1, meio = 0;
    
    if (exibir_caminho)
        printf(RRN_REGS_SECUNDARIOS);
    
    while (menor <= maior) {
        if ((maior + menor) % 2 != 0 && (maior + menor) != 0)
            meio = ((maior + menor) / 2) + 1;
        else
            meio = (maior + menor) / 2;
                
        if (exibir_caminho)
            printf(" %d", meio);
        
        if (!t->compar(chave, t->arquivo_secundario + meio * (TAM_CHAVE_CATEGORIAS_SECUNDARIO_IDX + TAM_RRN_REGISTRO))) {
            if (result)
                *result = meio;
        
            if (exibir_caminho)
                printf("\n");
            
            return true;
        }
        else if (t->compar(chave, t->arquivo_secundario + meio * (TAM_CHAVE_CATEGORIAS_SECUNDARIO_IDX + TAM_RRN_REGISTRO)) < 0)
            maior = meio - 1;
        else
            menor = meio + 1;
    }
 
    if (exibir_caminho)
        printf("\n");
    
    return false;
}
 
 
/* Funções de manipulação de Árvores-B */
void btree_insert_node_key(promovido_aux promo, btree_node *no, btree *t) {
    // Funçaõ responsável por inserir uma determinada chave em um nó.
    for (int i = 0; i < no->qtd_chaves; i++) {
        if (t->compar(promo.chave_promovida, no->chaves[i]) < 0) {
            for (int j = no->qtd_chaves - 1; j >= i; j--) {
                strncpy(no->chaves[j + 1], no->chaves[j], t->tam_chave);
                no->filhos[j + 2] = no->filhos[j + 1];
            }
    
            strncpy(no->chaves[i], promo.chave_promovida, t->tam_chave);
            no->filhos[i + 1] = promo.filho_direito;
            
            break;
        }
        else if (i + 1 == no->qtd_chaves) {
            strncpy(no->chaves[no->qtd_chaves], promo.chave_promovida, t->tam_chave);
            no->filhos[no->qtd_chaves + 1] = promo.filho_direito;
        }
    }
    
    no->qtd_chaves += 1;
    btree_write(*no, t);
}
 
void btree_insert(char *chave, btree *t) {
    if (t->qtd_nos == 0) { // Arvore vazia.
        btree_node no = btree_node_malloc(t);
        
        no.this_rrn = t->qtd_nos;
        no.qtd_chaves = 0;
        
        t->qtd_nos += 1;
        t->rrn_raiz = no.this_rrn;
        
        strcpy(no.chaves[no.qtd_chaves], chave);
        no.qtd_chaves += 1;
        no.folha = true;
 
        btree_write(no, t);
        btree_node_free(no);
    }
    else { // Arvore nao vazia.
        promovido_aux promo;
        promo = btree_insert_aux(chave, t->rrn_raiz, t);
        
        if (promo.chave_promovida[0] != '\0') { // Overflow na raiz.
            btree_node nova_raiz = btree_node_malloc(t);
            nova_raiz.qtd_chaves = 0;
 
            strncpy(nova_raiz.chaves[0], promo.chave_promovida, t->tam_chave);
            nova_raiz.qtd_chaves += 1;
 
            nova_raiz.filhos[0] = t->rrn_raiz;
            nova_raiz.filhos[1] = promo.filho_direito;
            
            nova_raiz.folha = false;
            nova_raiz.this_rrn = t->qtd_nos;
            
            t->qtd_nos += 1;
            t->rrn_raiz = nova_raiz.this_rrn;
 
            btree_write(nova_raiz, t);
            btree_node_free(nova_raiz);
        }
    }
}

/* Conversei sobre essa função com o Sergio Neres */
promovido_aux btree_insert_aux(char *chave, int rrn, btree *t) {
    promovido_aux promo;
    strcpy(promo.chave_promovida, chave);
    promo.filho_direito = -1;
 
    if (rrn >= 0) {
        btree_node no = btree_read(rrn, t);
        int result;
        
        if (!btree_binary_search(&result, false, chave, &no, t)) { 
            promo = btree_insert_aux(chave, no.filhos[result], t);
 
            if (promo.chave_promovida[0] != '\0') {
                if (no.qtd_chaves == btree_order - 1) // Precisa dividir o nó.
                    promo = btree_divide(promo, &no, result, t);
                else { // Só inserir a chave.
                    btree_insert_node_key(promo, &no, t);
                    promo.chave_promovida[0] = '\0';
                }
            }
        }
        btree_node_free(no);
    }
 
    return promo;
}
 
promovido_aux btree_divide(promovido_aux promo, btree_node *node, int i, btree *t) {
    btree_node no_novo = btree_node_malloc(t);
    int btree_min_elem = ceil((btree_order - 1) / 2); 
    no_novo.folha = node->folha;
    no_novo.this_rrn = t->qtd_nos;
    no_novo.qtd_chaves = 0;
    t->qtd_nos += 1;
 
    // Inserindo chave nova.
    if (i > btree_order / 2) {
        strcpy(no_novo.chaves[0], promo.chave_promovida);
        no_novo.filhos[0] = node->filhos[node->qtd_chaves];
        no_novo.filhos[1] = promo.filho_direito;
        no_novo.qtd_chaves += 1;
        node->filhos[node->qtd_chaves] = -1;
    }
    else {
        strcpy(no_novo.chaves[0], node->chaves[node->qtd_chaves - 1]);
        
        if (i >= btree_order / 2) {
            no_novo.filhos[0] = promo.filho_direito;
            promo.filho_direito = -1;
        }
        else {
        no_novo.filhos[0] = node->filhos[node->qtd_chaves - 1];
        node->filhos[node->qtd_chaves - 1] = -1;            
        }
        no_novo.filhos[1] = node->filhos[node->qtd_chaves];
        node->filhos[node->qtd_chaves] = -1;
        
        node->qtd_chaves -= 1;
        no_novo.qtd_chaves += 1;        
        btree_insert_node_key(promo, node, t);   
    }

    // Passando chaves para o nó novo.
    for (int j = btree_min_elem, k = no_novo.qtd_chaves; k < btree_min_elem; j++, k++) {
        strcpy(no_novo.chaves[k], node->chaves[j]);
        no_novo.filhos[k + 1] = node->filhos[j + 1];
        node->filhos[j + 1] = -1;
        node->qtd_chaves -= 1;
        no_novo.qtd_chaves += 1;
    }
 
    // Promovendo chave.
    strcpy(promo.chave_promovida, node->chaves[node->qtd_chaves - 1]);
    node->qtd_chaves -= 1;
    
    promo.filho_direito = no_novo.this_rrn;
    btree_write(*node, t);
    btree_write(no_novo, t);
    btree_node_free(no_novo);

    return promo;
}
 
void btree_delete(char *chave, btree *t) {
    if (t->qtd_nos) {
        if (btree_delete_aux(chave, t->rrn_raiz, t))
            t->rrn_raiz = 0;
    }
}
 
bool btree_delete_aux(char *chave, int rrn, btree *t) {
    if (rrn >= 0) {
        btree_node no = btree_read(rrn, t);
        int result = 0, btree_min_elem = ceil((btree_order - 1) / 2); 
 
        if (btree_binary_search(&result, false, chave, &no, t)) { // Encontrou a chave.
            if (no.folha) { // O nó é folha, basta remover.
                if (result != no.qtd_chaves - 1) {
                    for (int i = result + 1; i < no.qtd_chaves; i++)
                        strcpy(no.chaves[i - 1], no.chaves[i]);
                }
            
                no.qtd_chaves -= 1;
                btree_write(no, t);
                
                if (no.qtd_chaves < btree_min_elem)
                    return true;
                else
                    return false;
            }
            else { // A chave não está em um nó folha.
                btree_node no_pre = btree_read(no.filhos[result], t);
                strncpy(no.chaves[result], no_pre.chaves[no_pre.qtd_chaves - 1], t->tam_chave);
 
                btree_write(no, t);
                btree_node_free(no_pre);
                
                if (btree_delete_aux(no.chaves[result], no.filhos[result], t))
                    return btree_borrow_or_merge(&no, result, t);
            }
        }
        else { // Nao encontrou a chave.
            if (btree_delete_aux(chave, no.filhos[result], t))
                return btree_borrow_or_merge(&no, result, t); 
        }
 
        btree_node_free(no);
    }
 
    return false;
}
 
bool btree_borrow_or_merge(btree_node *node, int i, btree *t) {
    int btree_min_elem = ceil((btree_order - 1) / 2);
    btree_node no_remocao = btree_read(node->filhos[i], t);
    btree_node irmao_direita = btree_read(node->filhos[i + 1], t);
    btree_node irmao_esquerda = btree_read(node->filhos[i - 1], t);

    if (i + 1 < btree_order && node->filhos[i + 1] != -1) { // Verifica se existe um nó irmão à direita.
        if (irmao_direita.qtd_chaves > btree_min_elem) { // Irmão à direita pode emprestar.
            strcpy(no_remocao.chaves[no_remocao.qtd_chaves], node->chaves[i]);
            strcpy(node->chaves[i], irmao_direita.chaves[0]);
 
            for (int j = irmao_direita.qtd_chaves - 1; j > 0; j--)
                strcpy(irmao_direita.chaves[j - 1], irmao_direita.chaves[j]);            
 
            irmao_direita.qtd_chaves -= 1;
            no_remocao.qtd_chaves += 1;
 
            btree_write(*node, t);
            btree_write(no_remocao, t);
            btree_write(irmao_direita, t);
            btree_node_free(no_remocao);
            btree_node_free(irmao_direita);
            btree_node_free(irmao_esquerda);
            
            return false;
        }
    }
    
    if (i - 1 >= 0 && node->filhos[i - 1] != -1) { // Caso o irmão à direita não puder emprestar, verifica se existe um nó irmão à esquerda.
        if (irmao_esquerda.qtd_chaves > btree_min_elem) { // Irmão à esquerda pode emprestar.
            for (int j = no_remocao.qtd_chaves - 1; j > 0; j--) // Preparando o nó para receber a chave.
                strcpy(no_remocao.chaves[j + 1], no_remocao.chaves[j]);
                
            strcpy(no_remocao.chaves[0], node->chaves[i - 1]);
            strcpy(node->chaves[i - 1], irmao_esquerda.chaves[irmao_esquerda.qtd_chaves - 1]);
            irmao_esquerda.qtd_chaves -= 1;
            no_remocao.qtd_chaves += 1;
            
            btree_write(*node, t);
            btree_write(no_remocao, t);
            btree_write(irmao_esquerda, t);
            btree_node_free(no_remocao);
            btree_node_free(irmao_esquerda);
            btree_node_free(irmao_direita);
            
            return false;            
        }
    }
 
    if (i + 1 < btree_order && node->filhos[i + 1] != -1) { // Concatena com o irmão à direita, se possível.
        strcpy(no_remocao.chaves[no_remocao.qtd_chaves], node->chaves[i]);
        no_remocao.qtd_chaves += 1;
 
        for (int j = i; j < node->qtd_chaves - 1; j++)
            strcpy(node->chaves[j], node->chaves[j + 1]);
        
        for (int j = 0; j < irmao_direita.qtd_chaves; j++) {
            strcpy(no_remocao.chaves[no_remocao.qtd_chaves], irmao_direita.chaves[j]);
            no_remocao.filhos[no_remocao.qtd_chaves + 1] = irmao_direita.filhos[j + 1];
            no_remocao.qtd_chaves += 1;
        }
 
        node->qtd_chaves -= 1;
 
        if (!node->qtd_chaves) {
            for (int i = 0; i <= btree_order; i++)
                node->filhos[i] = -1;
        }
        else {
            node->filhos[i + 1] = node->filhos[i + 2];
            node->filhos[i + 2] = -1;
        }
        irmao_direita.qtd_chaves = 0;
        t->qtd_nos -= 1;
 
        btree_write(*node, t);
        btree_write(no_remocao, t);
        btree_write(irmao_direita, t);
        btree_node_free(no_remocao);
        btree_node_free(irmao_direita);
        btree_node_free(irmao_esquerda);
 
        if (node->qtd_chaves < btree_min_elem)
            return true;
        else
            return false;
    }
    else if (i - 1 >= 0 && node->filhos[i - 1] != -1) { // Concatena com o irmão à esquerda, se possível.
        strcpy(irmao_esquerda.chaves[irmao_esquerda.qtd_chaves], node->chaves[i]);
        irmao_esquerda.qtd_chaves += 1;
 
        for (int j = i; j < node->qtd_chaves - 1; j++)
            strcpy(node->chaves[j], node->chaves[j + 1]);
        
        for (int j = 0; j < irmao_direita.qtd_chaves; j++) {
            strcpy(irmao_esquerda.chaves[no_remocao.qtd_chaves], no_remocao.chaves[j]);
            irmao_esquerda.filhos[no_remocao.qtd_chaves + 1] = no_remocao.filhos[j + 1];
            irmao_esquerda.qtd_chaves += 1;
        }
 
        node->qtd_chaves -= 1;
 
        if (!node->qtd_chaves) {
            for (int i = 0; i <= btree_order; i++)
                node->filhos[i] = -1;
        }
        else {
            node->filhos[i] = node->filhos[i + 1];
            node->filhos[i + 1] = -1;
        }
        no_remocao.qtd_chaves = 0;
        t->qtd_nos -= 1;        
        
        btree_write(*node, t);
        btree_write(no_remocao, t);
        btree_write(irmao_direita, t);
        btree_node_free(no_remocao);
        btree_node_free(irmao_direita);
        btree_node_free(irmao_esquerda);
        
        if (node->qtd_chaves < btree_min_elem)
            return true;
        else
            return false;
    }
 
    return false;
}
 
bool btree_search(char *result, bool exibir_caminho, char *chave, int rrn, btree *t) {
    if (exibir_caminho)
        printf(" %d", rrn);
    
    if (!t->qtd_nos)
        return false;
    
    int resultBinaria;
    btree_node no = btree_read(rrn, t);
 
    if (btree_binary_search(&resultBinaria, exibir_caminho, chave, &no, t)) { // Encontrou.
        if (result)
            strcpy(result, no.chaves[resultBinaria]);
 
        return true;
    }
    else if (!no.folha)
        return btree_search(result, exibir_caminho, chave, no.filhos[resultBinaria], t);
    else
        return false;
 
    btree_node_free(no);
}
 
bool btree_binary_search(int *result, bool exibir_caminho, char* chave, btree_node* node, btree* t) {
    int menor = 0, maior = node->qtd_chaves - 1, meio = 0;
 
    if (exibir_caminho)
        printf(" (");
    
    while (menor <= maior) {
        if ((maior + menor) % 2 != 0 && (maior + menor) != 0)
            meio = ((maior + menor) / 2) + 1;
        else
            meio = (maior + menor) / 2;
 
        if (exibir_caminho)
            printf(" %d", meio);
        
        if (!t->compar(chave, node->chaves[meio])) {
            if (result)
                *result = meio;
        
            if (exibir_caminho)
                printf(")");
            
            return true;
        }
        else if (t->compar(chave, node->chaves[meio]) < 0)
            maior = meio - 1;
        else
            menor = meio + 1;
    }
 
    if (result) {
        if (t->compar(chave, node->chaves[meio]) > 0)
            *result = meio + 1;
        else
            *result = meio;
    }
 
    if (exibir_caminho)
        printf(")");
    
    return false;
}
 
bool btree_print_in_order(char *chave_inicio, char *chave_fim, bool (*exibir)(char *chave), int rrn, btree *t) {
    if (!t->qtd_nos)
        return false;
    else {
        btree_node no = btree_read(rrn, t);
        
        for (int i = 0; i < no.qtd_chaves; i++) {
            if (no.filhos[i] != -1)
                btree_print_in_order(chave_inicio, chave_fim, exibir, no.filhos[i], t);
 
            if (chave_inicio != NULL && chave_fim != NULL) {
                if (t->compar(no.chaves[i], chave_inicio) > 0 && t->compar(no.chaves[i], chave_fim) < 0)
                    exibir(no.chaves[i]);
            }
            else
                exibir(no.chaves[i]);
        }
 
        if (no.filhos[no.qtd_chaves] != -1)
            btree_print_in_order(chave_inicio, chave_fim, exibir, no.filhos[no.qtd_chaves], t);
 
        return true;
    }
}
 
btree_node btree_read(int rrn, btree *t) {
    int pArq, tamRegistro;
    char buffer[t->tam_chave + 1];
    
    btree_node no = btree_node_malloc(t);
    buffer[0] = '\0';
    tamRegistro = btree_register_size(t);
    pArq = tamRegistro * rrn;
    no.this_rrn = rrn;
    
    strncpy(buffer, t->arquivo + pArq, 3);
    buffer[3] = '\0';
    no.qtd_chaves = atoi(buffer);
    pArq += 3;
 
    for (int i = 0; i < btree_order - 1; i++) {
        strncpy(buffer, t->arquivo + pArq, t->tam_chave);
        buffer[t->tam_chave] = '\0';
        
        if (buffer[0] != '#')
            strcpy(no.chaves[i], buffer);
 
        pArq += t->tam_chave;
    }
 
    strncpy(buffer, t->arquivo + pArq, 1);
    
    if (buffer[0] == 'T')
        no.folha = true;
    else
        no.folha = false;
    
    pArq += 1;
 
    for (int i = 0; i < btree_order; i++) {
        strncpy(buffer, t->arquivo + pArq, 3);
        buffer[3] = '\0';
        
        if (buffer[0] == '*')
            no.filhos[i] = -1;
        else
            no.filhos[i] = atoi(buffer);
 
        pArq += 3;
    }
    
    return no;
}
 
void btree_write(btree_node no, btree *t) {
    int tam = btree_register_size(t);
    int chavesNaoUsadas = btree_order - 1 - no.qtd_chaves;
    char no_str[tam + 1], buffer[t->tam_chave + 1];
 
    buffer[0] = '\0';
    no_str[0] = '\0';
    
    sprintf(buffer, "%03d", no.qtd_chaves);
    strcpy(no_str, buffer);
 
    for (int i = 0; i < no.qtd_chaves; i++)
        strncat(no_str, no.chaves[i], t->tam_chave);
 
    if (chavesNaoUsadas != 0)
        strpadright(no_str, '#', strlen(no_str) + t->tam_chave * chavesNaoUsadas);
    
    if (no.folha)
        strcat(no_str, "T");
    else
        strcat(no_str, "F");
 
    for (int i = 0; i < btree_order; i++) {
        if (no.filhos[i] == -1)
            strcat(no_str, "***");
        else {
            sprintf(buffer, "%03d", no.filhos[i]);
            strncat(no_str, buffer, 3);
        }       
    }
 
    // Copia o nó na posição correta do arquivo.
    for (int iArq = no.this_rrn * tam, i = 0; i < tam; i++, iArq++)
        t->arquivo[iArq] = no_str[i];
}
 
int btree_register_size(btree *t) {
    int chaves_ordenadas = (btree_order-1)*t->tam_chave;
    return 3 + chaves_ordenadas + 1 + (btree_order*3);
}
 
btree_node btree_node_malloc(btree *t) {
    btree_node no;
 
    no.chaves = malloc((btree_order-1) * sizeof(char*));
    for (int i = 0; i < btree_order-1; ++i) {
        no.chaves[i] = malloc(t->tam_chave+1);
        no.chaves[i][0] = '\0';
    }
 
    no.filhos = malloc(btree_order * sizeof(int));
    for (int i = 0; i < btree_order; ++i)
        no.filhos[i] = -1;
 
    return no;
}
 
void btree_node_free(btree_node no) {
    for (int i = 0; i < btree_order-1; ++i)
        free(no.chaves[i]);
 
    free(no.chaves);
    free(no.filhos);
}
 
 
char* strpadright(char *str, char pad, unsigned size) {
    for (unsigned i = strlen(str); i < size; ++i)
        str[i] = pad;
    str[size] = '\0';
    return str;
}
 
char *strupr(char *str) {
    for (char *p = str; *p; ++p)
        *p = toupper(*p);
    return str;
}
 
char *strlower(char *str) {
    for (char *p = str; *p; ++p)
         *p = tolower(*p);
    return str;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#define DIM 100
#define R 6731
#define PI 3.14159265358979323846


#ifndef PARAMETROS_H
#define PARAMETROS_H
/*
    Informações sobre localidade
*/
typedef struct {
        char nome[DIM]; 
        float latitude;
        float longitude;
        float distancia;
} LOCALIDADE;

/*
    Item da lista de localidades
*/
typedef struct lista{
        LOCALIDADE localidade;
        struct lista * prox;
} ITEM_LOCAL;

/*
    Informaçao recebida dos comandos
*/
typedef struct info{
        char output1[DIM]; /*ficheiro de output, LO*/
        char output2[DIM]; /*ficheiro de output, LR*/
        int nome_rota; /*numero da rota, e inicializado a zero, ROTAS*/
        char nome_rem[DIM]; /*nome local a remover, REM*/
        LOCALIDADE localidade_ad; /*instancia de localidade a adicionar, ADI*/
}INFO;

/*
    Contem as informações associadas aos ficheiros de output
*/
typedef struct ficheiro_linhas
{
    int numero_linhas;
    int numero_localidades;
    int numero_rotas;
    FILE* fp_output1; 
    FILE* fp_output2; 
} META_FILE;

/*
    Item da lista de rotas
*/
typedef struct rotas{
    int nome_rota;
    struct rotas* prox;
    ITEM_LOCAL* topo_item_lista;
}ITEM_ROTA;

/*funcoes de verificacao e detecao de erros*/
void EXIT (int code);
void verifica_localidade(LOCALIDADE localidade, int linha, int print_erro);
void verifica_retorno(int retorno, int linha, int print_erro);
void verifica_cidade_nova(INFO dados_obtidos);
void verifica_localidade_a_adicionar(LOCALIDADE localidade);

/*funcoes de lista de localidades (comandos 1,2,3)*/
META_FILE * verfica_retorna_meta_ficheiro(INFO dados_obtidos, ITEM_LOCAL ** head, int print_erro);
int le_verifica_guarda(int print_erro, META_FILE* info_ficheiro, ITEM_LOCAL** head);
void ordena_alfabeticamente(ITEM_LOCAL** head, int linhas);
void adiciona_a_lista(ITEM_LOCAL ** head, LOCALIDADE localidade);
ITEM_LOCAL * adiciona_no_local(ITEM_LOCAL* atual, LOCALIDADE localidade);
void print_lista_localidades(ITEM_LOCAL* head, FILE* fp);

/*funcoes especificas de comandos 1 e 3*/
void print_output_com1 (ITEM_LOCAL * head, META_FILE * mfp);
void encontra_e_apaga_localidade(INFO dados_obtidos, ITEM_LOCAL ** head);

/*funcoes de lista de lista de rotas (comando 4)*/
void cria_rotas(INFO dados_obtidos, ITEM_LOCAL** head_localidades, ITEM_ROTA** head_rotas, META_FILE* ficheiro);
void add_rota(ITEM_ROTA ** atual_coluna, ITEM_LOCAL** atual_item_lista, int numero_rota);
void add_item_to_rota(ITEM_LOCAL** atual_item_lista, ITEM_LOCAL** previous_item_lista, ITEM_LOCAL** head_localidades, char nome_local[]);
float calcula_distancia(LOCALIDADE local1, LOCALIDADE local2);
void ordena_rotas(ITEM_ROTA** head_rotas, META_FILE* ficheiro, INFO dados_obtidos);
void print_lista_rotas(ITEM_ROTA** head_rotas, META_FILE* info_ficheiro, int* numero_da_rota);

/*funcoes de execucao de cada comando*/
void executa(int codigo_comando, INFO dados_recebidos, ITEM_LOCAL** head_localidades, ITEM_ROTA** head_rotas);
void com1(INFO dados_obtidos, ITEM_LOCAL ** head);
void com2(INFO dados_obtidos, ITEM_LOCAL ** head);
void com3(INFO dados_obtidos, ITEM_LOCAL ** head);
void com4(INFO dados_obtidos, ITEM_LOCAL ** head_localidades, ITEM_ROTA** head_rotas);

/*funcoes de rececao de argumentos e validacao dos dados*/
int verifica_numero (char* argv[], int x, int com, int* vezes_ponto);
int find_command (int* pos, char* argv[], int id, int argc);
void define_com (char* argv[], int argc, int* com);
void verifica_numero_parametros (int argc, int numero_parametros);
void find_com1_info (INFO *dados_recebidos, char* argv[], int argc);
void find_com2_info (INFO *dados_recebidos, char* argv[], int argc);
void find_com3_info (INFO *dados_recebidos, char* argv[],int argc);
void find_com4_info (INFO *dados_recebidos, char* argv[], int argc);

#endif
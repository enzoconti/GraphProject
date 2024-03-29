#ifndef __TOPOLOGIA_H__
#define __TOPOLOGIA_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*

Definição de constantes que serão utilizadas ao longo do programa:

TAM_PAG_DISCO -> define o tamanho de uma página de disco em bytes (960).
TAM_MAX_CAMPO_VARIAVEL -> define o tamanho máximo dos campos de tamanho variável em bytes(32).
TAM_REG_DADOS -> define o tamanho de um registro de dados em bytes (64).
TAM_REG_CABECALHO -> define o tamanho de um registro de cabeçalho em bytes (21).
TAM_NOME_ARQUIVO -> define um tamanho máximo de um nome de arquivo em bytes (20).

*/
#define TAM_PAG_DISCO 960
#define TAM_MAX_CAMPO_VARIAVEL 32
#define TAM_REG_DADOS 64
#define TAM_REG_CABECALHO 21
#define TAM_NOME_ARQUIVO 20
#define TAM_PAG_ARVORE 65
#define TAM_REG_CABECALHO_ARVORE 17
#define TAM_REG_DADOS_ARVORE 65
#define TAM_NOME_ARQUIVO 20
#define ORDEM_ARVORE_B 5

#define ENCONTRADO 1
#define NAO_ENCONTRADO 0
#define SEARCH_ERROR -1

/*

Definição de uma estrutura do registro de dados, com os campos definidos conforme especificações. Os campos "removido",
"siglaPais", e "unidadeMedida" são inicializados com 1 byte de espaço a mais do que o necesśario, para armazenamento do
caracter '\0', para controle interno do programa. Além disso, os campos "nomePoPs" e "nomePais", de tamanho variável, são
alocados estaticamente com um tamanho fixo de 32 bytes para controle interno do programa.

*/
typedef struct reg_dados{

    char removido[2];
    int encadeamento;
    int idConecta;
    char siglaPais[3];
    int idPoPsConectado;
    char unidadeMedida[2];
    int velocidade;
    char nomePoPs[TAM_MAX_CAMPO_VARIAVEL];
    char nomePais[TAM_MAX_CAMPO_VARIAVEL];
  
}reg_dados;

typedef struct reg_dados_indice{

    char folha[2];
    int nroChavesNo;
    int alturaNo;
    int RRNdoNo;
    int ponteiroSubarvore[ORDEM_ARVORE_B];
    int chaveBusca[ORDEM_ARVORE_B-1];
    int RRNdoRegistro[ORDEM_ARVORE_B-1];
}reg_dados_indice;

/*

Definição de uma estrutura do registro de cabeçalho, com os campos definidos conforme especificações. O campo "status",
que contém apenas um caracter que pode assumir '0' ou '1', é inicializado com 2 bytes para armazenamento do caracter '\0',
para controle interno do programa.

*/
typedef struct reg_cabecalho{

    char status[2];
    int topo;
    int proxRRN;
    int nroRegRem;
    int nroPagDisco;
    int qttCompacta;
  
}reg_cabecalho;

typedef struct reg_cabecalho_arvore{

    char status[2];
    int noRaiz;
    int nroChavesTotal;
    int alturaArvore;
    int RRNproxNo;
} reg_cabecalho_arvore;

void print_reg_dados_indice(reg_dados_indice* reg);

reg_cabecalho* cria_registro_cabecalho();
reg_dados* cria_registro_dados();
reg_cabecalho_arvore* cria_registro_cabecalho_arvore();
reg_dados_indice* cria_registro_dados_indice();

#endif
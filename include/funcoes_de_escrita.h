#ifndef __ESCRITA_H__
#define __ESCRITA_H__

#include "topologiaRede.h"
#include "funcoes_auxiliares.h"
#include "funcoes_checagem.h"

//escreve no arquivo
void escreve_string_no_arquivo(char*, FILE*, int);
void escrever_no_arquivo_dados(FILE*, reg_dados*);
void escrever_no_arquivo_cabecalho(FILE*, reg_cabecalho*);
void escrever_no_arquivo_cabecalho_arvore(FILE* arquivo, reg_cabecalho_arvore* reg);
void atualizar_reg_cabecalho(reg_cabecalho*, FILE*, int*);
int insere_registro_dados(FILE* arquivo, reg_dados* novo_reg_dados, reg_cabecalho* novo_reg_cabecalho, int* num_registros_total);
//int insere_registro_dados(FILE* arquivo_entrada, reg_cabecalho *h,reg_dados* rd);

//escreve na tela
void printa_registro(reg_dados*);
void printHeader(reg_cabecalho* h);
#endif
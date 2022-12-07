// esse arquivo contera todas as funcoes de leitura e escrita(rw) ou de input e output(io)
// relacionadas a arvore b e suas funcionalidades

#ifndef BTREERW_H
#define BTREERW_H
#include "topologiaRede.h"
#include "funcoes_de_escrita.h"
#include "btree_rw_e_io.h"

void escrever_no_arquivo_cabecalho_arvore(FILE*, reg_cabecalho_arvore*);

//escrita do indice
void escrever_dados_indice(FILE* arquivo, reg_dados_indice* reg);
void escrever_dados_indice_porRRN(FILE* arquivo, int RRN, reg_dados_indice* reg);

//leitura do indice
void ler_dados_indice(FILE* arquivo, reg_dados_indice* reg);
void ler_dados_indice_porRRN(FILE* arquivo, int RRN, reg_dados_indice* reg);
void ler_reg_cabecalho_arvore(FILE* arquivo, reg_cabecalho_arvore* reg);

void printHeaderArvore(reg_cabecalho_arvore* h_btree);
void printa_registro_arvore(reg_dados_indice *btree_reg);

#endif
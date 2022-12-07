// esse arquivo contera todas as funcoes que operam na estrutura da arvore b
#ifndef BTREE_H
#define BTREE_H
#include "topologiaRede.h"
#include "btree_rw_e_io.h"
#include "funcoes_de_print.h"

#define PROMOTION 1
#define NO_PROMOTION 0
#define INSERTION_ERROR -1



int busca_arvore(reg_cabecalho_arvore* reg, int* pos, reg_dados_indice* reg_arvore_encontrado, int chave, FILE* arq, int* num_paginas_lidas);
int _busca_arvore(int RRN, int *pos, reg_dados_indice* reg_arvore_encontrado, int chave, FILE* arq, int* num_paginas_lidas);

void insercao_btree(FILE*fp,reg_cabecalho_arvore*h,int key, int data_rrn_4insertion);
int _insercao_btree(FILE* fp,reg_cabecalho_arvore* h, int rrn_indice_atual, int key, int data_rrn_4insertion, int* promoted_child, int* promoted_key, int* promoted_data_rrn);
int insere_na_pagina(reg_dados_indice* r,int insert_key, int insert_data_rrn, int insert_child);
int busca_na_pagina(int key, int* pos, reg_dados_indice* r);
int busca_indexada(int valor, int* num_registros_encontrados, reg_cabecalho_arvore *novo_reg_cabecalho_arvore, reg_dados_indice *novo_reg_encontrado, FILE* arquivo_dados, FILE* arquivo_indice, reg_dados *novo_reg_dados, int* num_paginas_lidas);
void split(int key, int data_rrn,int right_child, int* upkey, int* updata_rrn, int*upchild, reg_dados_indice *r, reg_dados_indice *newr );

#endif
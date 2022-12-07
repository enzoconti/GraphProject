#ifndef __PRINT_H__
#define __PRINT_H__

#include "topologiaRede.h"

#define ERRO_FALHA_PROCESSAMENTO_ARQUIVO "Falha no processamento do arquivo.\n"
#define ERRO_REG_INEXISTENTE "Registro inexistente.\n\n"
#define BUSCA "Busca %d\n"
#define NUM_PAG_DISCO "Numero de paginas de disco: %d\n\n"

void print_busca(int buscas);
void print_num_pag_discos(int num_pag_disco);
void print_falha_processamento_arquivo();
void print_registro_inexistente();
void printa_registro(reg_dados* reg);
void printa_registro_juncao(reg_dados* reg1, reg_dados* reg2);

#endif
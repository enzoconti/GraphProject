#ifndef __ABERTURA_H__
#define __ABERTURA_H__

#include "topologiaRede.h"
#include "funcoes_de_print.h"

FILE* abrir_leitura(char*);
FILE* abrir_leitura_binario(char*);
FILE* abrir_leitura_binario_grafo(char*);
FILE* abrir_escrita_binario(char*);
FILE* abrir_leitura_e_escrita_binario(char*);
FILE* abrir_escrita_binario_cria_novo_arquivo(char*);

#endif
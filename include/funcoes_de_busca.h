#ifndef __BUSCA_H__
#define __BUSCA_H__

#include "topologiaRede.h"
#include "funcoes_fornecidas.h"
#include "funcoes_de_escrita.h"
#include "funcoes_de_leitura.h"
#include "funcoes_de_print.h"

//compara inteiros
int compara_campo_inteiro(int, int, reg_dados*);
int busca_inteiro(reg_dados*, FILE*, int, int*, int);

//compara strings
int compara_campo_string(int, char*, reg_dados* );
int busca_string(reg_dados*, FILE*, int, int*, char*);

#endif
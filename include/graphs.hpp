#ifndef GRAPHS_H
#define GRAPHS_H
#include <iostream>
#include <list>
#include <vector>
#include "topologiaRede.h"
using namespace std;

typedef struct vertice{
    int idConecta;
    char nomePoPs[TAM_MAX_CAMPO_VARIAVEL];
    char nomePais[TAM_MAX_CAMPO_VARIAVEL];
    char siglaPais[3];
}vertice;

typedef struct aresta{
    vertice saida;
    vertice entrada;
    int peso;
}aresta;

typedef struct adj_list{
    vertice v;
    list<aresta> lista;
}adj_list;

void cria_vertice_reg(reg_dados* reg, vertice* v);
void cria_aresta(aresta* a,vertice vsai, vertice ventra, int peso);
void cria_lista_adj(adj_list* lista, vertice* vin);
int insere_adj_list_no_grafo(adj_list* al, vector<adj_list> graph_als);
void insere_aresta_atual(list<aresta> l, aresta* a);
void swap_aresta(aresta* a);

#endif
#ifndef GRAPHS_H
#define GRAPHS_H
#include <iostream>
#include <list>
#include <vector>
#include <map>
#include "topologiaRede.h"
#include "funcoes_de_leitura.h"
using namespace std;

typedef struct vertice{
    int idConecta;
    char nomePoPs[TAM_MAX_CAMPO_VARIAVEL];
    char nomePais[TAM_MAX_CAMPO_VARIAVEL];
    char siglaPais[3];
}vertice;

typedef struct aresta{
    vertice *origem;
    vertice *destino;
    double peso;
}aresta;

typedef struct adj_list{
    vertice v;
    map<int,aresta> lista_de_arestas;
}adj_list;


void cria_vertice_reg(reg_dados* reg, vertice& v);
void cria_aresta(aresta& a,vertice *v_origem, vertice *v_destino,double peso);
void cria_lista_adj(adj_list* lista, vertice* vin);
void zera_lista_adj(adj_list& lista);
int insere_adj_list_no_grafo(adj_list al, vector<adj_list>& graph_als);
//int insere_adj_list_no_grafo(adj_list* al, vector<adj_list> graph_als);
void insere_aresta_atual(list<aresta>& l, aresta& a);
void swap_aresta(aresta& a);
map<int,adj_list> cria_grafo_do_binario(FILE* fp);

#endif
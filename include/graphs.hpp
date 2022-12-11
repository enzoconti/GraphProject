#ifndef GRAPHS_H
#define GRAPHS_H
#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <limits>
#include <set>
#include <algorithm>

#include "topologiaRede.h"
#include "funcoes_de_leitura.h"
using namespace std;

enum COR {branco, cinza, preto};
enum classificacao{sem_classificacao, arvore, retorno};

typedef struct vertice{
    int idConecta;
    char nomePoPs[TAM_MAX_CAMPO_VARIAVEL];
    char nomePais[TAM_MAX_CAMPO_VARIAVEL];
    char siglaPais[3];
    COR classificacao_vertice=branco;
}vertice;

typedef struct aresta{
    vertice *origem;
    vertice *destino;
    double peso;
    classificacao classificacao_aresta=sem_classificacao;
}aresta;

typedef struct adj_list{
    vertice v;
    map<int,aresta> lista_de_arestas;
}adj_list;

typedef struct grafo_em_struct{
    int nro_arestas=0;
    int nro_vertices=0;
    map<int, adj_list> map_do_grafo;
}GRAFO;


void cria_vertice_reg(reg_dados* reg, vertice& v);
void cria_aresta(aresta& a,vertice *v_origem, vertice *v_destino,double peso);
void cria_lista_adj(adj_list* lista, vertice* vin);
void zera_lista_adj(adj_list& lista);
int insere_adj_list_no_grafo(adj_list al, vector<adj_list>& graph_als);
//int insere_adj_list_no_grafo(adj_list* al, vector<adj_list> graph_als);
void insere_aresta_atual(list<aresta>& l, aresta& a);
void swap_aresta(aresta& a);
GRAFO cria_grafo_do_binario(FILE* fp);
void dijkstra(map<int, adj_list>& grafo, map<int, double>& distancias, map<int, int>& antecessores, int chave_origem);
int busca_em_profundidade(GRAFO& grafo);
void _busca_em_profundidade(GRAFO& grafo, int chave, int& numero_arestas_arvore);
//int min_value(list<int> S);

#endif
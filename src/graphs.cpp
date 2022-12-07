#include "../include/graphs.hpp"

void cria_vertice_reg(reg_dados* reg, vertice* v){
    v->idConecta = reg->idConecta;
    strcpy(v->nomePais,reg->nomePais);
    strcpy(v->nomePoPs,reg->nomePoPs);
    strcpy(v->siglaPais,reg->siglaPais);
}

void cria_lista_adj(adj_list* lista, vertice* vin){
    lista = (adj_list*) calloc(1,sizeof(adj_list));
    lista->v = *vin;
}

int insere_adj_list_no_grafo(adj_list* al, vector<adj_list> graph_als){
    int pos;
    // busca posicao para insercao
    for(int i=0;i < graph_als.size();i++){
        if( al->v.idConecta <= graph_als[i].v.idConecta){
            pos = i;
            break;
        }
    }
    

    for(int i=graph_als.size(); i>=pos;i++){
        graph_als[i + 1] = graph_als[i];
    }
    graph_als[pos] = *al;

    return pos;
}

void cria_aresta(aresta* a,vertice vsai, vertice ventra, int peso){
    a = (aresta*) calloc(1,sizeof(aresta));
    a->entrada = ventra;
    a->saida = vsai;
    a->peso = peso;
}

void swap_aresta(aresta* a){
    vertice tmp;
    tmp = a->entrada;
    a->entrada = a->saida;
    a->saida = tmp;
}


void insere_aresta_atual(list<aresta> l, aresta* a){
    list<aresta>::iterator it;
    for(it=l.begin(); it != l.end(); it++){
        if(a->saida.idConecta <= it->saida.idConecta){
            l.insert(it,*a);
            break;
        }
    }
}
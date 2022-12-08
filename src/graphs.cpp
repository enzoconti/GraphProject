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

void zera_lista_adj(adj_list& lista){
    lista.lista.clear();
    lista.v.idConecta = -1;
    lista.v.nomePais[0] = '\0';
    lista.v.nomePoPs[0] = '\0';
    lista.v.siglaPais[0] = '\0';
}

int insere_adj_list_no_grafo(adj_list& al, vector<adj_list>& graph_als){
    int pos=0;
    vector<adj_list>::iterator it;
    // busca posicao para insercao
    //debug//printf("about to start insere_adj_list_no_grafo loop\n");
    for(it = graph_als.begin();it != graph_als.end();it++){
        //printf("comparing idConectas to insert with pos=%d\n",pos);
        if( al.v.idConecta <= it->v.idConecta){
            graph_als.insert(it,al);
            //debug//printf("has inserted and now returning pos=%d\n",pos);
            return pos;
        }
        pos++;
    }
    //debug//printf("got outside of insere_adj_list_no_grafo loop\n");
    graph_als.push_back(al);
    //debug//printf("has pushed back\n");
    return graph_als.size() - 1;
}

void cria_aresta(aresta& a,vertice vsai, vertice ventra, double peso){
    a.entrada = ventra;
    a.saida = vsai;
    a.peso = peso;
}

void swap_aresta(aresta& a){
    vertice tmp;
    tmp = a.entrada;
    a.entrada = a.saida;
    a.saida = tmp;
}


void insere_aresta_atual(list<aresta>& l, aresta& a){
    list<aresta>::iterator it;
    for(it=l.begin(); it != l.end(); it++){
        if(a.saida.idConecta <= it->saida.idConecta){
            l.insert(it,a);
            return;
        }
    }
    l.push_back(a);
}
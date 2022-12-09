#include "../include/graphs.hpp"

void cria_vertice_reg(reg_dados* reg, vertice& v){
    v.idConecta = reg->idConecta;
    strcpy(v.nomePais,reg->nomePais);
    strcpy(v.nomePoPs,reg->nomePoPs);
    strcpy(v.siglaPais,reg->siglaPais);
}

void cria_lista_adj(adj_list* lista, vertice* vin){
    lista = (adj_list*) calloc(1,sizeof(adj_list));
    lista->v = *vin;
}

void zera_lista_adj(adj_list& lista_a_zerar){
    //lista_a_zerar.lista.clear();
    lista_a_zerar.v.idConecta = -1;
    lista_a_zerar.v.nomePais[0] = '\0';
    lista_a_zerar.v.nomePoPs[0] = '\0';
    lista_a_zerar.v.siglaPais[0] = '\0';
}

int insere_adj_list_no_grafo(adj_list al, vector<adj_list>& graph_als){
    int pos=0;
    
    auto it = graph_als.begin();
    // busca posicao para insercao
    //debug//printf("about to start insere_adj_list_no_grafo loop\n");
    for(;it != graph_als.end();it++){
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
    
    /*
   for(adj_list adj_list_tmp : graph_als){
        if(al.v.idConecta <= adj_list_tmp.v.idConecta){
            graph_als.insert(adj_list_tmp,al);
            return pos;
        }

        pos++
   }
   */

   graph_als.push_back(al);
   return graph_als.size() - 1;
}

void cria_aresta(aresta& a,vertice *v_origem, vertice *v_destino, double peso){
    a.destino = v_destino;
    a.origem = v_origem;
    a.peso = peso;
}

void swap_aresta(aresta& a){
    vertice *tmp;
    tmp = a.destino;
    a.destino = a.origem;
    a.origem = tmp;
}


void insere_aresta_atual(list<aresta>& l, aresta& a){
    
    auto it = l.begin();
    for(; it != l.end(); it++){
        if(a.origem->idConecta <= it->origem->idConecta){
            l.insert(it,a);
            return;
        }
    }
    l.push_back(a);
}

map<int,adj_list> cria_grafo_do_binario(FILE* fp){
  reg_dados *reg = cria_registro_dados();
  int rrn_lido, posicao_inserida;
  vertice vertice_atual, vertice_conectado;
  adj_list lista_adj_atual, lista_adj_conectada;
  aresta aresta_atual;
  double velocidade_tmp;
  map<int, adj_list> grafo;

    while (le_arquivo(reg, fp, &rrn_lido) != 0){
    // debug//printf("has readen reg_dados as:\n");
    // debug//printa_registro(reg);
    //  criar verticeAtual com reg_dados
    // debug//printf("creating vertice_atual with cria_vertice_reg\n");
    cria_vertice_reg(reg, vertice_atual);
    // criar adj_list alAtual com verticeAtual
    // debug//printf("creating lista_adj_atual with cria_lista_ad\nj");
    lista_adj_atual.v = vertice_atual;

    // debug//printf("inserting lista_adj_atual on graph\n");
    auto insert_flag = grafo.insert(pair<int, adj_list>(lista_adj_atual.v.idConecta, lista_adj_atual));
    if (insert_flag.second == false)
    { // already existed because we inserted it but it is not yet complete
      grafo.at(lista_adj_atual.v.idConecta).v = lista_adj_atual.v;
    }
    // grafo.insert({lista_adj_atual.v.idConecta, lista_adj_atual});

    vertice_conectado.idConecta = reg->idPoPsConectado;
    lista_adj_conectada.v = vertice_conectado;
    if (vertice_conectado.idConecta != -1)
    { // so adicionamos as arestas que
      grafo.insert(pair<int, adj_list>(lista_adj_conectada.v.idConecta, lista_adj_conectada));

      velocidade_tmp = (double)reg->velocidade;
      if (reg->unidadeMedida[0] == 'M')
        velocidade_tmp /= 1024;
      cria_aresta(aresta_atual, &grafo.at(lista_adj_atual.v.idConecta).v, &grafo.at(vertice_conectado.idConecta).v, velocidade_tmp);

      // debug//printf("inserting aresta_atual on graph\n");
      grafo.at(aresta_atual.origem->idConecta).lista_de_arestas.insert({aresta_atual.destino->idConecta, aresta_atual});
      swap_aresta(aresta_atual); // troca destino com origem
      // debug//printf("inserting aresta_atual_swapped on graph\n");
      grafo.at(aresta_atual.origem->idConecta).lista_de_arestas.insert({aresta_atual.destino->idConecta, aresta_atual});
    }
  }

    return grafo;
}
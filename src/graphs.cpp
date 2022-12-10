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
        if (vertice_conectado.idConecta != -1){ // so adicionamos as arestas que
        grafo.insert(pair<int, adj_list>(lista_adj_conectada.v.idConecta, lista_adj_conectada));

        velocidade_tmp = (double)reg->velocidade;
        if (reg->unidadeMedida[0] == 'G') velocidade_tmp *= 1024;
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

/**
 * @brief Função responsável por encontrar o menor caminho entre um vértice de origem de um grafo, para todos outros vértices
 * do grafo
 * 
 * @param grafo map de listas adjacentes que representa o grafo como um todo
 * @param distancias map que guarda as distancias aos vertices em relação à origem
 * @param antecessores map que guarda os antecessores de cada vertice 
 * @param chave_origem chave que representa o vertice de origem para o algoritmo
 */
void dijkstra(map<int, adj_list>& grafo, map<int, double>& distancias, map<int, int>& antecessores, int chave_origem){

    list<int> nao_visitados;//guarda os vertices ainda não visitados
    int cont = 0;

    int menor_chave_nao_visitada;//marca o próximo vertice a ser visitado
    double INF = 9999999999;

    map<int, adj_list>:: iterator grafo_iterator;

    //enquanto não acabar o grafo, insere a key do distancias como idConecta e o valor da distância 99999
    for(grafo_iterator = grafo.begin(); grafo_iterator != grafo.end(); grafo_iterator++){
        distancias.insert({grafo_iterator->first, INF});//distancias iniciais a todos vertices
        antecessores.insert({grafo_iterator->first, -1});//antecessores dos vertices
        nao_visitados.push_back(grafo_iterator->first);//idConecta dos não visitados
    }
    nao_visitados.sort();//ordena lista primeira vez

    distancias.at(chave_origem) = 0;//iguala-se o dado, isto é, distância

    set<int> visitados;//marca os vertices visitados

    while(!nao_visitados.empty()){//enquanto não acabar os vertices não visitados
        menor_chave_nao_visitada = nao_visitados.front();//define o menor elemento
        nao_visitados.pop_front();//remove vertice da fila
        visitados.insert(menor_chave_nao_visitada);//insere no visitados o ultimo vertice visitado

        map<int, double>::iterator teste_distancias;
        
        map<int, aresta>::iterator aresta_iterator;//Define um iterador de arestas

        //Utiliza o iterador de arestas para iterar pelas arestas da lista de adjacências do vertice {menor_chave_nao_visitada}
        for(aresta_iterator = grafo.at(menor_chave_nao_visitada).lista_de_arestas.begin(); aresta_iterator != grafo.at(menor_chave_nao_visitada).lista_de_arestas.end(); aresta_iterator++){

            //Verifica se a distância imediata do vertice destino da aresta iterada é maior do que a distância do vertice {menor_chave_nao_visitada} + o peso da aresta iterada (dá a distância por um caminho através da menor chave não visitada)
            if(distancias.at(aresta_iterator->second.destino->idConecta) > (distancias.at(menor_chave_nao_visitada) + aresta_iterator->second.peso)){

                //Altera a distância imediata do vertice destino da aresta iterada para a nova menor distância (passando pelo vértice {menor_chave_nao_visitada})
                distancias.at(aresta_iterator->second.destino->idConecta) = (distancias.at(menor_chave_nao_visitada) + aresta_iterator->second.peso);

                //Adiciona a menor chave não visitada como antecessor do vertice destino da aresta iterada (como o caminho é através do vertice menor chave nao visitada, o mapa de antecessores explicita que a nova menor distância da aresta iterada passa por {menor_chave_nao_visitada})
                antecessores.at(aresta_iterator->second.destino->idConecta) = menor_chave_nao_visitada;
            }
        }

        printf("%d° preenchimento de distâncias:\n", cont+1);
        for(teste_distancias = distancias.begin(); teste_distancias != distancias.end(); teste_distancias++){
            printf("Vértice: %d ", teste_distancias->first);
            printf("Distancia: %.0lf\n\n", teste_distancias->second);
        }
        cont++;
    }
}
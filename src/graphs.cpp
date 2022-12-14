#include "../include/graphs.hpp"


/**
 * @brief funcao simples que cria vertice alocado a partir das informacoes de um registro de dados
 * 
 * @param reg registro de dados lido do arquivo e salvo em RAM
 * @param v vertice que representa o registor lido
 */
void cria_vertice_reg(reg_dados* reg, vertice& v){
    v.idConecta = reg->idConecta;
    strcpy(v.nomePais,reg->nomePais);
    strcpy(v.nomePoPs,reg->nomePoPs);
    strcpy(v.siglaPais,reg->siglaPais);
    v.classificacao_vertice = branco; // todos vertices comecam como brancos(nao descobertos)
}


/**
 * @brief funcao simples que cria uma aresta ja alocada dado um par ordenado de vertices e um peso
 * 
 * @param a aresta a ser definida
 * @param v_origem vertice de origem
 * @param v_destino vertice de destino
 * @param peso peso da aresta
 */
void cria_aresta(aresta& a,vertice *v_origem, vertice *v_destino, double peso){
    a.destino = v_destino;
    a.origem = v_origem;
    a.peso = peso;
    a.classificacao_aresta = sem_classificacao; // todas arestas sao padronizadas como sem_classificacao ao serem criadas
}

/**
 * @brief funcao simples que swappa a origem e o destino de uma aresta
 * 
 * @param a aresta onde ocorrera o swap
 */
void swap_aresta(aresta& a){
    vertice *tmp;
    tmp = a.destino;
    a.destino = a.origem;
    a.origem = tmp;
}

/**
 * @brief funcao que cria a estrutura de um grafo a partir de um arquivo binario
 * 
 * @param fp ponteiro para o arquivo ja aberto a ser lido
 * @return GRAFO 
 */
GRAFO cria_grafo_do_binario(FILE* fp){

    reg_dados *reg = cria_registro_dados();
    int rrn_lido, posicao_inserida;
    vertice vertice_atual, vertice_conectado;
    adj_list lista_adj_atual, lista_adj_conectada;
    aresta aresta_atual;
    double velocidade_tmp;  
    GRAFO grafo;


    while (le_arquivo(reg, fp, &rrn_lido) != 0){
        cria_vertice_reg(reg, vertice_atual);
        lista_adj_atual.v = vertice_atual;

        auto insert_flag = grafo.map_do_grafo.insert(pair<int, adj_list>(lista_adj_atual.v.idConecta, lista_adj_atual));
        if (insert_flag.second == false){ 
        // se tentamos inserir e nao conseguimos(como os arquivos nao tem chaves repetidas) eh pq ja haviamos inserido
        // mas, no nosso algoritmo, isso significa que inserimos a chave e as arestas sem ter todas informacoes sobre o vertice(poi so registro ainda nao havia sido lido)
        // entao, atualizamos o vertice com todas as informacoes do registro

            grafo.map_do_grafo.at(lista_adj_atual.v.idConecta).v = lista_adj_atual.v;
        }
        
        // criamos incompletamente o vertice que esta conectado com o vertice que foi lido do registro
        vertice_conectado.idConecta = reg->idPoPsConectado;
        lista_adj_conectada.v = vertice_conectado;
        if (vertice_conectado.idConecta != -1){ // so adicionamos os vertices conectados que tem valor nao nulo de chave
            grafo.map_do_grafo.insert(pair<int, adj_list>(lista_adj_conectada.v.idConecta, lista_adj_conectada));
            grafo.nro_vertices = grafo.nro_vertices + 1; 

            velocidade_tmp = (double)reg->velocidade;
            if (reg->unidadeMedida[0] == 'G') velocidade_tmp *= 1024; // converte tudo para Mbps

            // cria a aresta com a origem e destino e a insere
            cria_aresta(aresta_atual, &grafo.map_do_grafo.at(lista_adj_atual.v.idConecta).v, &grafo.map_do_grafo.at(vertice_conectado.idConecta).v, velocidade_tmp);


            // insere a aresta na adj_list do vertice de origem
            // i.e, buscamos a chave do vertice de origem da aresta_atual e inserimos em sua lista_de_arestas o par formado pela chave do vertice de destino e a aresta_atual
            grafo.map_do_grafo.at(aresta_atual.origem->idConecta).lista_de_arestas.insert({aresta_atual.destino->idConecta, aresta_atual});
            grafo.nro_arestas = grafo.nro_arestas + 1;

            swap_aresta(aresta_atual); // troca destino com origem
            
            // repete a mesma insercao, mas com destino e origem trocados
            // ou seja, se antes inserimos a aresta (x,y) na lista de adjacencias do vertica x, agora inserimos (y,x) na lista de adjacenca do vertice y

            grafo.map_do_grafo.at(aresta_atual.origem->idConecta).lista_de_arestas.insert({aresta_atual.destino->idConecta, aresta_atual});
        }
    }

    // retornamos o grafo construido
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
/**
 * @brief funcao master que comanda a busca_em_profundidade, chama a funcao recursiva da busca em profundidade cada vez que encontra um vertice branco
 * 
 * @param grafo grafo a ser performada a busca
 * @return int numero de ciclos no grafo(que eh igual ao numero de arestas de retorno)
 */
int busca_em_profundidade(GRAFO& grafo){
    int numero_arestas_arvore=0; 

    // definimos um iterador que percorre o grafo todo e chama a usca em profundidade recursiva a cada vez que encontra um vertice branco
    // dessa forma, garante-se que todas possiveis partes desconexas do grafo serao percorridass
    for(auto it= grafo.map_do_grafo.begin(); it != grafo.map_do_grafo.end(); it++){
        if(it->second.v.classificacao_vertice == branco){
            _busca_em_profundidade(grafo, it->second.v.idConecta, numero_arestas_arvore);
        }
    }

    // o numero de ciclos que deve ser retornado eh igual ao numero total de arestas menos o numero de arestas de arvore, ja que o grafo eh nao direcionado
     return grafo.nro_arestas - numero_arestas_arvore;
}

/**
 * @brief funcao recursiva que realiza a busca em profundidade no grafo, percorrendo vertice a vertice em cada chamada
 * 
 * @param grafo grafo a ser performada a busca
 * @param chave chave do vertice atual(a partir da qual este eh buscado no map do grafo)
 * @param numero_arestas_arvore contador de arestas de arvore acumulado durante a busca em profundidade
 */
void _busca_em_profundidade(GRAFO& grafo, int chave, int& numero_arestas_arvore){
    // comecamos setando o vertice em que estamos como cinza, ja que ele foi descoberto
    grafo.map_do_grafo.at(chave).v.classificacao_vertice = cinza; 
    

    // definimos um iterador que percorre a lista de arestas do vertice atual
    for(auto iterador_arestas = grafo.map_do_grafo.at(chave).lista_de_arestas.begin(); iterador_arestas != grafo.map_do_grafo.at(chave).lista_de_arestas.end(); iterador_arestas++ ){
        // se o destino daquela aresta nao for um vertice preto(indicando que a aresta ainda nao foi percorrida), visitamos esse vertice de destino
        if(grafo.map_do_grafo.at(iterador_arestas->first).v.classificacao_vertice == branco){

            // antes de visitar o vertice nao_visitado(classificacao == branco) precisamos atualizar a classificacao da aresta que estamos percorrendo pela primeira vez
            // so atualizamos a classificacao da aresta se a aresta ainda nao tem classificacao
            // o acesso desse if eh feito da seguinte forma: acessa-se a lista_de_arestas do adj_list com chave igual a chave da origem do iterador de arestas
            // nessa lista, busca-se a aresta com chave igual ao destino do iterador de arestas
            
            if(grafo.map_do_grafo.at(iterador_arestas->second.origem->idConecta).lista_de_arestas.at(iterador_arestas->second.destino->idConecta).classificacao_aresta == sem_classificacao){

                // como o vertice de destino eh branco, classifica-se como aresta de arvore
                // o acesso da aresta eh identico ao do if anterior, so reescreve o "sem_classificacao" para "arvore"
                grafo.map_do_grafo.at(iterador_arestas->second.origem->idConecta).lista_de_arestas.at(iterador_arestas->second.destino->idConecta).classificacao_aresta = arvore;
                numero_arestas_arvore = numero_arestas_arvore + 1;

            // atualizada a aresta, chama-se a recursao nesse vertice de destino, com o mesmo grafo e contador de ciclos
            _busca_em_profundidade(grafo, iterador_arestas->second.destino->idConecta, numero_arestas_arvore);
            }
        }
    }
    // apos percorrer toda a lista de adjacencias do vertice chave, finaliza-se reescrevendo-o como preto
    grafo.map_do_grafo.at(chave).v.classificacao_vertice = preto;    

    return ;
}

void busca_em_largura(GRAFO& grafo, int inicio){
    // primeiro definimos o vertice de inicio como visitado
    grafo.map_do_grafo.at(inicio).v.classificacao_vertice = cinza;
    list<int> fila_chaves; // uma fila das chaves que serao processadas ao longo do algoritmo

    int chave_a_ser_processada; // a chave sendo processada em cada iteracao
    
    fila_chaves.push_back( inicio ); // adicionamos a chave inicial como primeiro elemento da fila

    while(fila_chaves.empty() == false){
        // enquanto a fila nao for vazia, consultamos e removemos o primeiro valor da fila (o que foi inserido ha mais tempo, ja que a insercao eh feita sempre no final)
        chave_a_ser_processada = fila_chaves.front();
        fila_chaves.pop_front();

        // iterador_arestas percorre toda lista de adjacencias da chave sendo processada nessa iteracao
        for(auto iterador_arestas = grafo.map_do_grafo.at(chave_a_ser_processada).lista_de_arestas.begin(); iterador_arestas != grafo.map_do_grafo.at(chave_a_ser_processada).lista_de_arestas.end(); iterador_arestas++){
            // se o vertice destino daquela aresta iterada nao tiver sido percorrido, o marcamos como percorrido e inserimos no final da fila
            if(iterador_arestas->second.destino->classificacao_vertice == branco){
                iterador_arestas->second.destino->classificacao_vertice = cinza;
                fila_chaves.push_back(iterador_arestas->second.destino->idConecta);
            }
        }

        // apos inserir todos vertices adjacentes na fila, reclassificamos o vertice da chave atual como totalmente percorrido
        grafo.map_do_grafo.at(chave_a_ser_processada).v.classificacao_vertice = preto;

    }

}
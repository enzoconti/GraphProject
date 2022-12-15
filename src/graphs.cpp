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
 * @brief funcao que cria um map de capacidades ja alocado iniciando-o com os pesos das arestas de um grafo
 * 
 * @param g grafo em questao
 * @param capacidade map de capacidade (retornado por referencia)
 */
void cria_map_capacidade(GRAFO g, map<pair<int,int>, double>& capacidade){
    for(auto it = g.map_do_grafo.begin(); it != g.map_do_grafo.end(); it++){
        for(auto it_arestas = it->second.lista_de_arestas.begin();it_arestas != it->second.lista_de_arestas.end() ; it_arestas++){
            capacidade.insert({{it_arestas->second.origem->idConecta, it_arestas->second.destino->idConecta}, it_arestas->second.peso});
        }
    }
}

/**
 * @brief funcao que cria um map de cores de vertice(que indicam se ja foram visitados) a partir de um grafo iniciando todos vertices dados como brancos(nao visitado)
 * 
 * @param grafo 
 * @param cor_vertices map de cores de vertice (retornado por referencia)
 */
void cria_cor_vertices(GRAFO grafo, map<int, COR>& cor_vertices){
    
    for(auto it = grafo.map_do_grafo.begin(); it != grafo.map_do_grafo.end(); it++){
        cor_vertices.insert({it->first, branco});
    }

}
    


/**
 * @brief Função responsável por encontrar o menor caminho entre um vértice de origem de um grafo, para outro vértice
 * do grafo
 * 
 * @param grafo map de listas adjacentes que representa o grafo como um todo
 * @param chave_origem chave que representa o vertice de origem para o algoritmo
 * @param chave_destino chave que representa o vertice de destino para o algoritmo
 */
double dijkstra(map<int, adj_list>& grafo, int chave_origem, int chave_destino){

    const double INF = numeric_limits<double>::infinity();//declara infinito
    double distancia = 0.0;

    map<int, double> dist_map;//map de distâncias para os vértices

    //distâncias desconhecidas para cada vértice = Infinito
    for(auto pair:grafo){
        auto key = pair.first;
        auto value = pair.second;
        dist_map[key] = INF;
    }

    //distância origem é 0
    dist_map[chave_origem] = 0;

    //priority queue organiza automaticamente em função dos vértices com menor peso
    //usa fila pois parte do princípio de uma bfs
    priority_queue<pair<double,int>, vector<pair<double, int>>, greater<pair<double,int>>>pq;

    pq.push({0.0, chave_origem});

    while(!pq.empty()){
        auto dist = pq.top().first;
        auto vertice = pq.top().second;
        pq.pop();

        //sai do loop se conferindo distâncias além do vértice destino
        if(vertice == chave_destino){
            break;
        }

        //checa se já existe uma maneira melhor de alcançar o objetivo
        if(dist_map[vertice]!=dist){
            continue;
        }

        for(auto pair : grafo[vertice].lista_de_arestas){
            auto key = pair.first;
            auto value = pair.second;

            if(dist_map.at(key) > value.peso + dist){
                //atualiza menor distância
                dist_map[key] = value.peso + dist;
                pq.push({dist_map[key], key});
            }
        }
    }

    return dist_map[chave_destino];

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

/**
 * @brief funcao que opera uma busca em largura modificada para o algoritmo de edmon karp - considerando as capacidades de fluxo de cada aresta para que o edmon_karp possa calcular o fluxo maximo por caminhos distintos
 * 
 * @param grafo grafo em questao
 * @param inicio chave do vertice de inicio do fluxo
 * @param destino_final chave do vertice de destino do fluxo
 * @param caminho map que guarda o caminho feito pela busca em largura, para que o edmon karp possa alterar as capacidades de fluxo das arestas ja percorridas
 * @param capacidade map que guarda as capacidades de cada aresta(iniciam em peso e sao decrementadas conforme o algoritmo edmon karp)
 * @return double 
 */
double busca_em_largura(GRAFO& grafo, int inicio,int destino_final, map<int,int>& caminho, map<pair<int,int>, double> capacidade){
    const double INF = numeric_limits<double>::infinity();//declara infinito


    double fluxo_atual, novo_fluxo;

    // map que contem a cor(informacao se foi visitado) dos vertices
    map<int, COR> cor_vertices;
    cria_cor_vertices(grafo, cor_vertices);

    // primeiro definimos o vertice de inicio como visitado
    cor_vertices.at(inicio) = cinza;


    list<pair<int,double>> fila_chaves_fluxos; // uma fila das chaves que serao processadas ao longo do algoritmo(1o valor) com seus respectivos fluxos ate aqui(2o valor)

    int chave_a_ser_processada; // a chave sendo processada em cada iteracao
    
    fila_chaves_fluxos.push_back( {inicio, INF } ); // adicionamos a chave inicial como primeiro elemento da fila

    while(fila_chaves_fluxos.empty() == false){
        // enquanto a fila nao for vazia, consultamos o primeiro valor da fila, salvando sua chave e valor de fluxo
        chave_a_ser_processada = fila_chaves_fluxos.front().first;
        fluxo_atual = fila_chaves_fluxos.front().second;

        // removemos o primeiro valor da fila (o que foi inserido ha mais tempo, ja que a insercao eh feita sempre no final)
        fila_chaves_fluxos.pop_front();
        

        // iterador_arestas percorre toda lista de adjacencias da chave sendo processada nessa iteracao
        for(auto iterador_arestas = grafo.map_do_grafo.at(chave_a_ser_processada).lista_de_arestas.begin(); iterador_arestas != grafo.map_do_grafo.at(chave_a_ser_processada).lista_de_arestas.end(); iterador_arestas++){
            // se o vertice destino daquela aresta iterada nao tiver sido percorrido E a capacidade nao for nula, o marcamos como percorrido e inserimos no final da fila
            

            // acessamos as estruturas da seguinte maneira:
            // pegamos o vertice destino da aresta(key do map de arestas, por isso ->first ) como chave do map de cor_vertices e conferimos se esta branco
            // pegamos a capacidade na key (origem, destino) da aresta iterada (usamos o ->first denovo para o destino e a origem eh chave_a_ser_processada)

            if(cor_vertices.at(iterador_arestas->first) == branco && capacidade.at({chave_a_ser_processada, iterador_arestas->first}) > 0){
                // inserimos o par entre a chave de origem(key do map) e destino(dado do map) no map de caminho
            
                auto flag_insercao = caminho.insert({iterador_arestas->first,  chave_a_ser_processada});
                if(flag_insercao.second == false){
                    caminho.at({iterador_arestas->first}) = chave_a_ser_processada;
                }


                cor_vertices.at(iterador_arestas->first) = cinza;

                // o fluxo novo eh o minimo entre o fluxo atual e a capacidade recem consultada da nova aresta percorrida
                novo_fluxo = min(fluxo_atual, capacidade.at({chave_a_ser_processada, iterador_arestas->first}));

                if(iterador_arestas->first == destino_final){
                    return novo_fluxo;
                }

                fila_chaves_fluxos.push_back({iterador_arestas->first, novo_fluxo});
            }
        }
        
        // apos inserir todos vertices adjacentes na fila, reclassificamos o vertice da chave atual como totalmente percorrido
        cor_vertices.at(chave_a_ser_processada) = preto;
    }

    return 0;
}

/**
 * @brief funcao que implementa o algoritmo de edmon_karp para encontrar o fluxo maximo entre dois vertices de um grafo
 * 
 * @param grafo grafo em questao
 * @param origem chave da origem do fluxo
 * @param destino chave do destino do fluxo
 * @return double fluxo maximo entre esses dois vertices
 */
double edmon_karp_fluxo_maximo(GRAFO grafo, int origem, int destino){
    int fluxo_total = 0, fluxo_recebido;
    map<int,int> caminho;                 // map que retornara o caminho encontrado pela busca_em_largura
    map<pair<int,int>,double> capacidade; // map com chave como par ordenado (origem, destino) e dado double de capacidade
    cria_map_capacidade(grafo, capacidade); // map criado com capacidades iniciais = peso das arestas

    while(fluxo_recebido = busca_em_largura(grafo, origem, destino, caminho, capacidade)){ 
        fluxo_total += fluxo_recebido;

        int chave_iterada_caminho = destino; // chave que vai iterar sobre o caminho recebido alterando o map de capacidades
        
        // percorrendo o caminho ao contrario
        while(chave_iterada_caminho != origem){
            int chave_pai = caminho.at(chave_iterada_caminho);

            // acessa a aresta [chave_pai] -> [chave_iterada_caminho] que faz parte do caminho encontrado 
            capacidade.at({chave_pai ,chave_iterada_caminho}) -= fluxo_recebido; // decrementa o fluxo recebido da capacidade da aresta encontrada
        

            chave_iterada_caminho = chave_pai; // para iterar sobre o caminho
        }



    }

    return fluxo_total;
}

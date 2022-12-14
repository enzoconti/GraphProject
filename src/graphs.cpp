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
    a.capacidade = peso;
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
 * @brief Função responsável por encontrar o menor caminho entre um vértice de origem de um grafo, para outro vértice
 * do grafo
 * 
 * @param grafo map de listas adjacentes que representa o grafo como um todo
 * @param chave_origem chave que representa o vertice de origem para o algoritmo
 * @param chave_destino chave que representa o vertice de destino para o algoritmo
 */
double dijkstra(map<int, adj_list>& grafo, int chave_origem, int chave_destino){

    const double INF = numeric_limits<double>::infinity();//declara infinito

    map<int, double> dist_map;//map de distâncias para os vértices

    //distâncias desconhecidas para cada vértice = Infinito
    for(auto [chave, valor]:grafo){
        dist_map[chave] = INF;
    }

    //distância origem é 0
    dist_map[chave_origem] = 0;

    //priority queue organiza automaticamente em função dos vértices com menor peso
    //usa fila pois parte do princípio de uma bfs
    priority_queue<pair<double,int>, vector<pair<double, int>>, greater<pair<double,int>>>fila_prioridade;

    fila_prioridade.push({0.0, chave_origem});

    while(!fila_prioridade.empty()){
        //v representa um vertice
        auto [dist, v] = fila_prioridade.top();
        fila_prioridade.pop();

        //sai do loop se conferindo distâncias além do vértice destino
        if(v == chave_destino){
            break;
        }

        //checa se já existe uma maneira melhor de alcançar o objetivo
        if(dist_map[v]!=dist){
            continue;
        }

        //itera por todos vértices e listas de arestas do grafo, colocando a chave em chave, e uma aresta em (ar)
        for(auto [chave, ar] : grafo[v].lista_de_arestas){
            if(dist_map[chave] > ar.peso + dist){
                //atualiza menor distância
                dist_map[chave] = ar.peso + dist;
                fila_prioridade.push({dist_map[chave], chave});
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

double busca_em_largura(GRAFO& grafo, int inicio,int destino_final, map<int,int>& caminho){
    const double INF = numeric_limits<double>::infinity();//declara infinito

    double fluxo_atual, novo_fluxo;

    // primeiro definimos o vertice de inicio como visitado
    grafo.map_do_grafo.at(inicio).v.classificacao_vertice = cinza;
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
            // se o vertice destino daquela aresta iterada nao tiver sido percorrido, o marcamos como percorrido e inserimos no final da fila
            if(iterador_arestas->second.destino->classificacao_vertice == branco && iterador_arestas->second.capacidade != 0){
                // inserimos o par entre a chave de origem e destino no map de caminho
                //debug//printf("inserindo o elemento %d na chave %d do map de caminho\n", iterador_arestas->second.origem->idConecta, iterador_arestas->second.destino->idConecta);
                
                caminho.insert({iterador_arestas->second.destino->idConecta,  iterador_arestas->second.origem->idConecta});
                iterador_arestas->second.destino->classificacao_vertice = cinza;

                novo_fluxo = min(fluxo_atual, iterador_arestas->second.capacidade);

                if(iterador_arestas->second.destino->idConecta == destino_final){
                    return novo_fluxo;
                }

                fila_chaves_fluxos.push_back({iterador_arestas->second.destino->idConecta, novo_fluxo});
            }
        }
        
        // apos inserir todos vertices adjacentes na fila, reclassificamos o vertice da chave atual como totalmente percorrido
        grafo.map_do_grafo.at(chave_a_ser_processada).v.classificacao_vertice = preto;

    }

    return 0;
}

double edmon_karp_fluxo_maximo(GRAFO& grafo, int origem, int destino){
    int fluxo_total = 0, fluxo_recebido;
    map<int,int> caminho;

    while(fluxo_recebido = busca_em_largura(grafo, origem, destino, caminho)){
        fluxo_total += fluxo_recebido;

        int chave_iterada_caminho = destino; // chave que vai iterar sobre o caminho recebido alterando o map de capacidades
        
        // percorrendo o caminho ao contrario
        while(chave_iterada_caminho != origem){
            int chave_pai = caminho.at(chave_iterada_caminho);

            // acessa a aresta na adj_list de chave_pai com key chave_iterada_caminho
            // isto eh, a aresta [chave_pai] -> [chave_iterada_caminho] que faz parte do caminho encontrado 
            grafo.map_do_grafo.at(chave_pai).lista_de_arestas.at(chave_iterada_caminho).capacidade -= fluxo_recebido; // decrementa o fluxo recebido da capacidade da aresta encontrada
            
            // acessa a aresta inversa para incrementar a capacidade da aresta encontrada
            //grafo.map_do_grafo.at(chave_iterada_caminho).lista_de_arestas.at(chave_pai).capacidade += fluxo_recebido;

            chave_iterada_caminho = chave_pai; // para iterar sobre o caminho
        }
    }

    return fluxo_total;
}

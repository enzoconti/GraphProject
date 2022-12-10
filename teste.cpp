#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <limits>
#include <set>
#include <algorithm>

using namespace std;

#define TAM_PAG_DISCO 960
#define TAM_MAX_CAMPO_VARIAVEL 32
#define TAM_REG_DADOS 64
#define TAM_REG_CABECALHO 21
#define TAM_NOME_ARQUIVO 20
#define TAM_PAG_ARVORE 65
#define TAM_REG_CABECALHO_ARVORE 17
#define TAM_REG_DADOS_ARVORE 65
#define TAM_NOME_ARQUIVO 20
#define ORDEM_ARVORE_B 5


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

void dijkstra(map<int, adj_list> grafo, map<int, int>& distancias, map<int, int>& antecessores, int chave_origem){

    list<int> nao_visitados;//guarda os vertices ainda não visitados

    int menor_chave_nao_visitada;//marca o próximo vertice a ser visitado

    map<int, adj_list>:: iterator grafo_iterator;
    map<int, int>::iterator teste_distancias;

    //enquanto não acabar o grafo, insere a key do distancias como idConecta e o valor da distância 99999
    for(grafo_iterator = grafo.begin(); grafo_iterator != grafo.end(); grafo_iterator++){
        distancias.insert({grafo_iterator->first, 999999});//distancias iniciais a todos vertices
        antecessores.insert({grafo_iterator->first, -1});//antecessores dos vertices
        nao_visitados.push_back(grafo_iterator->first);//idConecta dos não visitados
    }
    nao_visitados.sort();//ordena lista primeira vez

    distancias.at(chave_origem) = 0;//iguala-se o dado, isto é, distância

    printf("Primeiro preenchimento de distâncias:\n");
    for(teste_distancias = distancias.begin(); teste_distancias != distancias.end(); teste_distancias++){
        printf("Vértice: %d ", teste_distancias->first);
        printf("Distancia: %d\n\n ", teste_distancias->second);
    }
    printf("\n");

    set<int> visitados;//marca os vertices visitados
    int cont = 0;

    while(!nao_visitados.empty()){//enquanto não acabar os vertices não visitados
        menor_chave_nao_visitada = nao_visitados.front();//define o menor elemento
        nao_visitados.pop_front();//remove vertice da fila
        visitados.insert(menor_chave_nao_visitada);//insere no visitados o ultimo vertice visitado

       // map<int, int>::iterator teste;

        //for(teste = antecessores.begin(); teste != antecessores.end(); teste++){
            //printf("Vértice: %d ", teste->first);
            //printf("Antecessor: %d\n", teste->second);
       // }
        
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

        printf("%d° preenchimento de distâncias:\n", cont);
        for(teste_distancias = distancias.begin(); teste_distancias != distancias.end(); teste_distancias++){
            printf("Vértice: %d ", teste_distancias->first);
            printf("Distancia: %d\n\n ", teste_distancias->second);
        }
        cont++;
    }
}

int main(){

    map<int, adj_list> grafo;

    vertice v0, v1, v2, v3, v4, v5, v6;
    aresta ar0_1, ar0_3, ar0_4, ar1_0, ar1_2, ar1_3, ar2_1, ar2_3, ar3_0, ar3_1, ar3_2, ar3_4, ar4_0, ar4_3, ar5_4, ar4_5;
    adj_list adjl0, adjl1, adjl2, adjl3, adjl4, adjl5, adjl6;

    v0.idConecta = 0;
    v1.idConecta = 1;
    v2.idConecta = 2;
    v3.idConecta = 3;
    v4.idConecta = 4;
    v5.idConecta = 5;
    v6.idConecta = 6;

    strcpy(v0.nomePoPs, "nomePoPs_V0");
    strcpy(v1.nomePoPs, "nomePoPs_V1");
    strcpy(v2.nomePoPs, "nomePoPs_V2");
    strcpy(v3.nomePoPs, "nomePoPs_V3");
    strcpy(v4.nomePoPs, "nomePoPs_V4");
    strcpy(v5.nomePoPs, "nomePoPs_V5");
    strcpy(v6.nomePoPs, "nomePoPs_V6");

    strcpy(v0.nomePais, "nomePais_V0");
    strcpy(v1.nomePais, "nomePais_V1");
    strcpy(v2.nomePais, "nomePais_V2");
    strcpy(v3.nomePais, "nomePais_V3");
    strcpy(v4.nomePais, "nomePais_V4");
    strcpy(v5.nomePais, "nomePais_V5");
    strcpy(v6.nomePais, "nomePais_V6");

    strcpy(v0.siglaPais, "SP0");
    strcpy(v1.siglaPais, "SP1");
    strcpy(v2.siglaPais, "SP2");
    strcpy(v3.siglaPais, "SP3");
    strcpy(v4.siglaPais, "SP4");
    strcpy(v5.siglaPais, "SP5");
    strcpy(v6.siglaPais, "SP6");

    //Aresta (0,1)
    ar0_1.origem = &v0;
    ar0_1.destino = &v1;
    ar0_1.peso = 3;

    //Aresta (0,3)
    ar0_3.origem = &v0;
    ar0_3.destino = &v3;
    ar0_3.peso = 8;

    //Aresta (0,4)
    ar0_4.origem = &v0;
    ar0_4.destino = &v4;
    ar0_4.peso = 7;

    //Aresta (1,0)
    ar1_0.origem = &v1;
    ar1_0.destino = &v0;
    ar1_0.peso = 3;

    //Aresta (1,2)
    ar1_2.origem = &v1;
    ar1_2.destino = &v2;
    ar1_2.peso = 1;

    //Aresta (1,3)
    ar1_3.origem = &v1;
    ar1_3.destino = &v3;
    ar1_3.peso = 4;

    //Aresta (2,1)
    ar2_1.origem = &v2;
    ar2_1.destino = &v1;
    ar2_1.peso = 1;

    //Aresta (2,3)
    ar2_3.origem = &v2;
    ar2_3.destino = &v3;
    ar2_3.peso = 2;

    //Aresta (3,0)
    ar3_0.origem = &v3;
    ar3_0.destino = &v0;
    ar3_0.peso = 8;

    //Aresta (3,1)
    ar3_1.origem = &v3;
    ar3_1.destino = &v1;
    ar3_1.peso = 4;

    //Aresta (3,2)
    ar3_2.origem = &v3;
    ar3_2.destino = &v2;
    ar3_2.peso = 2;

    //Aresta (3,4)
    ar3_4.origem = &v3;
    ar3_4.destino = &v4;
    ar3_4.peso = 3;

    //Aresta (4,0)
    ar4_0.origem = &v4;
    ar4_0.destino = &v0;
    ar4_0.peso = 7;

    //Aresta (4,3)
    ar4_3.origem = &v4;
    ar4_3.destino = &v3;
    ar4_3.peso = 3;

    //Aresta (4,5)
    ar4_5.origem = &v4;
    ar4_5.destino = &v5;
    ar4_5.peso = 1;

    //Aresta (5,4)
    ar5_4.origem = &v5;
    ar5_4.destino = &v4;
    ar5_4.peso = 1;

    adjl0.v = v0;
    adjl1.v = v1;
    adjl2.v = v2;
    adjl3.v = v3;
    adjl4.v = v4;
    adjl5.v = v5;
    adjl6.v = v6;

    adjl0.lista_de_arestas.insert({1, ar0_1});
    adjl0.lista_de_arestas.insert({3, ar0_3});
    adjl0.lista_de_arestas.insert({4, ar0_4});

    adjl1.lista_de_arestas.insert({0, ar1_0});
    adjl1.lista_de_arestas.insert({2, ar1_2});
    adjl1.lista_de_arestas.insert({3, ar1_3});

    adjl2.lista_de_arestas.insert({1, ar2_1});
    adjl2.lista_de_arestas.insert({3, ar2_3});

    adjl3.lista_de_arestas.insert({0, ar3_0});
    adjl3.lista_de_arestas.insert({1, ar3_1});
    adjl3.lista_de_arestas.insert({2, ar3_2});
    adjl3.lista_de_arestas.insert({4, ar3_4});

    adjl4.lista_de_arestas.insert({0, ar4_0});
    adjl4.lista_de_arestas.insert({3, ar4_3});
    adjl4.lista_de_arestas.insert({5, ar4_5});

    adjl5.lista_de_arestas.insert({4, ar5_4});

    grafo.insert({0, adjl0});
    grafo.insert({1, adjl1});
    grafo.insert({2, adjl2});
    grafo.insert({3, adjl3});
    grafo.insert({4, adjl4});
    grafo.insert({5, adjl5});
    grafo.insert({6, adjl6});

    map<int, int> distancias;
    map<int, int> antecessores;

    dijkstra(grafo, distancias, antecessores, 0);
    printf("Distância de 0 para 4: %d\n", distancias.at(4));


}
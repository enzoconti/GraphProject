#include "../include/funcoes_de_print.h"

/**
 * @brief Função que imprime o número de buscas realizadas
 * 
 * @param buscas Inteiro que representa o número de buscas realizadas
 */
void print_busca(int buscas){
    printf(BUSCA, (buscas + 1));
}

/**
 * @brief Função que imprime o número de páginas de disco
 * 
 * @param num_pag_disco Inteiro que diz o número de páginas de disco
 */
void print_num_pag_discos(int num_pag_disco){
    printf(NUM_PAG_DISCO, num_pag_disco);
}

/**
 * @brief Mensagem de erro de falha no processamento do arquivo
 * 
 */
void print_falha_processamento_arquivo(){
    printf(ERRO_FALHA_PROCESSAMENTO_ARQUIVO);
}

/**
 * @brief Mensagem de erro para registro inexistente
 * 
 */
void print_registro_inexistente(){
    printf(ERRO_REG_INEXISTENTE);
}

void print_falha_grafo(){
    printf("Falha na execução da funcionalidade.\n");
}

/*

Função responsável por imprimir os dados de um registro conforme requisitado nas especificações do trabalho. Recebe um
ponteiro para registro de dados. Antes de cada impressão, é verificado em cada caso em que o campo pode ser nulo, se o valor
contido na struct não é referente à um campo nulo. Pois caso seja, não ocorre a impressão deste campo.

*/
void printa_registro(reg_dados* reg){

    printf("Identificador do ponto: %d\n", reg->idConecta);
    if(reg->nomePoPs[0] != '\0') printf("Nome do ponto: %s\n", reg->nomePoPs);//confere se não é nulo
    if(reg->nomePais[0] != '\0') printf("Pais de localizacao: %s\n", reg->nomePais);
    if(reg->siglaPais[0] != '$') printf("Sigla do pais: %s\n", reg->siglaPais);
    if(reg->idPoPsConectado != -1) printf("Identificador do ponto conectado: %d\n", reg->idPoPsConectado);
    if(reg->velocidade != -1) printf("Velocidade de transmissao: %d %sbps\n", reg->velocidade, reg->unidadeMedida);
    printf("\n");
}

/**
 * @brief Função responsável por imprimir os dados de um registro de junção conforme requisitado nas especificações do trabalho.
 * 
 * @param reg1 Ponteiro para o primeiro registro, vindo do arquivo de dados 1
 * @param reg2 Ponteiro para o segundo registro, vindo do arquivo de dados 2
 */
void printa_registro_juncao(reg_dados* reg1, reg_dados* reg2){

    printf("Identificador do ponto: %d\n", reg1->idConecta);
    if(reg1->nomePoPs[0] != '\0') printf("Nome do ponto: %s\n", reg1->nomePoPs);//confere se não é nulo
    if(reg1->nomePais[0] != '\0') printf("Pais de localizacao: %s\n", reg1->nomePais);
    if(reg1->siglaPais[0] != '$') printf("Sigla do pais: %s\n", reg1->siglaPais);
    if(reg1->idPoPsConectado != -1) printf("Identificador do ponto conectado: %d\n", reg1->idPoPsConectado);
    if(reg2->nomePoPs[0] != '\0') printf("Nome do ponto conectado: %s\n", reg2->nomePoPs);//confere se não é nulo
    if(reg2->nomePais[0] != '\0') printf("Nome do pais conectado: %s\n", reg2->nomePais);
    if(reg2->siglaPais[0] != '$') printf("Sigla do pais: %s\n", reg2->siglaPais);
    if(reg1->velocidade != -1) printf("Velocidade de transmissao: %d %sbps\n", reg1->velocidade, reg1->unidadeMedida);
    printf("\n");
}
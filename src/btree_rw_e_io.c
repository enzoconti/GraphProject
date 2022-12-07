#include "../include/btree_rw_e_io.h"

/**
 * @brief Função responsável pela escrita de um registro de cabeçalho da Árvore B
 * 
 * @param arquivo Ponteiro para o arquivo de índice
 * @param reg Ponteiro do registro cujos dados serão gravados no arquivo de índice
 */
void escrever_no_arquivo_cabecalho_arvore(FILE* arquivo, reg_cabecalho_arvore* reg){
    
    escreve_string_no_arquivo(reg->status, arquivo, 1);
    fwrite(&reg->noRaiz, sizeof(int), 1, arquivo);
    fwrite(&reg->nroChavesTotal, sizeof(int), 1, arquivo);
    fwrite(&reg->alturaArvore, sizeof(int), 1, arquivo);
    fwrite(&reg->RRNproxNo, sizeof(int), 1, arquivo);
    
    for(int i = 0; i < (TAM_PAG_ARVORE - TAM_REG_CABECALHO_ARVORE); i++){
        fwrite("$", sizeof(char), 1, arquivo);
    }
}

/**
 * @brief Função responsável pela leitura de um registro de cabeçalho da Árvore B
 * 
 * @param arquivo Ponteiro para o arquivo de índice
 * @param reg Ponteiro para o registro em que serão gravados os dados lidos do arquivo
 */
void ler_reg_cabecalho_arvore(FILE* arquivo, reg_cabecalho_arvore* reg){
    
    fread(&reg->status[0], sizeof(char), 1, arquivo);
    reg->status[1] = '\0';

    fread(&reg->noRaiz, sizeof(int), 1, arquivo);
    fread(&reg->nroChavesTotal, sizeof(int), 1, arquivo);
    fread(&reg->alturaArvore, sizeof(int), 1, arquivo);
    fread(&reg->RRNproxNo, sizeof(int), 1, arquivo);
    
    fseek(arquivo, TAM_PAG_ARVORE, SEEK_SET);
}

/**
 * @brief Função responsável pela escrita de um registro de índice (nó da árvore B) num arquivo de índice
 * 
 * @param arquivo Ponteiro para o arquivo de índice
 * @param reg Ponteiro do registro de dados que terá suas informações escritas no arquivo de índice
 */
void escrever_dados_indice(FILE* arquivo, reg_dados_indice* reg){

    fwrite(&(reg->folha[0]), sizeof(char), 1, arquivo);
    fwrite(&reg->nroChavesNo, sizeof(int), 1, arquivo);
    fwrite(&reg->alturaNo, sizeof(int), 1, arquivo);
    fwrite(&reg->RRNdoNo, sizeof(int), 1, arquivo);

    for(int i = 0; i < ORDEM_ARVORE_B-1; i++){
        fwrite(&(reg->ponteiroSubarvore[i]), sizeof(int), 1, arquivo);
        fwrite(&(reg->chaveBusca[i]), sizeof(int), 1, arquivo);
        fwrite(&(reg->RRNdoRegistro[i]), sizeof(int), 1, arquivo);
    }
    fwrite(&(reg->ponteiroSubarvore[ORDEM_ARVORE_B-1]), sizeof(int), 1, arquivo);
}

/**
 * @brief Função responsável pela escrita de um registro de índice (nó da árvore B) num arquivo de índice a partir de dado RRN
 * 
 * @param arquivo Ponteiro para o arquivo de índice
 * @param RRN RRN do nó que se quer escrever
 * @param reg Ponteiro do registro de dados que terá suas informações escritas no arquivo de índice
 */
void escrever_dados_indice_porRRN(FILE* arquivo, int RRN, reg_dados_indice* reg){

    int byte_offset = TAM_PAG_ARVORE + RRN*TAM_REG_DADOS_ARVORE;
    fseek(arquivo, byte_offset, SEEK_SET);
    escrever_dados_indice(arquivo, reg);
}

/**
 * @brief Função responsável pela leitura dos dados de um nó de um arquivo de índice
 * 
 * @param arquivo Ponteiro para o arquivo de índice
 * @param reg Ponteiro para o registro em que serão gravados os dados lidos do arquivo de índice
 */
void ler_dados_indice(FILE* arquivo, reg_dados_indice* reg){

    fread(&(reg->folha[0]), sizeof(char), 1, arquivo);
    reg->folha[1] = '\0';


    fread(&reg->nroChavesNo, sizeof(int), 1, arquivo);
    fread(&reg->alturaNo, sizeof(int), 1, arquivo);
    fread(&reg->RRNdoNo, sizeof(int), 1, arquivo);

    for(int i = 0; i < ORDEM_ARVORE_B-1; i++){
        fread(&(reg->ponteiroSubarvore[i]), sizeof(int), 1, arquivo);
        fread(&(reg->chaveBusca[i]), sizeof(int), 1, arquivo);
        fread(&(reg->RRNdoRegistro[i]), sizeof(int), 1, arquivo);
    }
    fread(&(reg->ponteiroSubarvore[ORDEM_ARVORE_B-1]), sizeof(int), 1, arquivo);
}

/**
 * @brief Função responsável pela leitura dos dados de um nó de um arquivo de índice a partir de um dado RRN
 * 
 * @param arquivo Ponteiro para o arquivo de índice
 * @param RRN RRN do nó que se quer ler
 * @param reg Ponteiro para o registro em que serão gravados os dados lidos do arquivo de índice
 */
void ler_dados_indice_porRRN(FILE* arquivo, int RRN, reg_dados_indice* reg){

    int byte_offset = TAM_PAG_ARVORE + RRN*TAM_REG_DADOS_ARVORE;
    fseek(arquivo, byte_offset , SEEK_SET);
    ler_dados_indice(arquivo, reg);
}

 void printHeaderArvore(reg_cabecalho_arvore* h_btree){
    printf("status: %s\n", h_btree->status);
    printf("noRaiz: %d\n",h_btree->noRaiz);
    printf("nroChavesTotal: %d\n",h_btree->nroChavesTotal);
    printf("alturaArvore: %d\n",h_btree->alturaArvore);
    printf("RRNproxNo: %d\n",h_btree->RRNproxNo);
    printf("\n");
 }

 void printa_registro_arvore(reg_dados_indice *btree_reg){
    printf("folha: %c\n", btree_reg->folha[0]);
    printf("nroChavesNo: %d\n", btree_reg->nroChavesNo);
    printf("alturaDoNO: %d\n", btree_reg->alturaNo);
    printf("RRNdoNo: %d\n", btree_reg->RRNdoNo);
    for(int i=0;i<ORDEM_ARVORE_B-1;i++){
        printf("\tchaveBusca[%d]: %d",i,btree_reg->chaveBusca[i]);
    }
    printf("\n");
    for(int i=0;i<ORDEM_ARVORE_B-1;i++){
        printf("\tRRNdoRegistro[%d]: %d",i,btree_reg->RRNdoRegistro[i]);
    }
    printf("\n");
    for(int i=0;i<ORDEM_ARVORE_B;i++){
        printf("ponteiroSubarvore[%d]: %d ",i,btree_reg->ponteiroSubarvore[i]);
    }
    printf("\n\n");
 }

#include "../include/funcoes_auxiliares.h"

/*

Função responsável por ler uma string, e separá-la conforme o caracter ','. Declara um ponteiro de char estático como NULL
que armazena o endereço da posição analisada da string. Em seguida, é declarado um ponteiro de char que aponta para o endereço da posição analisada da string.
Daí, é feita uma comparação, se a string é NULL e o endereço da string é NULL, é retornado NULL pela função.
Se a string for NULL, a função retorna o endereço da posição analisada da string.

Em seguida, se o marcador for NULL, o que aconteceria caso não houvesse o separador na string analisada, o que é feito
utilizando a função (strstr) o endereço da string é NULL e a string é retornada.

Caso a posição zero da string seja um separador ',', o ponteiro é incrementado e é retornado um separador. Em seguida, o
marcador é definido como um '\0', ou seja, o fim da string. 

Por fim, verifica se o marcador aponta para o final da string, se sim, o endereço da string é nulo, se não, o endereço da string é o marcador + 1,
e é retornado a string.
*/

char* separador(char* string){
    static char* endereco_string = NULL;//define uma variável estática que armazena o endereço da posição analisada da stringing
    char* marcador;//define um marcador que aponta para o endereço da posição analisada da string
    char* comma;
    comma = (char*) malloc(2* sizeof(char));

    if ((string == NULL && endereco_string == NULL)){
        return NULL;
    }

    if(string == NULL){//se a string for "separa_proximo", a função retorna o endereço da posição analisada da string
        string = endereco_string;
    }

    if((marcador = strstr(string, ",")) == NULL){
        endereco_string = NULL;
        return string;
    }

    if(string[0] == ','){
        endereco_string++;
        return comma;
    }
    *marcador = '\0';//substitui a vírgula por um caractere nulo

    endereco_string = (marcador + 1 == NULL) ? NULL : (marcador + 1);//verifica se o marcador aponta para o final da string, se sim, o endereço da string é nulo, se não, o endereço da string é o marcador + 1

    return string;//retorna a string temporária
}

/*

Função responsável por receber um ponteiro para um registro de dados, um contador da posição do campo lido,
e um token que representa uma string do arquivo csv separado por vírgula

É feito um switch case para verificar o valor que indica a posição do campo, dado que os campos estão sempre na
mesma ordem. Portanto para cada valor da posição, de 0 a 6, é feita a operação desejada:

Se 0: representa o idConecta, nunca nulo, portanto a função (atoi) converte para inteiro a string (token)
e coloca na variável tmp, em seguida o valor é colocado na struct

Se 1 ou 2: refere-se aos campos de tamanho varíavel, portanto, verifica-se primeiro se o primeiro caracter é ",", caso seja
o campo tem valor nulo e é copiado um '|' e um '\0' para a string. Caso contrário, é copiada a string a partir de (token),
e na posição seguinte é colocado o indicador de fim de campo '|', e na à frente um '\0' para controle interno do programa.

Para todos outros casos, é feita a mesma verificação do primeiro caracter de (token), e seguindo a mesma lógica, caso seja
',', é copiado no campo o indicador de valor nulo para inteiros (-1) ou '$' indicador de lixo para campos de tamanho fixo,
mais um '\0' para controle interno do programa. Caso contrário é copiada a string para a struct, e no caso de campos
referentes à inteiros, é utilizada a função (atoi) para converter (token) para um inteiro, e assim o valor é colocado
na struct.

Apenas para o caso 6, referente ao campo velocidade, a verificação é diferente, pois neste caso, em situações que o campo é
nulo, não há dois caracteres ',' adjacentes, portanto a verificação é feita utilizando a função (atoi), em que se for
retornado 0, isto é, se (token) não é conversível para um inteiro, então é nulo, e portanto coloca-se -1. Caso contrário,
o valor obtido é convertido e armazenado na struct.

*/
void gravar_dados(reg_dados* reg, int pos_campo, char* token){
    int tmp;

    switch(pos_campo){
        case 0:
            tmp = atoi(token);
            reg->idConecta = tmp;
            break;
        case 1:
            if(token[0] != ','){
                strcpy(reg->nomePoPs, token);
                strcat(reg->nomePoPs, "|");
            }
            else{
                strcpy(reg->nomePoPs, "|");
            }
            break;
        case 2:
            if(token[0] != ','){
                strcpy(reg->nomePais, token);
                strcat(reg->nomePais, "|");
            }
            else{
                strcpy(reg->nomePais, "|");
            }
            break;
        case 3:
            if(token[0] != ','){
                strcpy(reg->siglaPais, token);
            }
            else{
                strcpy(reg->siglaPais, "$$");
            }
            break;
        case 4:
            if(token[0] != ','){
                tmp = atoi(token);
                reg->idPoPsConectado = tmp;
            }
            else{
                reg->idPoPsConectado = -1;
            }
            break;
        case 5:
            if(token[0] != ','){
                strcpy(reg->unidadeMedida, token);
            }
            else{
                strcpy(reg->unidadeMedida, "$");
            }
            break;
        case 6:
            tmp = atoi(token);
            if(tmp == 0){
                reg->velocidade = -1;
            }
            else{
                reg->velocidade = tmp;
            }
            break;
    }
}


/**
 * @brief Função responsável por calcular a quantidade de páginas de disco em um arquivo de dados
 * 
 * @param cont_registros recebe o número de registros de dados
 * @return int com a quantidade de páginas de disco
 */
int calcula_pag_disco(int cont_registros){
    int modulo = 0;
    int nroPagDisco = 0;

    modulo = (TAM_REG_DADOS*cont_registros)%TAM_PAG_DISCO;
    nroPagDisco = (int)(TAM_REG_DADOS*cont_registros)/TAM_PAG_DISCO;

    if(modulo != 0){
        nroPagDisco++;
    }
    nroPagDisco++;

    return nroPagDisco;
}

/*

Função responsável por conferir se o registro é removido ou não, e caso negativo 
printar o registro. Recebe o registro de dados e o arquivo de entrada. Declara-se 
o inteiro num_RRN para mandar como argumento para a função le_arquivo, e o 
inteiro status que recebe a flag retornada pela função le_arquivo() 

Se o status recebeu 1, significa que não é o fim do arquivo. Então, é conferido 
se o registro foi removido, e caso negativo chama-se a função printa_registro() 
para printar o registro. O retorno, então, é o repasse da flag do status, 1. 

Se o status recebeu 0, significa que o arquivo chegou ao fim, e a flag recebida 
pelo status é retornada. 

*/
int confere_remocao(reg_dados* reg, FILE* arquivo_entrada){

    int num_RRN = 0;
    int status = le_arquivo(reg, arquivo_entrada, &num_RRN);
    if(status==1){//nao é o fim do arquivo

        if (reg->removido[0] == '0'){ //registro não removido
            printa_registro(reg);
        }
        return status;
    }
    else return status;
}

/**
 * @brief Função responsável por ler registros até e retornar uma flag indicando se o arquivo chegou ao fim ou não, e neste processo
 * caso os registros do arquivo de dados não sejam removidos, eles são inseridos no arquivo de índice árvore B
 * 
 * @param reg Ponteiro para registro de dados que será lido
 * @param arquivo_entrada Ponteiro para arquivo de entrada de dados
 * @param arquivo_indice Ponteiro para arquivo de índice que será criado
 * @param novo_reg_cabecalho_arvore Ponteiro para um registro de cabeçalho de árvore B
 * @return int Retorna um inteiro que sinaliza se o arquivo chegou ao fim ou não
 */
int monta_arvore(reg_dados* reg, FILE* arquivo_entrada, FILE* arquivo_indice, reg_cabecalho_arvore* novo_reg_cabecalho_arvore){

    int num_RRN = 0;
    int status = le_arquivo(reg, arquivo_entrada, &num_RRN);
    if(status==1){//nao é o fim do arquivo

        if (reg->removido[0] == '0'){ //registro não removido
            insercao_btree(arquivo_indice, novo_reg_cabecalho_arvore, reg->idConecta, num_RRN);
        }
        return status;
    }
    else return status;
}

/**
 * @brief Função responsável por ler registros até e retornar uma flag indicando se o arquivo chegou ao fim ou não, e neste processo
 * caso os registros do arquivo de dados não sejam removidos, é feita uma busca indexada no arquivo de índice da árvore B,
 * usando com chave o campo idPoPsConectado do registro do arquivo 1, e caso hajam registros com idConecta igual ao idPoPsConectado,
 * eles são impressos na tela.
 * 
 * @param reg1 Ponteiro para registro de dados que será lido do arquivo 1
 * @param arquivo_entrada Ponteiro para arquivo de dados que será feita a busca linear
 * @param cabecalho_arvore Ponteiro para um registro de cabeçalho de árvore B
 * @param reg_indice Ponteiro para registro de índice da árvore B que contém o RRN do registro de dados procurado
 * @param arquivo_procura Ponteiro para arquivo de dados que contém o registro procurado usando o arquivo de índice
 * @param arquivo_indice Ponteiro para o arquivo de índice da árvore B
 * @param reg_procura Ponteiro para o registro de dados que armazenará o registro de dados procurado com árvore B
 * @return int 
 */
int juncao(int* num_reg_encontrados_total, reg_dados* reg1, FILE* arquivo_entrada, reg_cabecalho_arvore* cabecalho_arvore, reg_dados_indice* reg_indice, FILE* arquivo_procura, FILE* arquivo_indice, reg_dados*reg_procura){

    int num_RRN = -1;//argumento necesário para função busca_indexada
    int status = le_arquivo(reg1, arquivo_entrada, &num_RRN);
    int num_reg_encontrados = 0;

    if(status==1){//nao é o fim do arquivo
        
        int flag_busca=0;
        flag_busca = busca_indexada(reg1->idPoPsConectado, &num_reg_encontrados, cabecalho_arvore, reg_indice, arquivo_procura, arquivo_indice, reg_procura, &num_RRN);
        *num_reg_encontrados_total += num_reg_encontrados;
        
        if (flag_busca == ENCONTRADO){
          if (reg1->removido[0] == '0') printa_registro_juncao(reg1, reg_procura);//registro não removido
        }
        return status;
    }
    else return status;
}

/*

Função responsável por compactar o arquivo. Recebe o registro de dados, o arquivo 
de entrada, o arquivo de saida e o contador de registros. Declara-se o inteiro 
num_RRN para mandar como argumento para a função le_arquivo, e o inteiro status 
que recebe a flag retornada pela função le_arquivo() 

Se o status recebeu 1, significa que não é o fim do arquivo. Então, é conferido 
se o registro foi removido, e caso negativo o contador de registro aumenta em um 
e o registro lido é escrito no arquivo de saida com a função 
escrever_no_arquivo_dados(). O retorno, então, é o repasse da flag do status, 1. 

Se o status recebeu 0, significa que o arquivo chegou ao fim, e a flag 
recebida pelo status é retornada. 

*/
int compacta_arquivo( reg_dados* reg, FILE* arquivo_entrada, FILE* arquivo_saida, int*contador_reg){
    //se o arquivo não está no final, le registro
    int num_RRN = 0;
    int status = le_arquivo(reg, arquivo_entrada, &num_RRN);
    
    if (status == 1){
        if (reg->removido[0]=='0'){
            (*contador_reg)++;
            //registro não removido, escreve registro;
            escrever_no_arquivo_dados(arquivo_saida, reg);
        }
        return status;
    }
    else return status;
}

/*
Função responsável por remover logicamente um registro. Recebe o registro de dados, 
o arquivo de entrada, o registro de saida e o RRN do registro. 

Primeiro muda-se na RAM o removido do registro de dados de '0' para '1'. Então, o 
numero no encadeamento, no registro de dados, passa a ser o numero do topo, que 
esta no registro de cabecalho. Já o topo passa a receber o valor do RRN do 
registro removido, criando uma pilha. 

Então, é declarada a variavel inteira do byteoffset, que é em seguida calculada, 
variando com o valor do RRN. É feito um fseek para o byteoffset do RRN desejado, 
atualizado no arquivo o "removido" com a função escreve_string_no_arquivo() e 
o encadeamento com um fwrite(). O resto do espaço ate o final do registro é 
por fim preenchido com lixo, em um loop for() que faz o fwrite() do caracter $.
*/
void apaga_registro(FILE*arquivo_entrada, reg_dados* reg_dados, reg_cabecalho*reg_cabecalho, int* RRN ){
  
    reg_dados->removido[0] = '1';
    reg_dados->encadeamento = reg_cabecalho->topo;
    reg_cabecalho->topo = *RRN;
   
    int byteoffset = 0;
    
    byteoffset = TAM_PAG_DISCO + (TAM_REG_DADOS)*(*RRN);

    fseek(arquivo_entrada, byteoffset, SEEK_SET);

    escreve_string_no_arquivo(reg_dados->removido, arquivo_entrada, strlen(reg_dados->removido));
    fwrite(&reg_dados->encadeamento, sizeof(int), 1, arquivo_entrada);

    for(int i = 0; i < (TAM_REG_DADOS - 5); i++){
        fwrite("$", sizeof(char), 1, arquivo_entrada);
    }
}

/*

Função responsável por remover o arquivo. Recebe o nome do arquivo temporario 
e o nome do arquivo de entrada. São inializadas as variaveis inteiras renomear 
e remover como 1. 

Primeiro a variavel remover recebe o retorno da função remove(), que remove 
o arquivo enviado, o arquivo de entrada, e retorna 0 se a operação foi realizada 
com sucesso. Em seguida, a variavel renomear recebe o retorno da função rename(), 
que renomeia o arquivo enviado no primeiro argumento (arquivo de saida) para o 
nome do segundo argumento (arquivo original de entrada) e retorna 0 se a operação 
for realizada com sucesso

Se ambas operações tem sucesso, ou seja, renomear e remover são iguais a 0, é 
printado o binario do arquivo compactado com a função binarioNaTela. 
Caso contrário é printado "Falha no processamento do arquivo". 

*/
void remover_arquivo(char* nome_temp, char* nome_do_arquivo_entrada){
    int renomear = 1;
    int remover = 1;

    remover = remove(nome_do_arquivo_entrada);
    renomear = rename(nome_temp, nome_do_arquivo_entrada);

    if (renomear==0 && remover==0) binarioNaTela(nome_do_arquivo_entrada);
    else print_falha_processamento_arquivo();
}
#include "../include/funcoes_de_busca.h"

/*

Função responsável por comparar um campo de inteiro recebido ao respectivo campo salvo num registro lido de um arquivo.
Recebe um inteiro referente à posição do campo, um inteiro referente ao valor recebido do (input), e um ponteiro para um
registro de dados. É feito um (switch case) com a posição do campo, com cada caso referente à uma posição de campo no registro
de dados. Caso o valor contido na struct seja igual ao valor buscado, é retornado 1, caso contrário, é retornado 0, para 
todos casos de comparação.

Caso nenhuma das condições seja verdadeira, é impressa na tela uma mensagem de erro e retornado -1.

*/
int compara_campo_inteiro(int pos_campo, int valor, reg_dados* reg){

    switch(pos_campo){
        case 0://idConecta
            if(reg->idConecta == valor) return ENCONTRADO;
            return NAO_ENCONTRADO;
            break;
        case 2://idPoPsConectado
            if(reg->idPoPsConectado == valor) return ENCONTRADO;
            return NAO_ENCONTRADO;
            break;
        case 4://velocidade
            if(reg->velocidade == valor) return ENCONTRADO;
            return NAO_ENCONTRADO;
            break;
        default:
            print_falha_processamento_arquivo();
            return -1;
    }
}

/*

Função responsável por comparar um campo de string recebido ao respectivo campo salvo num registro lido de um arquivo.
Recebe um inteiro referente à posição do campo, uma string referente ao valor recebido do (input), e um ponteiro para um
registro de dados. É feito um (switch case) com a posição do campo, com cada caso referente à uma posição de campo no registro
de dados. Caso a string contido na struct seja igual à string buscada (comparação feita utilizando (strcmp)), é retornado 1,
caso contrário, é retornado 0, para  todos casos de comparação.

Caso nenhuma das condições seja verdadeira, é impressa na tela uma mensagem de erro e retornado -1.

*/
int compara_campo_string(int pos_campo, char* campo, reg_dados* reg){

    switch(pos_campo){
        case 1://siglaPais
            if(strcmp(reg->siglaPais, campo) == 0) return ENCONTRADO;
            return NAO_ENCONTRADO;
            break;
        case 3://unidadeMedida
            if(strcmp(reg->unidadeMedida, campo) == 0) return ENCONTRADO;
            return NAO_ENCONTRADO;
            break;
        case 5://nomePoPs
            if(strcmp(reg->nomePoPs, campo) == 0) return ENCONTRADO;
            return NAO_ENCONTRADO;
            break;
        case 6://nomePais
            if(strcmp(reg->nomePais, campo) == 0) return ENCONTRADO;
            return NAO_ENCONTRADO;
            break;
        default:
            print_falha_processamento_arquivo();
            return -1;
    }
}


int busca_inteiro(reg_dados* novo_reg_dados, FILE* arquivo_entrada, int pos_campo, int* num_registros, int valor){

        if (novo_reg_dados->removido[0] != '1')
        {
            if (compara_campo_inteiro(pos_campo, valor, novo_reg_dados) == ENCONTRADO)
            { // se o registro foi encontrado
              (*num_registros)++;
              return ENCONTRADO;
            }
        }
        return NAO_ENCONTRADO;
}


int busca_string(reg_dados* novo_reg_dados, FILE* arquivo_entrada, int pos_campo, int*num_registros, char* buffer){

        if (novo_reg_dados->removido[0] != '1')
        {
            if (compara_campo_string(pos_campo, buffer, novo_reg_dados) == ENCONTRADO)
            { // se o registro foi encontrado
              (*num_registros)++;
              return ENCONTRADO;
            }
  
        }
        return NAO_ENCONTRADO;
}
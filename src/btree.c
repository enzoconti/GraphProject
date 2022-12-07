#include "../include/btree.h"

/**
 * @brief Função responsável por inicializar a busca recursiva de um registro de dados no arquivo de índice da Árvore B
 * 
 * @param reg Ponteiro de registro de cabeçalho para registro de cabeçalho da árvore
 * @param pos posicao em um no especifico(que sera retornado por reg_arvore_encontrado) que a chave esta ou deveria estar(retornada por referencia por isso int*)
 * @param reg_arvore_encontrado Ponteiro para registro de dados do índice que recebe os dados de um registro encontrado na busca
 * @param chave Inteiro referente à chave de busca, isto é, o campo de busca indexada "idConecta" 
 * @param arq Ponteiro para o arquivo de índice em que será realizado a busca
 * @return int Responsável por informar se foi ou não encontrado o registro de dados buscado a partir da chave
 */

int busca_arvore(reg_cabecalho_arvore* reg, int* pos, reg_dados_indice *reg_arvore_encontrado, int chave, FILE* arq, int* num_paginas_lidas){

    if(reg->noRaiz == -1) {
        return NAO_ENCONTRADO;//caso base, não existe arvore
    }

    //Inicializa-se a recursão a partir do RRN do nó da raiz da árvore
    return _busca_arvore(reg->noRaiz, pos, reg_arvore_encontrado, chave, arq, num_paginas_lidas);

}

/**
 * @brief Função recursiva de busca por um registro de dados no arquivo de índice da Árvore B
 * 
 * @param RRN RRN do nó em que se está fazendo a busca
 * @param pos posicao no nó em que se está fazendo a busca que a chave esta ou deveria estar(retornada por referencia por isso int*)
 * @param reg_arvore_encontrado Ponteiro para registro de dados do índice que recebe os dados de um registro encontrado na busca
 * @param chave Inteiro referente à chave de busca, isto é, o campo de busca indexada "idConecta" 
 * @param arq Ponteiro para o arquivo de índice em que será realizado a busca
 * @return int Responsável por informar se foi ou não encontrado o registro de dados buscado a partir da chave
 */
int _busca_arvore(int RRN, int *pos, reg_dados_indice *reg_arvore_encontrado, int chave, FILE* arq, int* num_paginas_lidas){

    if(RRN == -1) { 

    return NAO_ENCONTRADO;//Caso base, nó pai eh folha
    }
    (*num_paginas_lidas)++;


    reg_dados_indice* novo_reg_dados = cria_registro_dados_indice();
    ler_dados_indice_porRRN(arq, RRN, novo_reg_dados);//lê o nó atual para RAM


    int flag_retorno = busca_na_pagina(chave, pos, novo_reg_dados); //busca na pagina (nó) atual
    if(flag_retorno == ENCONTRADO){//se encontrado

        *reg_arvore_encontrado = *novo_reg_dados; // reg_arvore_encontrado se torna o novo_reg_dados 
        return ENCONTRADO;
    }
    else{
        //Chama busca recursiva, com o novo RRN referente ao ponteiro para subArvore na posição "pos"
        *reg_arvore_encontrado = *novo_reg_dados; // atualiza o reg_arvore_encontrado para ser utilizado na insercao
        return(_busca_arvore(novo_reg_dados->ponteiroSubarvore[*pos], pos, reg_arvore_encontrado, chave, arq, num_paginas_lidas));
    }
}

/**
 * @brief Função responsável por fazer a busca indexada de um registro de dados a partir de uma chave e um arquivo de
 * índice Árvore B
 * 
 * @param valor Buffer de inteiro para armazenar o o valor do campo idConecta
 * @param num_registros_encontrados Variável responsávael por marcar o número de registros encontrados
 * @param novo_reg_cabecalho_arvore Ponteiro para um registro de cabeçalho da Árvore B
 * @param novo_reg_encontrado Ponteiro de um registro de dados da árvore B para armazenar o registro encontrado na busca
 * @param arquivo_dados Ponteiro para o arquivo de dados com os registros
 * @param arquivo_indice Ponteiro para o arquivo de índice
 * @param novo_reg_dados Ponteiro para registro de dados que armazenará as informações do registro encontrado no arquivo de dados
 */
int busca_indexada(int valor, int* num_registros_encontrados, reg_cabecalho_arvore *novo_reg_cabecalho_arvore, reg_dados_indice *novo_reg_encontrado, FILE* arquivo_dados, FILE* arquivo_indice, reg_dados *novo_reg_dados, int* num_paginas_lidas){
    int pos = 0;//posicao no arquivo de indice

    *num_registros_encontrados = 0;

    int flag_retorno = busca_arvore(novo_reg_cabecalho_arvore, &pos, novo_reg_encontrado, valor, arquivo_indice, num_paginas_lidas);

    if(flag_retorno != NAO_ENCONTRADO){//encontrou registro
        int byteoffset = TAM_PAG_DISCO + (novo_reg_encontrado->RRNdoRegistro[pos])*TAM_REG_DADOS;
        fseek(arquivo_dados, byteoffset, SEEK_SET);
        fread(novo_reg_dados->removido, sizeof(char), 1, arquivo_dados);
        novo_reg_dados->removido[1] = '\0';
        le_registro(novo_reg_dados, arquivo_dados);
        (*num_paginas_lidas)++;//contar a leitura do registro de dados do arquivo de dados

        if (novo_reg_dados->removido[0] != '1'){
            (*num_registros_encontrados)++;
            return ENCONTRADO;
        }
        else if(num_registros_encontrados == 0){
            return NAO_ENCONTRADO;
        }
    }
    return NAO_ENCONTRADO;
}


/**
 * @brief funcao master que chama a recursao da insercao da btree e lida com split em raiz
 * 
 * @param fp arquivo de indice onde esta a btree
 * @param h registro de cabecalho do arquivo de indice lido em RAM  
 * @param key chave a ser inserida
 * @param data_rrn_4insertion rrn do registro de dados a ser inserido (acompanha key)
 */
void insercao_btree(FILE*fp, reg_cabecalho_arvore*h, int key, int data_rrn_4insertion){
    int flag_retorno;
    int promoted_child, promoted_key, promoted_data_rrn;
    reg_dados_indice* new_root; // em possivel split na raiz
    int flag_busca;

    flag_retorno = _insercao_btree(fp,h,h->noRaiz,key,data_rrn_4insertion,&promoted_child,&promoted_key,&promoted_data_rrn); // chama a primeira recursao

    if(flag_retorno == PROMOTION){ // houve split na raiz ou a arvore estava vazia
        // nova raiz
        new_root = cria_registro_dados_indice();

        // se houve split na raiz(e nao estava vazia), entao o no raiz != -1 e portanto o no criado nao eh folha
        if(h->noRaiz != -1){
        new_root->folha[0] = '0';
        } // em caso contrario o no eh folha e nao o alteramos ja que o padrao de cria_registro_dados_indice() eh inicializar com folha[0] = 1

        new_root->alturaNo = h->alturaArvore + 1;

        new_root->nroChavesNo = 1;

        new_root->RRNdoNo = h->RRNproxNo;

        new_root->chaveBusca[0] = promoted_key;
        new_root->RRNdoRegistro[0] = promoted_data_rrn;

        new_root->ponteiroSubarvore[0] = h->noRaiz;
        new_root->ponteiroSubarvore[1] = promoted_child;

        // atualiza header
        h->alturaArvore++;
        h->noRaiz = new_root->RRNdoNo;
        h->RRNproxNo++;

        escrever_dados_indice_porRRN(fp, new_root->RRNdoNo,new_root);
    }
    h->nroChavesTotal++; // unica atualizacao feita independentemente de ter ocorrido split na raiz ou nao
}


/**
 * @brief funcao recursiva que insere os nodes internos da btree
 * 
 * @param fp arquivo de indice onde esta a btree
 * @param h cabecalho do arquivo ja lido
 * @param rrn_indice_atual rrn no arquivo de indice da pagina da btree em que queremos inserir
 * @param key chave a ser inserida
 * @param data_rrn_4insertion rrn no arquivo de dados que deve ser inserido
 * @param promoted_child rrn do filho promovido daqui pro nivel superior
 * @param promoted_key chave promovida daqui pro nivel superior
 * @param promoted_data_rrn rrn no arquivo de dados promovido daqui pro nivel superior
 * @return ** int retorna se houve promocao ou nao, com uma flag extra para erro em caso de chaves repetidas
 */
int _insercao_btree(FILE* fp,reg_cabecalho_arvore* h, int rrn_indice_atual, int key, int data_rrn_4insertion, int* promoted_child, int* promoted_key, int* promoted_data_rrn){
    int posicao_key, flag_retorno, flag_insercao=0;

    if(rrn_indice_atual == -1){ // atingimos uma pagina inexistente - rrn vazio
        *promoted_key = key;
        *promoted_data_rrn = data_rrn_4insertion;
        *promoted_child = -1; // chave promovida vai sem nenhum filho
        
        return PROMOTION; // promovemos a key para a pagina pai na qual sera inserida
    }

    // criamos um novo reg que eh o reg atual correspondente ao rrn_indice_atual passado como argumento da funcao
    reg_dados_indice *reg_arvore_atual = cria_registro_dados_indice();
    ler_dados_indice_porRRN(fp, rrn_indice_atual, reg_arvore_atual);
    
    // buscamos na pagina a posicao em que a chave esta ou deveria estar
    flag_insercao = busca_na_pagina(key, &posicao_key, reg_arvore_atual);
        
    if(flag_insercao == ENCONTRADO) return INSERTION_ERROR; // retorna erro se ja possui a chave

    // as informacoes possivelmente promovidas de baixo sao atribuidas nessas variaveis
    int* promoted_below_child = (int*)malloc(sizeof(int));
    int* promoted_below_key = (int*)malloc(sizeof(int));
    int* promoted_below_data_rrn = (int*)malloc(sizeof(int));

    // chamada recursiva para onde a chave deveria estar (nao esta pois flag_insercao == ENCONTRADO ja foi descartado)
    flag_retorno =  _insercao_btree(fp,h, reg_arvore_atual->ponteiroSubarvore[posicao_key] ,key,data_rrn_4insertion, promoted_below_child, promoted_below_key, promoted_below_data_rrn);


    if(flag_retorno == NO_PROMOTION || flag_retorno == INSERTION_ERROR){ // houve erro ou a inserco ja ocorreu na camada de baixo e esse nivel nao precisa realizar mais nada
        return flag_retorno;
    }
    // flag_retorno == PROMOTION a partir daqui, pois NO_PROMOTION e INSERTION_ERROR foram descartados
    if(reg_arvore_atual->nroChavesNo < ORDEM_ARVORE_B - 1){ // significa que esse no tem espaco
        flag_insercao = insere_na_pagina(reg_arvore_atual,*promoted_below_key,*promoted_below_data_rrn,*promoted_below_child); // simplesmente inserimos ordenadametne na pagina
        escrever_dados_indice_porRRN(fp,reg_arvore_atual->RRNdoNo,reg_arvore_atual); // escreve os dados atualizados da pagina 
        

        if(flag_insercao == ENCONTRADO) return INSERTION_ERROR; // descartamos tambem o erro caso o insere_na_pagina encontre a chave onde for inserir
        else return NO_PROMOTION;
    }else{ // nao temos espaco e devemos dar split com uma nova pagina
        // cria nova pagina
        reg_dados_indice *newreg_arvore;
        newreg_arvore = cria_registro_dados_indice();
        newreg_arvore->RRNdoNo = h->RRNproxNo; // escreveremos no prox RRN disponivel
        h->RRNproxNo++;
        strcpy(newreg_arvore->folha,reg_arvore_atual->folha); // propriedade de folha eh herdada da pagina "irma"
        newreg_arvore->alturaNo = reg_arvore_atual->alturaNo; // altura do no tambem eh herdada da pagina "irma"

        // rotina de split separa o conjunto de chaves que estavam na pagina atual + a chave inserida e promove uma
        split(*promoted_below_key,*promoted_below_data_rrn,*promoted_below_child,promoted_key,promoted_data_rrn,promoted_child,reg_arvore_atual,newreg_arvore);

        // reescreve os dados de ambas as paginas da btree
        escrever_dados_indice_porRRN(fp,reg_arvore_atual->RRNdoNo,reg_arvore_atual);
        escrever_dados_indice_porRRN(fp,newreg_arvore->RRNdoNo,newreg_arvore);

        return PROMOTION;
    }
}

   /**
    * @brief uma funcao que divide o mais igualmente possivel o conjunto de chaves formado pelas chaves em um no cheio + a chave a ser inserida em dois nos da arvore b
    * 
    * @param key chave a ser inserida
    * @param data_rrn rrn no arquivo de dados que corresponde a chave a ser inserida
    * @param right_child rrn do filho da direita da chave a ser inserida
    * @param upkey chave a ser promovida
    * @param updata_rrn rrn no arquivo de dados que corresponde a chave a ser promovida
    * @param upchild filho da direita a ser promovido
    * @param r pagina original da arvore b
    * @param newr pagina recem criada da arvore b
    */
    /* 
        explicacao do funcionamento da funcao split:
        um array auxiliar aux[TAM+1] recebe o array original a[TAM] + o valor pra insercao 
        [a0 , a1 , ... , a(n-1)] + valor_insercao = [aux0, aux1, ..., aux(n) ]
        sendo que essa insercao eh feita pelo algoritmo de insercao ordenada similar ao da funcao insere_na_pagina
        nesse caso, como a ordem eh impar, teremos a seguinte construcao pros arrays de chave e de data_rrn:
        [a0, a1, a2, a3] + valor_insercao = [aux0, aux1, aux2, aux3, aux4]
        assim, promovemos a chave aux2 para distribuir mais igualmente as chaves, isto eh, [aux0,aux1] ficam na pagina original e [aux3,aux4] na nova pagina

        apesar da insercao ocorrer apenas em nos folha, o split pode ocorrer em nos nao folha por potencialmente se propagar pra cima
        portanto devemos nos preocupar tambem com o array de ponteiros para subarvores filhas, no qual temos a seguinte construcao:
        [auxf0, auxf1, ..., auxf(n+1)] = [f0, f1, ..., f(n)] + filho_insercao
        nesse caso, temos n valores para o array original de filhos pois tem-se um filho a mais do que o numero de chaves (assim como em uma arvore binaria com apenas uma chave tem-se dois filhos)
        dessa forma, o que acontece eh que cada chave[i] carrega, alem de seu data_rrn[i], seu filho da direita[i+1]
        com isso, fica mais facil trabalhar com um array apenas com os filhos da direita, excluindo o filho mais a esquerda do no original(ele nao sofre alteracoes durante todo split)
        portanto, o array aux_right_children tem todos filhos da direita das chaves originais mais o que deve ser inserido
        dessa forma, o array aux_right_child eh manipulado exatamente como o array data_rrn, isto eh, eh apenas preenchido de acordo com a ordenacao de aux_keys

        no entanto, nesse caso, a promocao leva o proprio rrn do no criado(rrn_np no esquema),
        uma vez que todas keys [ aux_keys(posicao_promocao+1), ... , aux_keys(n)] soa necessariamente maiores que a key promovida aux_keys(posicao_promocao) pela ordenacao do array
        com isso, o filho da direita da chave a ser inserida, isto eh, aux_right_children[posicao_promocao] eh inserido na esquerda do no criado
        assim, os dois filhos mais a esquerda do no criado sao os casos especiais: o do no original(mais a esquerda) nao se altera, e o do novo no(mais a direita) recebe o filho da direita da chave que eh promovida
        no esquema abaixo, para economizar espaco, o array aux_right_children eh nomeado apenas de arc

        assim teremos a construcao:

                                pagina de cima
            [...,        chavedecimai,     chavedecima(i+1), ...]    
                   /                    \              \
                  /   promocao:  ^       \             ...
                 /    chave2     |        \
                /   data_rrn2    |         \
               /    rrn_np       |          \
              /                  |           \
            pagina original                nova pagina
            [chave0,chave1]               [chave3,chave4]
           /       |       \            /        |      \
        [f0,      arc0,      arc1]     [arc2,      arc3,    arc4]
                                        | |
                                        | |__|\
                                        |____  | aux_right_childen[2] eh o filho da direita da chave2, a chave a ser promovida
                                             |/

        apesar de no esquema assumirmos ORDEM_ARVORE_B como impar(i.e numero de chave eh par), isso nao faz diferenca no algoritmo
        caso tenhamos 4 chaves(ordem 5) + 1 de insercao e devemos promover uma, promovemos a chave3(posicao 2)
        caso tenhamos 3 chaves(ordem 4) + 1 de insercao e devemos promover uma(optando por deixar o no da esquerda com mais chaves), promovemos a chave3 (posicao 2)
        em ambos casos, a posicao(=2) eh igual ao resultado da divisao inteira da ordem por 2(5/2 = 2 ou 4/2 = 2)
    */
void split(int key, int data_rrn,int right_child, int* upkey, int* updata_rrn, int*upchild, reg_dados_indice *r, reg_dados_indice *newr ){
    // aqui precisamos de vetores auxiliares que serao ordenados de acordo com as keys
    int aux_keys[ORDEM_ARVORE_B];
    int aux_data_rrns[ORDEM_ARVORE_B];
    int aux_right_children[ORDEM_ARVORE_B];
    int promotion_position = ORDEM_ARVORE_B/2;

    // primeiro inserimos ordenadamente(ordenacao baseada nas keys) nos vetores auxiliares
    int has_added_value=0; // indica se o valor ja foi adicionado
    int j=0;              // contador adicional que percorre chaveBusca[] e ponteiroSubarvore[]
    for(int i=0;i<ORDEM_ARVORE_B;i++){
        // se o valor ainda nao foi adicionado e encontramos a primeira chave em chaveBusca[] maior que a key 
        // OU se ja percorremos todas chaves originais(caso em que a chave a ser inserida eh a maior)
        // inserimos a key ao inves dessa chaveBusca[]
        
        if( (j == ORDEM_ARVORE_B-1) || (has_added_value == 0 && key < r->chaveBusca[j]) ){ 
            has_added_value = 1;
            aux_keys[i] = key;
            aux_data_rrns[i] = data_rrn;
            aux_right_children[i] = right_child;
        }else{    // caso contrario, so adicionamos a chaveBusca[] (seja antes da key pois chaveBusca[j] < key ou seja depois pois key ja foi adicionada)
            aux_keys[i] = r->chaveBusca[j];
            aux_data_rrns[i] = r->RRNdoRegistro[j];
            aux_right_children[i] = r->ponteiroSubarvore[j+1];
            j++;
        }
    }

    // atualizando os valores a serem promovidos
    *upkey = aux_keys[promotion_position];
    *updata_rrn = aux_data_rrns[promotion_position];
    *upchild = newr->RRNdoNo;

    // atualizando os valores de chave e RRNdoRegistro das paginas da btree
    for(int i=0;i<promotion_position;i++){
        r->RRNdoRegistro[i] = aux_data_rrns[i];
        r->chaveBusca[i] = aux_keys[i];
        r->ponteiroSubarvore[i+1] = aux_right_children[i];

        newr->RRNdoRegistro[i] = aux_data_rrns[i+promotion_position+1];
        newr->chaveBusca[i] = aux_keys[i+promotion_position+1];
        newr->ponteiroSubarvore[i+1] = aux_right_children[i+promotion_position+1];
    }
    // o fiho da direita da chave a ser promovida fica na posicao mais a esquerda do novo no
    newr->ponteiroSubarvore[0] = aux_right_children[promotion_position];

    // atualizando o numero de chaves em cada no
    newr->nroChavesNo = promotion_position;
    r->nroChavesNo = promotion_position;
    if(ORDEM_ARVORE_B % 2 == 0) r->nroChavesNo++; // o da esquerda fica com um a mais se a ordem for par

    // colocando NIL nos arrays com valores invalidos para padronizacao de lixo
    for(int i=promotion_position;i<ORDEM_ARVORE_B-1;i++){
        r->RRNdoRegistro[i] = -1;
        r->chaveBusca[i] = -1;
        r->ponteiroSubarvore[i+1] = -1;

        newr->RRNdoRegistro[i] = -1;
        newr->chaveBusca[i] = -1;
        newr->ponteiroSubarvore[i+1] = -1;
    }

}

/**
 * @brief funcao que busca em uma pagina da btree a posicao em que a chave esta ou deveria estar(i.e, onde deveria ser inserida)
 * 
 * @param key chave de busca   
 * @param pos posicao em que esta a chave ou deveria estar (retornada por referencia)
 * @param r pagina a ser buscada
 * @return int representa se a chave foi encontrada ou nao
 */
int busca_na_pagina(int key, int* pos, reg_dados_indice* r){
    int i=0;
    // a posicao onde deveria estar ou esta eh antes da primeira chave maior do que a chave buscada
    for(i=0;i<r->nroChavesNo;i++){
        if(key <= r->chaveBusca[i]){
            *pos = i;

            if(r->chaveBusca[*pos] == key){
                return ENCONTRADO;
            }
            else return NAO_ENCONTRADO;
        }
    }

    if(i == r->nroChavesNo){ // isso significa que nao encontrou nenhuma chave maior que ela
        *pos = i; // deveria estar por ultimo (isso sai dos array bounds do array original, mas pode ser usado no array de ponteirosSubarvores ja que tem um a mais de tamanho)
        return NAO_ENCONTRADO;
    }

    return SEARCH_ERROR;
}

// insere ordenadamente a key insert_key com seu dado insert_data_rrn e seu filho da direita insert_child no reg_dados_indice* r
int insere_na_pagina(reg_dados_indice* r,int insert_key, int insert_data_rrn, int insert_child){
    int pos,flag_insercao; // posicao onde sera inserido no array ordenado
    
    // buscamos a posicao onde deveria estar
    flag_insercao = busca_na_pagina(insert_key,&pos,r);
    if(flag_insercao == ENCONTRADO) return ENCONTRADO;

    // esse loop percorre o array de traz pra frente deslocando os valores pra frente pra abrir espaco pra insercao
    // nesse caso desloca-se a chave de busca(chaveBusca[]), os filhos(ponteiroSubarvore[]) e os registros de dados(RRNdoRegistro[])
    // o deslocamento pode ser feito pois essa funcao so eh chamada quando ha espaco no array
    for(int i=r->nroChavesNo-1;i>=pos;i--){
        r->chaveBusca[i+1] = r->chaveBusca[i];
        r->RRNdoRegistro[i+1] = r->RRNdoRegistro[i];
        r->ponteiroSubarvore[i+2] = r->ponteiroSubarvore[i+1]; // o indice eh +1 em comparacao aos outros pois esse array eh maior e desloca-se apenas o filho da direita
    }

    // insere-se as variaveis de insercao na posicao definida
    r->chaveBusca[pos] = insert_key;
    r->RRNdoRegistro[pos] = insert_data_rrn;
    r->ponteiroSubarvore[pos+1] = insert_child; // pos+1 pois eh o filho da direita

    r->nroChavesNo++;

    return NAO_ENCONTRADO;
}

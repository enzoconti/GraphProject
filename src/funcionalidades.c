#include "../include/funcionalidades.h"

/**
 * @brief Função referente à funcionalidade 1, que lê um arquivo csv registros e grava os dados em um arquivo binário.
 * Exibindo o output da função binarioNaTela no final da execução.
 *
 */
void comando1()
{
  char *nome_do_arquivo_entrada, *nome_do_arquivo_saida;
  scanf("%ms", &nome_do_arquivo_entrada);
  scanf("%ms", &nome_do_arquivo_saida);

  FILE *arquivo_entrada = abrir_leitura(nome_do_arquivo_entrada);
  if (arquivo_entrada == NULL) return;
  FILE *arquivo_saida = abrir_escrita_binario(nome_do_arquivo_saida);
  if (arquivo_saida == NULL) return;

  char buffer[TAM_REG_DADOS]; // buffer temporário para armazenar as linhas do arquivo csv
  reg_dados *novo_reg_dados = cria_registro_dados();
  reg_cabecalho *novo_reg_cabecalho = cria_registro_cabecalho();

  int cont_registros = 0;

  escrever_no_arquivo_cabecalho(arquivo_saida, novo_reg_cabecalho);
  fgets(buffer, 64, arquivo_entrada); // pula primeira linha
  fgets(buffer, 64, arquivo_entrada); // pula primeira linha

  while (fgets(buffer, 64, arquivo_entrada)){ // enquanto não chegar no fim do arquivo csv (fgets retorna NULL)
    cont_registros++;
    ler_dados(buffer, novo_reg_dados); // lê dados do arquivo csv e grava na struct
    escrever_no_arquivo_dados(arquivo_saida, novo_reg_dados);
  }

  novo_reg_cabecalho->nroPagDisco = calcula_pag_disco(cont_registros);
  strcpy(novo_reg_cabecalho->status, "1"); // atualiza status do arquivo para "1" (não corrompido)
  novo_reg_cabecalho->proxRRN = cont_registros;

  fseek(arquivo_saida, 0, SEEK_SET); // retorna para o início do arquivo

  escrever_no_arquivo_cabecalho(arquivo_saida, novo_reg_cabecalho);

  free(novo_reg_dados);
  free(novo_reg_cabecalho);

  fclose(arquivo_entrada);
  fclose(arquivo_saida);

  binarioNaTela(nome_do_arquivo_saida);
}

/**
 * @brief Função referente à funcionalidade 2, que lê um arquivo binário e exibe todos seus registros.
 *
 */
void comando2()
{

  char *nome_do_arquivo_entrada;
  scanf("%ms", &nome_do_arquivo_entrada);

  FILE *arquivo_entrada = abrir_leitura_binario(nome_do_arquivo_entrada);
  if (arquivo_entrada == NULL) return;

  reg_dados *novo_reg_dados = cria_registro_dados();
  reg_cabecalho *novo_reg_cabecalho = cria_registro_cabecalho();

  ler_reg_cabecalho(arquivo_entrada, novo_reg_cabecalho);

  if (checa_consistencia(novo_reg_cabecalho) != 0)
  {
    free(novo_reg_dados);
    free(novo_reg_cabecalho);
    fclose(arquivo_entrada);
    return;
  }
  if (novo_reg_cabecalho->nroPagDisco == 1){ // se não existirem registros no arquivo
    print_registro_inexistente();
    print_num_pag_discos(novo_reg_cabecalho->nroPagDisco);

    free(novo_reg_dados);
    free(novo_reg_cabecalho);

    fclose(arquivo_entrada);
    return;
  }

  while (confere_remocao(novo_reg_dados, arquivo_entrada) != 0); // enquanto o confere remoçao não retornar 0, ainda há registros a serem lidos

  print_num_pag_discos(novo_reg_cabecalho->nroPagDisco);

  free(novo_reg_dados);
  free(novo_reg_cabecalho);

  fclose(arquivo_entrada);
}

/**
 * @brief Função referente à funcionalidade 3, que lê um arquivo binário e exibe todos registros encontrados em uma busca.
 *
 */
void comando3(){

  char *nome_do_arquivo_entrada;
  scanf("%ms", &nome_do_arquivo_entrada);

  FILE *arquivo_entrada = abrir_leitura_binario(nome_do_arquivo_entrada);
  if (arquivo_entrada == NULL) return;

  int num_buscas = 0;
  int pos_campo = -1;
  int valor = 0;   // buffer de inteiro para o campo recebido
  char buffer[24]; // buffer de string para o campo recebido
  int num_RRN = -1; // necessário para argumento da função le_arquivo
  int num_registros = 0;

  reg_dados *novo_reg_dados = cria_registro_dados();
  reg_cabecalho *novo_reg_cabecalho = cria_registro_cabecalho();

  ler_reg_cabecalho(arquivo_entrada, novo_reg_cabecalho);

  if (checa_consistencia(novo_reg_cabecalho) != 0){
    free(novo_reg_dados);
    free(novo_reg_cabecalho);
    fclose(arquivo_entrada);
    return;
  }

  scanf("%d", &num_buscas);
  for (int i = 0; i < num_buscas; i++){ // enquanto as buscas não acabarem
    print_busca(i + 1);
    int num_registros = 0; // zera o contador de registros encontrados

    pos_campo = ler_campo();

   if (pos_campo == 0 || pos_campo == 2 || pos_campo == 4){ //  se for um campo de inteiro 
      scanf("%d", &valor);
      while (le_arquivo(novo_reg_dados, arquivo_entrada, &num_RRN)!=0){
        if (busca_inteiro(novo_reg_dados, arquivo_entrada, pos_campo, &num_registros, valor) == ENCONTRADO) printa_registro(novo_reg_dados);
      } 
    } 
    else{
      scan_quote_string(buffer);
      while (le_arquivo(novo_reg_dados, arquivo_entrada, &num_RRN)!=0){
        if (busca_string(novo_reg_dados, arquivo_entrada, pos_campo, &num_registros, buffer) == ENCONTRADO) printa_registro(novo_reg_dados);
      }
    }
    
    if(num_registros == 0) print_registro_inexistente();

    print_num_pag_discos(novo_reg_cabecalho->nroPagDisco);
    fseek(arquivo_entrada, TAM_PAG_DISCO, SEEK_SET); // volta pro inicio do arquivo após o registro de cabeçalho para nova busca
  
    free(novo_reg_dados);
    free(novo_reg_cabecalho);
    fclose(arquivo_entrada);  
  }
}

/**
 * @brief Função referente à funcionalidade 4, que lê um arquivo binário e apaga todos registros encontrados em uma busca,
 * cujo output é a função binarioNaTela
 *
 */
void comando4(){

  char *nome_do_arquivo_entrada;
  scanf("%ms", &nome_do_arquivo_entrada);

  FILE *arquivo_entrada = abrir_leitura_e_escrita_binario(nome_do_arquivo_entrada);
  if (arquivo_entrada == NULL) return;

  int num_buscas = 0;
  int pos_campo = -1;
  int valor = 0;   // buffer de inteiro para o campo recebido
  char buffer[24]; // buffer de string para o campo recebido

  reg_dados *novo_reg_dados = cria_registro_dados();
  reg_cabecalho *novo_reg_cabecalho = cria_registro_cabecalho();
  ler_reg_cabecalho(arquivo_entrada, novo_reg_cabecalho);

  if (checa_consistencia(novo_reg_cabecalho) != 0){
    free(novo_reg_dados);
    free(novo_reg_cabecalho);
    fclose(arquivo_entrada);
    return;
  }

  scanf("%d", &num_buscas);
  int num_registros = 0;
  
  for (int i = 0; i < num_buscas; i++){ // enquanto as buscas não acabarem
    pos_campo = ler_campo();
    int num_RRN = -1; //reinicia o RRN para cada busca

    if (pos_campo == 0 || pos_campo == 2 || pos_campo == 4){ //  se for um campo de inteiro 
      scanf("%d", &valor);
      while (le_arquivo(novo_reg_dados, arquivo_entrada, &num_RRN)!=0){
        if (busca_inteiro(novo_reg_dados, arquivo_entrada, pos_campo, &num_registros, valor) == ENCONTRADO) apaga_registro(arquivo_entrada, novo_reg_dados, novo_reg_cabecalho, &num_RRN);
      }
    } 
    else{
      scan_quote_string(buffer);
      while (le_arquivo(novo_reg_dados, arquivo_entrada, &num_RRN)!=0){
        if (busca_string(novo_reg_dados, arquivo_entrada, pos_campo, &num_registros, buffer) == ENCONTRADO) apaga_registro(arquivo_entrada, novo_reg_dados, novo_reg_cabecalho, &num_RRN);
      }
    }
    fseek(arquivo_entrada, TAM_PAG_DISCO, SEEK_SET);
  }
  
  novo_reg_cabecalho->nroRegRem += num_registros;
  fseek(arquivo_entrada, 0, SEEK_SET);
  escrever_no_arquivo_cabecalho(arquivo_entrada, novo_reg_cabecalho);

  free(novo_reg_dados);
  free(novo_reg_cabecalho);

  fclose(arquivo_entrada);

  binarioNaTela(nome_do_arquivo_entrada);
}

/**
 * @brief Função referente à funcionalidade 5, que lê um arquivo binário e insere um registro no arquivo, cujo output é
 * a função binárioNaTela
 *
 */
void comando5(){

  char *nome_do_arquivo_entrada;
  scanf("%ms", &nome_do_arquivo_entrada);

  FILE *arquivo_entrada = abrir_leitura_e_escrita_binario(nome_do_arquivo_entrada);
  if (arquivo_entrada == NULL) return;

  int num_registros_adicionados = 0;
  int num_registros_total = 0;

  reg_cabecalho *novo_reg_cabecalho = cria_registro_cabecalho();
  reg_dados *novo_reg_dados = cria_registro_dados();

  ler_reg_cabecalho(arquivo_entrada, novo_reg_cabecalho);

  if (checa_consistencia(novo_reg_cabecalho) != 0){
    free(novo_reg_dados);
    free(novo_reg_cabecalho);
    fclose(arquivo_entrada);
    return;
  }

  scanf("%d", &num_registros_adicionados);

  for (int i = 0; i < num_registros_adicionados; i++){ // enquanto os registros a serem adicionados não acabarem
    ler_registros_dados_teclado(novo_reg_dados);
    insere_registro_dados(arquivo_entrada, novo_reg_dados, novo_reg_cabecalho, &num_registros_total);
  }

  strcpy(novo_reg_cabecalho->status, "1");
  fseek(arquivo_entrada, 0, SEEK_SET);
  escrever_no_arquivo_cabecalho(arquivo_entrada, novo_reg_cabecalho);

  free(novo_reg_dados);
  free(novo_reg_cabecalho);

  fclose(arquivo_entrada);

  binarioNaTela(nome_do_arquivo_entrada);
}

/**
 * @brief Função referente à funcionalidade 6, que lê um arquivo binário e desfragmenta o arquivo,cujo output é a função
 * binarioNaTela
 *
 */
void comando6(){

  char *nome_do_arquivo_entrada;
  scanf("%ms", &nome_do_arquivo_entrada);

  int contador_reg = 0;

  FILE *arquivo_entrada = abrir_leitura_e_escrita_binario(nome_do_arquivo_entrada);
  if (arquivo_entrada == NULL) return;

  reg_dados *novo_reg_dados = cria_registro_dados();
  reg_cabecalho *novo_reg_cabecalho = cria_registro_cabecalho();

  ler_reg_cabecalho(arquivo_entrada, novo_reg_cabecalho);

  if (checa_consistencia(novo_reg_cabecalho) != 0){
    free(novo_reg_dados);
    free(novo_reg_cabecalho);
    fclose(arquivo_entrada);
    return;
  }
  if (novo_reg_cabecalho->topo == -1){ // se o registro de cabeçalho é -1, então não há registros removidos
    novo_reg_cabecalho->qttCompacta++;
    novo_reg_cabecalho->status[0] = '1';
    fseek(arquivo_entrada, 0, SEEK_SET); // volta pro inicio
    escrever_no_arquivo_cabecalho(arquivo_entrada, novo_reg_cabecalho);
    fclose(arquivo_entrada);
    binarioNaTela(nome_do_arquivo_entrada);
    return;
  }
  else{                                                          // se o registro de cabeçalho é diferente de -1, então há registros removidos
    FILE *arquivo_saida = abrir_escrita_binario("temp.bin"); // Cria um novo arquivo para escrita dos registros não-removidos
    if (arquivo_saida == NULL) return;

    escrever_no_arquivo_cabecalho(arquivo_saida, novo_reg_cabecalho);
    while (compacta_arquivo(novo_reg_dados, arquivo_entrada, arquivo_saida, &contador_reg) != 0); // enquanto o compacta_arquivo não retornar 0, ainda há registros a serem lidos
    atualizar_reg_cabecalho(novo_reg_cabecalho, arquivo_saida, &contador_reg);
    fclose(arquivo_saida);
    remover_arquivo("temp.bin", nome_do_arquivo_entrada);

    free(novo_reg_dados);
    free(novo_reg_cabecalho);

    fclose(arquivo_entrada);
  }
}

/**
 * @brief Função responsável por ler os registros de dados de um arquivo de dados e criar um índice de árvore B num arquivo
 * de índice
 * 
 */
void comando7(){
  char *nome_arquivo_dados, *nome_arquivo_indice;

  scanf("%ms", &nome_arquivo_dados);
  scanf("%ms", &nome_arquivo_indice);

  FILE* arquivo_dados = abrir_leitura_binario(nome_arquivo_dados); 
  if(arquivo_dados == NULL) return;
  FILE* arquivo_indice = abrir_escrita_binario_cria_novo_arquivo(nome_arquivo_indice);
  if(arquivo_indice == NULL) return;

  reg_dados *novo_reg_dados = cria_registro_dados();
  reg_cabecalho *novo_reg_cabecalho = cria_registro_cabecalho();
  reg_cabecalho_arvore* novo_reg_cabecalho_arvore = cria_registro_cabecalho_arvore();

  ler_reg_cabecalho(arquivo_dados, novo_reg_cabecalho);

  if (checa_consistencia(novo_reg_cabecalho) != 0){
    free(novo_reg_dados);
    free(novo_reg_cabecalho);
    fclose(arquivo_dados);
    return;
  }

  if(novo_reg_cabecalho->nroPagDisco == 1){//não há registros
    print_falha_processamento_arquivo();

    free(novo_reg_dados);
    free(novo_reg_cabecalho);
    free(novo_reg_cabecalho_arvore);

    fclose(arquivo_dados);
    fclose(arquivo_indice);

    return;
  }

  escrever_no_arquivo_cabecalho_arvore(arquivo_indice,novo_reg_cabecalho_arvore);
  
  while(monta_arvore(novo_reg_dados, arquivo_dados, arquivo_indice, novo_reg_cabecalho_arvore) != 0);//monta arvore

  fseek(arquivo_indice, 0, SEEK_SET);//volta pro inicio no arquivo de indice
  
  strcpy(novo_reg_cabecalho_arvore->status, "1");
  escrever_no_arquivo_cabecalho_arvore(arquivo_indice, novo_reg_cabecalho_arvore);//reescreve cabeçalho do arquivo de indice

  free(novo_reg_dados);
  free(novo_reg_cabecalho);
  free(novo_reg_cabecalho_arvore);

  fclose(arquivo_dados);
  fclose(arquivo_indice);

  binarioNaTela(nome_arquivo_indice);
}

/**
 * @brief Função responsável por realizar a funcionalidade 8, que lê um arquivo binário e um arquivo de índice e realiza a
 * busca de um registro, utilizando o arquivo de índice no caso de buscas que utilizem o campo idConecta, que é a chave de busca.
 * e utiliza a busca linear (da funcionalidade 3), caso seja utilizado qualquer outro campo.
 * 
 */
void comando8(){
  char *nome_arquivo_dados, *nome_arquivo_indice;

  scanf("%ms", &nome_arquivo_dados);
  scanf("%ms", &nome_arquivo_indice);

  FILE* arquivo_dados = abrir_leitura_binario(nome_arquivo_dados);
  if(arquivo_dados == NULL) return;
  FILE* arquivo_indice = abrir_leitura_binario(nome_arquivo_indice); 
  if(arquivo_indice == NULL) return;

  int num_buscas = 0;
  int pos_campo = -1;
  char buffer[24]; // buffer de string para o campo recebido
  int valor = 0;   // buffer de inteiro para o campo recebido
  int num_registros_encontrados = 0;
  int num_RRN = -1;
  int num_paginas_lidas = 0;

  scanf("%d", &num_buscas);

  reg_dados *novo_reg_dados = cria_registro_dados();
  reg_dados_indice *novo_reg_encontrado = cria_registro_dados_indice();
  reg_cabecalho *novo_reg_cabecalho = cria_registro_cabecalho();
  reg_cabecalho_arvore *novo_reg_cabecalho_arvore = cria_registro_cabecalho_arvore();

  ler_reg_cabecalho(arquivo_dados, novo_reg_cabecalho);
  ler_reg_cabecalho_arvore(arquivo_indice, novo_reg_cabecalho_arvore);

  if (checa_consistencia(novo_reg_cabecalho) != 0){
    free(novo_reg_dados);
    free(novo_reg_cabecalho);
    free(novo_reg_encontrado);
    free(novo_reg_cabecalho_arvore);
    fclose(arquivo_dados);
    fclose(arquivo_indice);
    return;
  }
  if (checa_consistencia_indice(novo_reg_cabecalho_arvore) != 0){
    free(novo_reg_dados);
    free(novo_reg_cabecalho);
    free(novo_reg_encontrado);
    free(novo_reg_cabecalho_arvore);
    fclose(arquivo_dados);
    fclose(arquivo_indice);
    return;
  }
  

  for (int i = 0; i < num_buscas; i++){ // enquanto as buscas não acabarem
    print_busca(i);
    pos_campo = ler_campo();

    if(pos_campo == 0){
      scanf("%d", &valor);
      num_paginas_lidas = 2;//reseta contador para nova busca, 2 para contar a leitura do cabeçalho do arquivo de índice e do cabeçalho do arquivo de dados
      if(busca_indexada(valor, &num_registros_encontrados, novo_reg_cabecalho_arvore, novo_reg_encontrado, arquivo_dados, arquivo_indice, novo_reg_dados, &num_paginas_lidas) == ENCONTRADO) printa_registro(novo_reg_dados);
    
      if (num_registros_encontrados == 0) print_registro_inexistente(); 
      print_num_pag_discos(num_paginas_lidas);
    }
    else if (pos_campo == 2 || pos_campo == 4){// se for um campo de inteiro

      scanf("%d", &valor);
      while (le_arquivo(novo_reg_dados, arquivo_dados, &num_RRN)!=0){
        if (busca_inteiro(novo_reg_dados, arquivo_dados, pos_campo, &num_registros_encontrados, valor) == ENCONTRADO) printa_registro(novo_reg_dados);
      }
      if (num_registros_encontrados == 0) print_registro_inexistente(); 
      print_num_pag_discos(novo_reg_cabecalho->nroPagDisco);
    }
    else if (pos_campo == 1 || pos_campo == 3 || pos_campo == 5 || pos_campo == 6){ // se for campo de string

      scan_quote_string(buffer);
      while (le_arquivo(novo_reg_dados, arquivo_dados, &num_RRN)!=0){
        if(busca_string(novo_reg_dados, arquivo_dados, pos_campo, &num_registros_encontrados, buffer) == ENCONTRADO) printa_registro(novo_reg_dados);
      }
      if (num_registros_encontrados == 0) print_registro_inexistente(); 
      print_num_pag_discos(novo_reg_cabecalho->nroPagDisco);
    }
  

    fseek(arquivo_dados, TAM_PAG_DISCO, SEEK_SET); // volta pro inicio do arquivo após o registro de cabeçalho para nova busca
    fseek(arquivo_indice, TAM_PAG_ARVORE, SEEK_SET); // volta pro inicio do arquivo após o registro de cabeçalho para nova busca
  }
  free(novo_reg_dados);
  free(novo_reg_cabecalho);
  free(novo_reg_encontrado);
  free(novo_reg_cabecalho_arvore);
  fclose(arquivo_dados);
  fclose(arquivo_indice);
}

/**
 * @brief Função responsável por realizar o comando 9, isto é, para dado número de registros a serem inseridos num arquivdo
 * de dados, insere os registros no arquivo de dados e no arquivo de índice da árvore B.
 * 
 */
void comando9(){

  char *nome_arquivo_dados, *nome_arquivo_indice;
  int n_insercoes, rrn_reg_dados;
  FILE *data_fp, *btree_fp;
  reg_dados rd;
  scanf("%ms", &nome_arquivo_dados);
  scanf("%ms", &nome_arquivo_indice);

  data_fp = abrir_leitura_e_escrita_binario(nome_arquivo_dados);
  btree_fp = abrir_leitura_e_escrita_binario(nome_arquivo_indice);

  reg_cabecalho *h;
  h = cria_registro_cabecalho();
  ler_reg_cabecalho(data_fp, h);

  if (checa_consistencia(h) != 0){
    free(h);
    fclose(data_fp);
    return;
  }
  strcpy(h->status, "0");
  fseek(data_fp,0,SEEK_SET);
  escrever_no_arquivo_cabecalho(data_fp,h);

  reg_cabecalho_arvore *h_btree;
  h_btree = cria_registro_cabecalho_arvore();
  ler_reg_cabecalho_arvore(btree_fp, h_btree);

  if (checa_consistencia_indice(h_btree) != 0){
    free(h_btree);
    fclose(btree_fp);
    return;
  }
  strcpy(h_btree->status, "0");
  fseek(btree_fp,0,SEEK_SET);
  escrever_no_arquivo_cabecalho_arvore(btree_fp,h_btree);

  scanf("%d", &n_insercoes);

  for (int i = 0; i < n_insercoes; i++){
    ler_registros_dados_teclado(&rd);
    strcpy(rd.removido,"0");
    rd.encadeamento = -1;
    int x;
    rrn_reg_dados = insere_registro_dados(data_fp, &rd,h,&x);

    insercao_btree(btree_fp, h_btree, rd.idConecta, rrn_reg_dados);
  }

  strcpy(h->status, "1");
  fseek(data_fp, 0, SEEK_SET);
  escrever_no_arquivo_cabecalho(data_fp, h);

  strcpy(h_btree->status, "1");
  fseek(btree_fp, 0, SEEK_SET);
  escrever_no_arquivo_cabecalho_arvore(btree_fp, h_btree); // CRIAR ESSA FUNCAO

  fclose(data_fp);
  fclose(btree_fp);
  binarioNaTela(nome_arquivo_dados);
  binarioNaTela(nome_arquivo_indice);
}


/**
 * @brief Função responsável por abrir 2 arquivos binários para leitura, sendo que o segundo possui um arquivo de índice
 * de árvore B relacionado. Daí é realizada a auto-junção, isto é, para cada registro do primeiro arquivo de dados, é verificado
 * se o campo idPoPsconectado é igual ao campo idConecta de algum registro do segundo arquivo de dados. A busca é feita utilizando
 * o arquivo de índice. Caso sejam iguais, ambos registros são impressos.
 * 
 */
void comando10(){

  char *nome_arquivo_dados1, *nome_arquivo_dados2, *nome_arquivo_indice, *campo_arquivo_dados1, *campo_arquivo_dados2;
  int num_reg_encontrados = 0;//argumento necesário para função busca_indexada

  scanf("%ms", &nome_arquivo_dados1);
  scanf("%ms", &nome_arquivo_dados2);
  scanf("%ms", &campo_arquivo_dados1);
  scanf("%ms", &campo_arquivo_dados2);
  scanf("%ms", &nome_arquivo_indice);

  if(strcmp(campo_arquivo_dados1, "idPoPsConectado") != 0){
    print_falha_processamento_arquivo();
    return;
  } 
  if(strcmp(campo_arquivo_dados2, "idConecta") != 0){
    print_falha_processamento_arquivo();
    return;
  } 

  FILE* arquivo_dados1 = abrir_leitura_binario(nome_arquivo_dados1);
  if(arquivo_dados1 == NULL) return;
  FILE* arquivo_dados2 = abrir_leitura_binario(nome_arquivo_dados2);
  if(arquivo_dados2 == NULL) return;
  FILE* arquivo_indice = abrir_leitura_binario(nome_arquivo_indice);
  if(arquivo_indice == NULL) return;

  reg_dados *novo_reg_dados1 = cria_registro_dados();
  reg_dados *novo_reg_dados2 = cria_registro_dados();
  reg_dados_indice *novo_reg_indice= cria_registro_dados_indice();
  reg_cabecalho *novo_reg_cabecalho1 = cria_registro_cabecalho();
  reg_cabecalho *novo_reg_cabecalho2 = cria_registro_cabecalho();
  reg_cabecalho_arvore *novo_reg_cabecalho_arvore = cria_registro_cabecalho_arvore();

  ler_reg_cabecalho(arquivo_dados1, novo_reg_cabecalho1);
  ler_reg_cabecalho(arquivo_dados2, novo_reg_cabecalho2);
  ler_reg_cabecalho_arvore(arquivo_indice, novo_reg_cabecalho_arvore);

  if(novo_reg_cabecalho1->nroPagDisco == 1 || novo_reg_cabecalho2->nroPagDisco == 1){//se houver apenas o registro de cabeçalho, não há registros
    print_registro_inexistente();
    free(novo_reg_dados1);
    free(novo_reg_cabecalho1);
    free(novo_reg_dados2);
    free(novo_reg_cabecalho2);
    free(novo_reg_cabecalho_arvore);
    free(novo_reg_indice);
    fclose(arquivo_dados1);
    fclose(arquivo_dados2);
    fclose(arquivo_indice);
    return;
  }

  if (checa_consistencia(novo_reg_cabecalho1) != 0){
    free(novo_reg_dados1);
    free(novo_reg_cabecalho1);
    free(novo_reg_dados2);
    free(novo_reg_cabecalho2);
    free(novo_reg_cabecalho_arvore);
    free(novo_reg_indice);
    fclose(arquivo_dados1);
    fclose(arquivo_dados2);
    fclose(arquivo_indice);
    return;
  }
  if (checa_consistencia_indice(novo_reg_cabecalho_arvore) != 0){
    free(novo_reg_dados1);
    free(novo_reg_cabecalho1);
    free(novo_reg_dados2);
    free(novo_reg_cabecalho2);
    free(novo_reg_cabecalho_arvore);
    free(novo_reg_indice);
    fclose(arquivo_dados1);
    fclose(arquivo_dados2);
    fclose(arquivo_indice);
    return;
  }

  while (juncao(&num_reg_encontrados, novo_reg_dados1, arquivo_dados1, novo_reg_cabecalho_arvore, novo_reg_indice, arquivo_dados2, arquivo_indice, novo_reg_dados2 ) != 0); // enquanto o juncao não retornar 0, ainda há registros a serem lidos

  if(num_reg_encontrados == 0) print_registro_inexistente();

  free(novo_reg_dados1);
  free(novo_reg_cabecalho1);
  free(novo_reg_dados2);
  free(novo_reg_cabecalho2);
  free(novo_reg_cabecalho_arvore);
  free(novo_reg_indice);
  fclose(arquivo_dados1);
  fclose(arquivo_dados2);
  fclose(arquivo_indice);
}

void comando11(){
  char *nome_arquivo;
  FILE* fp;
  reg_dados *reg = cria_registro_dados();
  reg_cabecalho *h = cria_registro_cabecalho();
  int rrn_lido, posicao_inserida;
  double velocidade_tmp;
  vertice vertice_atual;
  adj_list lista_adj_atual;
  aresta aresta_atual;
  vector<adj_list> graph_of_adj_lists;
  scanf("%ms", &nome_arquivo);

  fp = abrir_leitura_binario(nome_arquivo);

  ler_reg_cabecalho(fp,h);
  while(le_arquivo(reg,fp,&rrn_lido) != 0){
    printf("has readen reg_dados as:\n");
    printa_registro(reg);
    // criar verticeAtual com reg_dados
    //debug//printf("creating vertice_atual with cria_vertice_reg\n");
    cria_vertice_reg(reg,&vertice_atual);
    // criar adj_list alAtual com verticeAtual
    //debug//printf("creating lista_adj_atual with cria_lista_ad\nj");
    lista_adj_atual.v = vertice_atual;
    
    //cria_lista_adj(lista_adj_atual,&vertice_atual);
    // inserir alAtual no vector de adj_list (ORDENADAMENTE - GUARDA INDICE DE INSERCAO insertPos)
    //debug//printf("inserting adj_list with insere_adj_list_no_grafo\n");
    posicao_inserida = insere_adj_list_no_grafo(lista_adj_atual, graph_of_adj_lists);
    //debug//printf("has inserted and returned posicao_inserida=%d\n",posicao_inserida);
    // buscar reg_dados.idPoPsConectado nos idConectas do vector<adj_list>
    //debug//printf("calling for loop to search idPoPsConectado\n");
    for(int i=0;i<graph_of_adj_lists.size();i++){
      // se graph_of_adj_lists[i].vertice.idConecta == reg_dados.idPoPsConectado
      //printf("on iteration %d of the search\n",i);
      if(graph_of_adj_lists[i].v.idConecta == reg->idPoPsConectado){
        //debug//printf("has found grap_hof_adj_lists[%d].v.idConecta = %d == %d = reg->idPoPsConectado\n",i, graph_of_adj_lists[i].v.idConecta,reg->idPoPsConectado);
        //  cria arestaAtual (verticeAtual,graph_of_adj_lists[i].vertice,reg_dados.velocidade)
        //debug//printf("creating aresta_atual with cria_aresta\n");
        velocidade_tmp = reg->velocidade;
        if(reg->unidadeMedida[0] == 'M') velocidade_tmp/=1024;
        cria_aresta(aresta_atual, vertice_atual ,graph_of_adj_lists[i].v, velocidade_tmp );
        //  insere arestaAtual em graph_of_adj_lists[pos].lista e graph_of_adj_lists[insertPos].lista (ORDENADAMENTE)
        //debug//printf("inserindo aresta with insere_aresta_atual\n");
        insere_aresta_atual(graph_of_adj_lists[posicao_inserida].lista, aresta_atual);
        // swap ventra vsai
        //debug//printf("swapping aresta\n");
        swap_aresta(aresta_atual);
        // insere aresta_atual em graph_of_adj_lists[i].lista
        //debug//printf("inserting swapped aresta\n");
        insere_aresta_atual(graph_of_adj_lists[i].lista, aresta_atual);
        break;
      }
    }
    //zera lista atual para proxima iteracao
    //zera_lista_adj(lista_adj_atual);
  }
  //debug//printf("outside comando11() major while\n");
  list<aresta>::iterator it;
  for(int i=0;i< graph_of_adj_lists.size(); i++){
    //printf("%ld\n",graph_of_adj_lists[i].lista.size());
    for(it=graph_of_adj_lists[i].lista.begin();it != graph_of_adj_lists[i].lista.end() ; it++){
      printf("%d %s %s %s %d %.0lfMbps\n", graph_of_adj_lists[i].v.idConecta, graph_of_adj_lists[i].v.nomePoPs ,
                                         graph_of_adj_lists[i].v.nomePais, graph_of_adj_lists[i].v.siglaPais,
                                         it->entrada.idConecta ,it->peso * 1024);
    }
  }
}
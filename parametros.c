#include "parametros.h"

/*
    Sai do programa e da print ao codigo de saida
    1: Erro;
    0: Sucesso;
*/
void EXIT (int code) {
    printf("Exited with code %d\n", code);
    exit(code);
}

/*
    Escreve e guarda a lista de localidades num ficheiro
*/
void print_lista_localidades(ITEM_LOCAL* head, FILE* fp){
    ITEM_LOCAL *atual;
    atual = head -> prox;
    if(atual == NULL) {
        printf("Lista de localidades vazia.\n"); 
        return;
    }
    while(atual!=NULL){
        fprintf(fp, "Nome: %s; Latitude: %.2f; Longitude: %.2f\n", atual->localidade.nome, atual->localidade.latitude, atual->localidade.longitude);
        atual = atual->prox;
    }
    fclose(fp);
}

/*
    Verifica latitude e longitude da localidade
*/
void verifica_localidade(LOCALIDADE localidade, int linha, int print_erro){
    /* so se for corrido comando 1 e que se faz print do erro, mas este e sempre detetado*/
    if (localidade.latitude < -90 || localidade.latitude > 90){
        if(print_erro) printf("Linha %d: Erro latitude fora dos limites (menor que -90 ou maior que 90)\n", linha+1);
        EXIT(1);
    }
    if (localidade.longitude < -180 || localidade.longitude > 180){
        if(print_erro) printf("Linha %d: Erro longitude fora dos limites (menor que -180 ou maior que 180\n", linha+1);
        EXIT(1);
    }
}

/*
    Verifica numero de parametros do retorno
*/
void verifica_retorno(int retorno, int linha, int print_erro){
    if (retorno != 3 && retorno != -1){
        if(print_erro) printf("Linha %d: Erro no numero de parametros (%d)\n", linha+1, retorno);
        EXIT(1);
    }
}

/*
    Adiciona item na lista de localidades a seguir ao ITEM_LOCAL passado e da update ao atual
*/
ITEM_LOCAL * adiciona_no_local(ITEM_LOCAL* atual, LOCALIDADE localidade){
    ITEM_LOCAL * new_node;
    new_node = (ITEM_LOCAL*)malloc(sizeof(ITEM_LOCAL));
    new_node->localidade = localidade;
    new_node -> prox = NULL;

    atual->prox = new_node;
    atual = atual->prox;
    
    return atual;
}

/*
    Escreve a lista de rotas para o ficheiro indicado pelo utilizador
*/
void print_lista_rotas(ITEM_ROTA** head_rotas, META_FILE* info_ficheiro, int* numero_da_rota){
    FILE* fp_output2 = info_ficheiro->fp_output2;

    ITEM_ROTA * rota_atual = (*head_rotas)->prox;
    int rota_encontrada = 0;
    while(rota_atual != NULL){
        if(*numero_da_rota != 0 && rota_atual->nome_rota!=*numero_da_rota){
            rota_atual = rota_atual->prox;
        } else {
            rota_encontrada = 1;
            fprintf(fp_output2, "#ROTA %d \n", rota_atual->nome_rota);
            ITEM_LOCAL* item_atual = rota_atual->topo_item_lista;

            while(item_atual->prox != NULL){
                fprintf(fp_output2, "%s %.2f \n", item_atual->localidade.nome, item_atual->localidade.distancia);
                item_atual = item_atual->prox;
            }
            rota_atual = rota_atual->prox;
            fprintf(fp_output2, "\n");
        }
    }
    if(rota_encontrada == 0){
        printf("ERRO: rota numero '%d' nao encontrada\n", *numero_da_rota);
        EXIT(1);
    }
    fclose(fp_output2);
}

/*
    Ordena a lista de rotas por ordem crescente da distancia final
*/
void ordena_rotas(ITEM_ROTA** head_rotas, META_FILE* ficheiro, INFO dados_obtidos){
    ITEM_ROTA* atual, * next, *previous;
    previous = (*head_rotas);
    atual = previous->prox;
    next = atual->prox;

    int troca = 1;
    while(troca == 1){
        troca = 0;
        for(int i = 0; i<ficheiro->numero_rotas-1; ++i){

            ITEM_LOCAL *atual1 = atual->topo_item_lista;
            ITEM_LOCAL *atual2 = next->topo_item_lista;

            while(atual1->prox->prox != NULL) atual1 = atual1->prox;
            while(atual2->prox->prox != NULL) atual2 = atual2->prox;

            if (atual1->localidade.distancia > atual2->localidade.distancia){
                atual->prox = next->prox;
                next->prox = atual;
                previous->prox = next;

                troca = 1;
                previous = *head_rotas;
                atual = previous->prox;
                next = atual->prox;
            } 
            else if(next->prox != NULL){
                    previous = atual;
                    atual = previous->prox;
                    next = atual->prox;
            } 
        }
    }
}

/*
    Ordena a lista de localidade alfabeticamente
*/
void ordena_alfabeticamente(ITEM_LOCAL** head, int linhas){
    ITEM_LOCAL* atual, * next, *previous;
    previous = *head;
    atual = previous->prox;
    next = atual->prox;

    int troca = 1;
    while(troca == 1){
        troca = 0;
        for(int i = 0; i<linhas-1; ++i){
            if (strcmp(atual->localidade.nome, next->localidade.nome) > 0){
                atual->prox = next->prox;
                next->prox = atual;
                previous->prox = next;

                troca = 1;
                previous = *head;
                atual = previous->prox;
                next = atual->prox;
            } 
            else if(next->prox != NULL){
                    previous = atual;
                    atual = previous->prox;
                    next = atual->prox;
            } 
        }
    }
}

/*
    Escreve os outputs do comando 1 para o ficheiro indicado pelo utilizador
*/
void print_output_com1 (ITEM_LOCAL * head, META_FILE * mfp){
    fprintf(mfp->fp_output1, "Numero de localidades: %d\n", mfp->numero_localidades);
    fprintf(mfp->fp_output1, "Memoria ocupada: %lu\n", mfp->numero_localidades * sizeof(ITEM_LOCAL));
    print_lista_localidades(head, mfp->fp_output1);
}

/*
    Lê o ficheiro localidade.txt, verifica se os valores sao validos e cria linked-list de localidades
*/
int le_verifica_guarda(int print_erro, META_FILE* info_ficheiro, ITEM_LOCAL** head){
    
    FILE * fp1 = fopen("localidades.txt", "r");
    if(fp1 == NULL){
        printf("ERRO: Ficheiro localidades.txt nao encontrado");
        EXIT(1);
    }
    

    char linha[DIM], nome_localidade_l[DIM], resto;
    float longitude_l, latitude_l;
    ITEM_LOCAL* atual = *head; 
    int empty;
    

    while(fgets(linha, 100, fp1)!= NULL){
        int retorno = sscanf(linha, "%s %f %f %c", nome_localidade_l, &latitude_l, &longitude_l, &resto);

        LOCALIDADE nova_localidade;
        nova_localidade.latitude=latitude_l;
        nova_localidade.longitude=longitude_l;
        strcpy(nova_localidade.nome, nome_localidade_l);
        
        verifica_retorno(retorno, info_ficheiro->numero_linhas, print_erro);
        verifica_localidade(nova_localidade, info_ficheiro->numero_linhas, print_erro);
        
        if(retorno == -1) empty+=1;
        if(retorno!=-1) atual = adiciona_no_local(atual, nova_localidade);
        info_ficheiro->numero_linhas += 1;
    }
    info_ficheiro->numero_localidades = info_ficheiro->numero_linhas - empty;
    return 1;
}

/*
    Lê o ficheiro localidades.txt e verifica o valor das localidades
*/
META_FILE * verfica_retorna_meta_ficheiro(INFO dados_obtidos, ITEM_LOCAL ** head, int print_erro){
    
    FILE *fp_output1 = fopen(dados_obtidos.output1, "w");
    FILE *fp_output2 = fopen(dados_obtidos.output2, "w");

    META_FILE* info_ficheiro = (META_FILE*)malloc(sizeof(META_FILE));  
    info_ficheiro -> numero_linhas = 0;
    info_ficheiro -> numero_localidades = 0;
    info_ficheiro -> numero_rotas = 0;
    info_ficheiro -> fp_output1 = fp_output1;
    info_ficheiro -> fp_output2 = fp_output2;
    
    le_verifica_guarda(print_erro, info_ficheiro, head);
    return info_ficheiro;
}

/*
    Adiciona ITEM_LOCAL ao final da lista de localidades
*/
void adiciona_a_lista(ITEM_LOCAL ** head, LOCALIDADE localidade){
    ITEM_LOCAL* atual = *head;
    while(atual->prox!=NULL)
        atual = atual->prox;
    adiciona_no_local(atual,localidade);
}

/*
    Calcula a distancia entre duas localidade com base na latitude e longitude
*/
float calcula_distancia(LOCALIDADE local1, LOCALIDADE local2){
    float dy, dx, distancia;

    dy = R*tan((local2.latitude-local1.latitude) * PI/180);
    dx = R*tan((local2.longitude-local1.longitude) * PI/180);
    distancia = sqrt(pow(dy, 2) + pow(dx, 2));

    return distancia;
}

/*
    Adiciona nova rota à lista de rotas
*/
void add_rota(ITEM_ROTA ** atual_coluna, ITEM_LOCAL** atual_item_lista, int numero_rota){
    ITEM_ROTA* new_node_rota = (ITEM_ROTA*)malloc(sizeof(ITEM_ROTA));
    ITEM_LOCAL* new_item_list = (ITEM_LOCAL*)malloc(sizeof(ITEM_LOCAL));

    /* criar new node */
    new_node_rota->nome_rota = numero_rota;
    new_node_rota->prox = NULL;
    new_node_rota->topo_item_lista = new_item_list;

    /* adiconar new node à lista de listas*/
    (*atual_coluna) -> prox = new_node_rota;
    (*atual_coluna) = new_node_rota;
    (*atual_item_lista) = new_item_list;
}

/*
    Adiciona item a determinada rota da lista de rotas
*/
void add_item_to_rota(ITEM_LOCAL** atual_item_lista, ITEM_LOCAL** previous_item_lista, ITEM_LOCAL** head_localidades, char nome_local[]){
    /*Vai buscar o node com a localidade pretendida à lista de localidades*/
    ITEM_LOCAL *item_lista_original = *head_localidades;
    while(strcmp(item_lista_original->localidade.nome, nome_local)!= 0 && item_lista_original->prox != NULL){
        item_lista_original = item_lista_original->prox;
    }

    /* Se localidade nao existir na lista de localidades sair do programa*/
    if(strcmp(item_lista_original->localidade.nome, nome_local)!= 0) {
        printf("ERRO: cidade '%s' nao existe nas localidades \n", nome_local);
        EXIT(1);
    }
    
    LOCALIDADE localidade_original = item_lista_original->localidade;

    /*Copia o node de localidade da lista de localidades para 
    um novo node que ira para a lista de rotas*/
    ITEM_LOCAL * new_item_lista = (ITEM_LOCAL*)malloc(sizeof(ITEM_LOCAL));
    LOCALIDADE* nova_localidade = (LOCALIDADE*)malloc(sizeof(LOCALIDADE));
    nova_localidade->latitude = localidade_original.latitude;
    nova_localidade->longitude = localidade_original.longitude;
    strcpy(nova_localidade->nome, localidade_original.nome);

    float nova_distancia = 0;
    /* Desde que a localidade nao seja o primeiro item da rota, calcular a distancia entre localidades*/
    if((*previous_item_lista) != NULL) {
        LOCALIDADE local1 = (*previous_item_lista) ->localidade;
        LOCALIDADE local2 = *nova_localidade;
        float distancia = calcula_distancia(local1, local2);
        nova_distancia = (*previous_item_lista)->localidade.distancia + distancia;
    }
    nova_localidade->distancia = nova_distancia;

    /* Avançar um item na lista*/
    (*atual_item_lista) ->localidade = *nova_localidade;
    (*atual_item_lista) -> prox = new_item_lista;
    (*previous_item_lista) = (*atual_item_lista);
    (*atual_item_lista) = new_item_lista;
}

/*
    Cria lista de lista com as rotas
*/
void cria_rotas(INFO dados_obtidos, ITEM_LOCAL** head_localidades, ITEM_ROTA** head_rotas, META_FILE* ficheiro){
    char linha[DIM], string[DIM];
    int nome_rota;
    ITEM_ROTA* atual_coluna = *head_rotas;
    ITEM_LOCAL* atual_item_lista = NULL, *previous_item_lista = NULL;
    FILE* fp = fopen("rotas.txt", "r");
    if(fp==NULL){
        printf("ERRO: ficheiro 'rotas.txt' nao existe \n");
        EXIT(1);
    }

    while(fgets(linha, 100, fp)!=NULL){
        int retorno = sscanf(linha, "%s %d", string, &nome_rota);
        if(strcmp(string, "#ROTA") == 0) {
            add_rota(&atual_coluna, &atual_item_lista, nome_rota);
            previous_item_lista = NULL;
            (ficheiro->numero_rotas) += 1;
        }
        if(string[0]!='#' && retorno != -1) {
            add_item_to_rota(&atual_item_lista, &previous_item_lista, head_localidades, string);
            }
    }
}

/*
    Verifica se cidade a adicionar a lista de localidades ja existe
*/
void verifica_cidade_nova(INFO dados_obtidos){
    FILE* fp = fopen("localidades.txt", "r");
    if(fp==NULL){
        printf("ERRO: o ficheiro 'localidades.txt' nao existe.\n");
        EXIT(1);
    }

    char linha[DIM], nome_localidade_l[DIM];
    float longitude_l, latitude_l;
    while(fgets(linha, DIM, fp)!=NULL){
        sscanf(linha, "%s %f %f", nome_localidade_l, &latitude_l, &longitude_l);
        if(strcmp("Leiria", dados_obtidos.localidade_ad.nome)==0){
            /*Easter egg: Leiria nao existe, este programa nao faz milagres*/
            printf("ERRO: Leiria e um mito, por favor escolha uma cidade valida\n");
            EXIT(1);
        }
        if(strcmp(nome_localidade_l, dados_obtidos.localidade_ad.nome)==0){
            printf("ERRO: cidade '%s' ja existe nas localidades\n", nome_localidade_l);
            EXIT(1);
        }
    }
}

/*
    Verifica se latitude de longitude de uma localidade sao validas
*/
void verifica_localidade_a_adicionar(LOCALIDADE localidade){
    if (localidade.latitude < -90 || localidade.latitude > 90){
        printf("Erro latitude com valor '%.2f' fora dos limites (menor que -90 ou maior que 90\n", localidade.latitude);
        EXIT(1);
    }
    if (localidade.longitude < -180 || localidade.longitude > 180){
        printf("Erro longitude com valor '%.2f' fora dos limites (menor que -180 ou maior que 180\n", localidade.latitude);
        EXIT(1);
    }
}

/*
    Encontra localidade a apagar na lista de localidades e remove-a da linked-list
*/
void encontra_e_apaga_localidade(INFO dados_obtidos, ITEM_LOCAL ** head){
    ITEM_LOCAL *atual = *head, *previous = NULL;
    while(atual->prox != NULL && strcmp(atual->localidade.nome, dados_obtidos.nome_rem)!= 0){
        previous = atual;
        atual = atual->prox;
    }
    if(strcmp(atual->localidade.nome, dados_obtidos.nome_rem)!= 0) {
        printf("ERRO: cidade '%s' nao existe em localidades \n", dados_obtidos.nome_rem);
        EXIT(1);
    }
    previous -> prox = atual->prox;
    free(atual);
}

/*
    executa comando 1 (-LO, -TL)
*/
void com1(INFO dados_obtidos, ITEM_LOCAL ** head){
    META_FILE* meta_ficheiro = verfica_retorna_meta_ficheiro(dados_obtidos, head, 1);
    ordena_alfabeticamente(head, meta_ficheiro -> numero_linhas);
    print_output_com1(*head, meta_ficheiro);
}

/*
    executa comando 2 (-LO, -ADI)
*/
void com2(INFO dados_obtidos, ITEM_LOCAL ** head){
    META_FILE * ficheiro = verfica_retorna_meta_ficheiro(dados_obtidos, head, 0);
    verifica_localidade_a_adicionar(dados_obtidos.localidade_ad);
    verifica_cidade_nova(dados_obtidos);
    adiciona_a_lista(head, dados_obtidos.localidade_ad);
    ordena_alfabeticamente(head, ficheiro->numero_linhas+1);
    print_lista_localidades(*head, ficheiro->fp_output1);
}

/*
    executa comando 3 (-LO, -REM)
*/
void com3(INFO dados_obtidos, ITEM_LOCAL ** head){
    META_FILE * ficheiro = verfica_retorna_meta_ficheiro(dados_obtidos, head, 0);
    encontra_e_apaga_localidade(dados_obtidos, head);
    ordena_alfabeticamente(head, ficheiro->numero_linhas);
    print_lista_localidades(*head, ficheiro->fp_output1);
}

/*
    executa comando 4 (-LO, -LR, -ROTAS)
*/
void com4(INFO dados_obtidos, ITEM_LOCAL ** head_localidades, ITEM_ROTA** head_rotas){
    int numero_da_rota = dados_obtidos.nome_rota;
    META_FILE* ficheiro = verfica_retorna_meta_ficheiro(dados_obtidos, head_localidades, 0);
    ordena_alfabeticamente(head_localidades, ficheiro->numero_linhas);
    print_lista_localidades(*head_localidades, ficheiro->fp_output1);

    cria_rotas(dados_obtidos, head_localidades, head_rotas, ficheiro);
    /*so ordena as rotas se utilizador nao especifica uma rota*/
    if(dados_obtidos.nome_rota==0) ordena_rotas(head_rotas, ficheiro, dados_obtidos);
    print_lista_rotas(head_rotas, ficheiro, &numero_da_rota);
}

void executa(int codigo_comando, INFO dados_recebidos, ITEM_LOCAL** head_localidades, ITEM_ROTA** head_rotas){
    if (codigo_comando == 1) com1(dados_recebidos, head_localidades);
    if (codigo_comando == 2) com2(dados_recebidos, head_localidades);
    if (codigo_comando == 3) com3(dados_recebidos, head_localidades);
    if (codigo_comando == 4) com4(dados_recebidos, head_localidades, head_rotas);
}

/*
    Verifica se valor recebido é numero válido
    No caso do comando 4, deve ser numero inteiro
    No caso do comand0 2, deve ser real
    Retorna:
        0: se nao for numero
        1: se for numero
*/
int verifica_numero (char* argv[], int x, int com, int* vezes_ponto){
    int i = 0;
    *vezes_ponto = 0;

    /*verifica se numero e negativo (no caso de nao ser o comando 4)*/
    if((com != 4) && ((argv[x][i] == '-') || (argv[x][i] == '+'))) {i += 1;}

    /*NOTA: explicar utilizacao de switch case na vez de if else (ascii)*/
    if (argv[x] != NULL){
        for ( ; i < strlen(argv[x]); i = i+1){
            switch (argv[x][i]){
                case '0': break;
                case '1': break;
                case '2': break;
                case '3': break;
                case '4': break;
                case '5': break;
                case '6': break;
                case '7': break;
                case '8': break;
                case '9': break;
                case '.':
                    if (com == 4)
                        return 0;
                    if (com == 2)
                        *vezes_ponto=*vezes_ponto+1;
                break;
                default: return 0;
            }
        }
        if(*vezes_ponto>1){
            printf("ERRO: parametros do comando errados\n");
            EXIT(1);
        }
    }
    else return 0;
return 1;
}

/*
    Encontra a posicao do comando
*/
int find_command (int* pos, char* argv[], int id, int argc){
    int i = 0, times = 0;
    char comandos[][DIM]={"-LO","-TL","-ADI","-REM","-ROTAS","-LR"};
    for (i=0; i<argc; i=i+1){
        if (strcmp(argv[i],comandos[id])==0){
            *pos=i;
            times = times+1;
        }
    }
return times;
}

/*
    Define o comando através de um codigo (estado)
*/
void define_com (char* argv[], int argc, int* com){
    int a = 0, b = 0, c = 0, d = 0, e = 0, f = 0, total = 0, pos;

    if (find_command(&pos, argv, 0, argc)==1) a=1;
    if (find_command(&pos, argv, 1, argc)==1) b=10;
    if (find_command(&pos, argv, 2, argc)==1) c=100;
    if (find_command(&pos, argv, 3, argc)==1) d=1000;
    if (find_command(&pos, argv, 4, argc)==1) e=10000;
    if (find_command(&pos, argv, 5, argc)==1) f=100000;

    total = a + b + c + d + e + f;
    switch (total){
        case 11: *com = 1; break;
        case 101: *com = 2; break;
        case 1001: *com = 3; break;
        case 110001: *com = 4; break;
        default: printf("ERRO: comando invalido\n"); exit(0);
    }
}

void verifica_numero_parametros(int argc, int numero_parametros){
    if (argc != numero_parametros) {
        printf("ERRO: numero de parametros do comando errado\n");
        EXIT(1);
    }
}

/*
    Encontra informacao do comando 1
*/
void find_com1_info (INFO *dados_recebidos, char* argv[], int argc){
    int pos_LO;

    verifica_numero_parametros(argc, 4);
    find_command(&pos_LO, argv, 0, argc);

    /*Verificacoes para posicoes validas; Nome ficheiro nao pode comecar por '-'*/
    if(pos_LO < argc-1 && argv[pos_LO+1][0]!='-'){
        strcpy(dados_recebidos->output1, argv[pos_LO+1]);
    }
    else{
        printf("ERRO: parametros do comando errados\n");
        EXIT(1);
    }
}

/*
    Encontra informacao do comando 2
*/
void find_com2_info (INFO *dados_recebidos, char* argv[], int argc){
    int pos_LO, pos_ADI, vezes_ponto;

    verifica_numero_parametros(argc, 7);

    find_command(&pos_LO, argv, 0, argc);
    find_command(&pos_ADI, argv, 2, argc);

    /*verifica se é numero*/
    if (verifica_numero (argv, pos_ADI+2, 2, &vezes_ponto) == 0 || verifica_numero (argv, pos_ADI+3, 2, &vezes_ponto) == 0){
        printf("ERRO: Parametros do comando errados\n");
        EXIT(1);
    }

    /*Verificacoes para posicoes validas; Nome ficheiro nao pode comecar por '-'*/
    if(pos_LO < argc-1 && pos_ADI < argc-3 && argv[pos_LO+1][0]!='-' && argv[pos_ADI+1][0]!='-'){
        strcpy(dados_recebidos->output1, argv[pos_LO+1]);
        strcpy(dados_recebidos->localidade_ad.nome, argv[pos_ADI+1]);
    }
    else{
        printf("ERRO: parametros do comando errados\n");
        EXIT(1);
    }
    
    dados_recebidos->localidade_ad.latitude = (float) atof (argv[pos_ADI+2]);
    dados_recebidos->localidade_ad.longitude = (float) atof (argv[pos_ADI+3]);
}


/*
    Encontra informacao do comando 3
*/
void find_com3_info (INFO *dados_recebidos, char* argv[],int argc){
    int pos_LO, pos_REM;

    verifica_numero_parametros(argc, 5);

    find_command(&pos_LO, argv, 0, argc);
    find_command(&pos_REM, argv, 3, argc);

    /*Verificacoes para posicoes validas; Nome ficheiro nao pode comecar por '-'*/
    if(pos_LO < argc-1 && argv[pos_LO+1][0]!='-' && pos_REM < argc-1 && argv[pos_REM+1][0]!='-'){
        strcpy(dados_recebidos->output1, argv[pos_LO+1]);
        strcpy(dados_recebidos->nome_rem, argv[pos_REM+1]);
    }
    else{
        printf("ERRO: parametros do comando errados\n");
        EXIT(1);
    }
}

/*
    Encontra informacao do comando 4
*/
void find_com4_info (INFO *dados_recebidos, char* argv[], int argc){
    int pos_LO, pos_LR, pos_ROTAS, vezes_ponto = 0;

    find_command(&pos_LO, argv, 0, argc);
    find_command(&pos_LR, argv, 5, argc);
    find_command(&pos_ROTAS, argv, 4, argc);

    /*Verificacoes numero parametros*/
    if(argc == 6 && verifica_numero(argv, pos_ROTAS+1, 4, &vezes_ponto) == 1){
        printf("ERRO: Parametros do comando errados\n");
        EXIT(1);
    }
    else if(argc == 7 && verifica_numero(argv, pos_ROTAS+1, 4, &vezes_ponto) == 0){
        printf("ERRO: Parametros do comando errados\n");
        EXIT(1);
    }

    /*Verificacoes para posicoes validas; Nome fichiro nao pode comecar por '-'*/
    if(pos_ROTAS < argc-1){
        dados_recebidos->nome_rota = (int) atof (argv[pos_ROTAS+1]);
    }
    if(pos_LO < argc-1 && argv[pos_LO+1][0]!='-' && pos_LR < argc-1 && argv[pos_LR+1][0]!='-'){
        strcpy(dados_recebidos->output1, argv[pos_LO+1]);
        strcpy(dados_recebidos->output2, argv[pos_LR+1]);
    }
    else{
        printf("ERRO: parametros do comando errados\n");
        EXIT(1);
    }    
}
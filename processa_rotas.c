#include "parametros.h"

int main(int argc, char* argv[]){
    /*aloca memoria necessaria*/
    ITEM_LOCAL* topo_localidades = (ITEM_LOCAL*)malloc(sizeof(ITEM_LOCAL));
    ITEM_ROTA* topo_rotas = (ITEM_ROTA*)malloc(sizeof(ITEM_ROTA));
    INFO* dados_recebidos = (INFO*)malloc(sizeof(INFO));

    /*define comando*/
    int codigo = 0;
    define_com (argv, argc, &codigo);

    /*recolhe informacao consoante comando*/
    if (codigo == 1) find_com1_info (dados_recebidos, argv, argc);
    else if (codigo == 2) find_com2_info (dados_recebidos, argv, argc);
    else if (codigo == 3) find_com3_info (dados_recebidos, argv, argc);
    else if (codigo == 4) find_com4_info (dados_recebidos, argv, argc);

    /*executa comando recebido caso este seja reconhecido com valido*/
    executa(codigo, *dados_recebidos, &topo_localidades, &topo_rotas);

    return 0;
}
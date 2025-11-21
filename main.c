/*
    --------    MANIPULADOR DE INVENTÁRIOS E ESTOQUES   --------
    --------    FEITO EM PURO ANSI C                    --------    
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>   // adição da função Sleep para pausas periódicas
#include "index.h"     // importação das funções presentes no arquivo de cabeçalho

int main() {
    FILE *io_data = fopen("io_data.bin", "r+b");
    if (!io_data) {                                         // se arquivo não existir, cria um novo...
        io_data = fopen("io_data.bin", "w+b");
    }
    inventory Inventario = malloc(sizeof(struct node));  // variável de tipo artificial para o inventário
    inventory Secundario = malloc(sizeof(struct node));   // inventário secundário
    Inventario->next = null;                               // inicializa o primeiro nó
    Secundario->next = null;
    int OP, Contador = 0;                                 // variavel de escolha de opção do usuario e um contador
    unsigned int qtd;                                   // variavel de quantidade de itens

    //menu que se trata de um inventario simples com opções de visualizar, adicionar e remover itens etc.
    printf("\nOla, seja bem-vindo, por favor, escolha uma opcao:\n");       Sleep(500);
    do {
        Sleep(150);
        printf("\n==============MENU===============\n");                    // menu de opções
        printf("1 - Visualizar Inventario\n");
        printf("2 - Adicionar Item ao Inventario\n");                   
        printf("3 - Remover Item do Inventario\n");                     
        printf("4 - Verificar Tamanho do Banco\n");
        printf("5 - Criar Registro\n");
        printf("0 - Sair\n");
        printf("=================================\n"); Sleep(250);
        printf("Digite aqui sua opcao: ");      
        scanf("%d", &OP);
        switch (OP)
        {
        case 0: 
            printf("\nSaindo...\n");
            Sleep(1000);
            return 0;
        case 1:
            printf("\n\033[3mVisualizando Inventario...\033[0m\n");       Sleep(500);
            line_break(0);   
            inventory_view();
            break;
        case 2:
            printf("Quantos items deseja adicionar? ");   Sleep(1000);
            scanf("%d", &qtd);
            line_break(0);
            inventory_add(&Inventario, qtd);                            
            break;
        case 3:
            printf("Removendo item da lista...\n");     Sleep(1000);
            inventory_remove(Inventario);
            printf("Item removido com sucesso!\n");
            break;
        case 4:
            printf("\nVerificando arquivo...\n");     Sleep(800);
            printf("Quantidade de items no estoque: \033[33m%ld\033[0m\n", inventory_size());       
            break;
        case 5:
            printf("\nGerando arquivo de registro...\n");   Sleep(500);
            inventory_report(io_data);
            break;
        default:
            break;
        }
    } while (OP!=0);
    
    fclose(io_data);
    free(Inventario);
    return 0;
}
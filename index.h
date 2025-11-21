/*
========= ARQUIVO DE CABEÇALHO =========
=--------------------------------------=
=  UTILIZADO PARA DECLARAR AS FUNÇÕES  =
=  FORA DO ARQUIVO PRINCIPAL           =
=--------------------------------------=
========================================
*/

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#define null NULL
#define ITEM_SIZE 64
#define VAL_SIZE 12

typedef struct node {
    char *item;
    unsigned int value;
    char *val_date;
    struct node* next;
} *inventory;


/* --- prototypes --- */
void inventory_view();                                           //visualização do inventário
void inventory_report();                                        //relatório em .txt
void inventory_add(inventory *Inventario, unsigned int qtd);     //adiciona items no inventário
inventory inventory_remove(inventory *Inventario);   //remove o primeiro item adicionado na fila
long inventory_size();                                           //verifica quantos items há cadastrado no inventário
char *read_string(FILE *io_data);
void write_string(FILE *io_data, const char *s);
void line_break(unsigned int x);
void limpaBuffer();
/* --- end/prototypes --- */



/* 
============================
==== FUNÇÕES PRINCIPAIS ====
============================
-- São as principais funções a serem utilizadas
*/

void inventory_view() {
    FILE *io_data = fopen("io_data.bin", "rb");
    if (!io_data) {
        perror("Erro ao abrir...");             // verifica tentativa de abrir arquivo, pois no Ansi C não existe try/catch
        return;
    }

    fseek(io_data, 0, SEEK_END);            // ir para o fim para saber o tamanho do arquivo
    long file_size = ftell(io_data);

    if (file_size == 0) {                               // se não tiver nada no arquivo...
        printf("\033[33mINVENTARIO VAZIO!\033[0m\n");    // imprime diagnóstico do inventário
        fclose(io_data);
        return;
    }

    fseek(io_data, 0, SEEK_SET);        // voltar ao início
    printf("===============MERCADORIAS=================\n");
    while (ftell(io_data) < file_size) {
        char item[64];
        char val_date[12];
        unsigned int value;
        int c, i = 0;

        /* ========= LER ITEM ========= */
        while ((c = fgetc(io_data)) != '\0' && c != EOF && i < 63) {
            item[i++] = c;
        }
        if (c == EOF) break;

        item[i] = '\0';

        /* ========= LER QUANTIDADE ========= */
        if (fread(&value, sizeof(unsigned int), 1, io_data) != 1) break;

        /* ========= LER DATA ========= */
        i = 0;
        while ((c = fgetc(io_data)) != '\0' && c != EOF && i < 11) {
            val_date[i++] = c;
        }
        if (c == EOF) break;
        val_date[i] = '\0';

        /* ========= IMPRIMIR ========= */
        printf("Mercadoria: %s;\n %u restantes --> validade: %s\n", item, value, val_date);
    }
    printf("=============FIM-MERCADORIAS===============\n");
    line_break(0);
    fclose(io_data);
}


void inventory_add(inventory *Inventario, unsigned int qtd) {
    FILE *io_data = fopen("io_data.bin", "ab");
    if (!io_data) {
        perror("Erro ao abrir...");
        return;
    }

    inventory aux = *Inventario;
    int i = 1;

    while (i <= qtd) {
        aux->item = malloc(64);
        aux->val_date = malloc(12);
        limpaBuffer();
        /* ======== ITEM ======== */
        printf("%do item a ser colocado: ", i);
        fgets(aux->item, 64, stdin);
        aux->item[strcspn(aux->item, "\n")] = 0; // remove '\n'
        if (strcmp(aux->item, "0") == 0) {
            printf("Saindo do programa...\n");
            free(aux->item);
            free(aux->val_date);
            break;
        }
        fwrite(aux->item, sizeof(char), strlen(aux->item) + 1, io_data);

        /* ======== QUANTIDADE ======== */
        printf("Quantidade no estoque: ");
        scanf("%u", &aux->value);
        fwrite(&aux->value, sizeof(unsigned int), 1, io_data);
        limpaBuffer();

        /* ======== DATA ======== */
        printf("Data de validade (dd/mm/aa): ");
        fgets(aux->val_date, 12, stdin);
        aux->val_date[strcspn(aux->val_date, "\n")] = 0;
        fwrite(aux->val_date, sizeof(char), strlen(aux->val_date) + 1, io_data);

        /* ======== CRIA PRÓXIMO NÓ ======== */
        aux->next = malloc(sizeof(struct node));  
        aux = aux->next;
        aux->next = NULL;

        printf("Item adicionado com sucesso!\n");
        i++;
        line_break(1);
    }

    fclose(io_data);
}


inventory inventory_remove(inventory *Inventario) {
    FILE *io_data = fopen("io_data.bin", "rb");
    if (!io_data) {
        printf("Erro ao abrir arquivo!\n");
        return NULL;
    }


    inventory head = NULL;
    inventory *tail = &head;


    while (!feof(io_data)) {
        int len;
        if (fread(&len, sizeof(int), 1, io_data) != 1) break;
        if (len <= 0) break;


        inventory node = malloc(sizeof(struct node));
        node->item = malloc(len);
        fread(node->item, 1, len, io_data);


        fread(&node->value, sizeof(unsigned int), 1, io_data);


        fread(&len, sizeof(int), 1, io_data);
        node->val_date = malloc(len);
        fread(node->val_date, 1, len, io_data);


        node->next = NULL;
        *tail = node;
        tail = &node->next;
    }


    fclose(io_data);


    if (!head) {
        printf("Inventario vazio. Nada a remover.\n");
        return NULL;
    }


    inventory removed = head;
    head = head->next;


    FILE *out = fopen("io_data.bin", "wb");
    inventory current = head;
    while (current) {
        write_string(out, current->item);
        fwrite(&current->value, sizeof(unsigned int), 1, out);
        write_string(out, current->val_date);
        current = current->next;
    }
    fclose(out);


    printf("Item removido com sucesso!\n");
    return removed;
}


long inventory_size() {
    FILE *io_data = fopen("io_data.bin", "rb");
    if (!io_data) {
        perror("Erro ao abrir...");
        return -1;
    }

    long count = 0;
    int c;
    long file_size;

    // pegar tamanho do arquivo
    fseek(io_data, 0, SEEK_END);
    file_size = ftell(io_data);
    fseek(io_data, 0, SEEK_SET);

    if (file_size == 0) {
        fclose(io_data);
        return 0;
    }

    while (ftell(io_data) < file_size) {
        char ch;

        /* ======== LER STRING DO ITEM ======== */
        // ler até encontrar '\0'
        while ((c = fgetc(io_data)) != '\0' && c != EOF) {}
        if (c == EOF) break;

        /* ======== LER QUANTIDADE ======== */
        unsigned int value;
        if (fread(&value, sizeof(unsigned int), 1, io_data) != 1)
            break;

        /* ======== LER DATA ======== */
        // ler até '\0' denovo
        while ((c = fgetc(io_data)) != '\0' && c != EOF) {}
        if (c == EOF) break;

        count++;
    }

    fclose(io_data);
    return count;
}


void inventory_report(FILE *fp) {
    if (!fp) {
        printf("Arquivo inexistente.\n");
        return;
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (file_size == 0) {
        printf("Nenhum registro para relatorio.\n");
        return;
    }

    FILE *rel = fopen("relatorio.txt", "w");
    if (!rel) {
        printf("Erro ao criar arquivo de relatorio.\n");
        return;
    }

    fprintf(rel, "========== RELATORIO DE ITENS ==========\n\n");

    while (ftell(fp) < file_size) {

        /* ====== LER NOME ====== */
        int len_item;
        if (fread(&len_item, sizeof(int), 1, fp) != 1) break;

        char *item = malloc(len_item);
        fread(item, 1, len_item, fp);

        /* ====== LER QUANTIDADE ====== */
        unsigned int value;
        fread(&value, sizeof(unsigned int), 1, fp);

        /* ====== LER VALIDADE ====== */
        int len_data;
        fread(&len_data, sizeof(int), 1, fp);

        char *val_date = malloc(len_data);
        fread(val_date, 1, len_data, fp);

        /* ====== ESCREVER NO RELATORIO ====== */
        fprintf(rel, "Item: %s\n", item);
        fprintf(rel, "Quantidade: %u\n", value);
        fprintf(rel, "Validade: %s\n", val_date);
        fprintf(rel, "------------------------------\n\n");

        free(item);
        free(val_date);
    }

    fclose(rel);

    printf("Relatorio gerado em relatorio.txt\n");
}



/* 
============================
==== FUNÇÕES AUXILIARES ====
============================
-- São funções que auxiliam no código, sendo elas essenciais ou não
*/


char *read_string(FILE *io_data) {
    int len;
    fread(&len, sizeof(int), 1, io_data);
    if (len <= 0) return NULL;

    char *s = malloc(len);
    fread(s, 1, len, io_data);
    return s;
}

void write_string(FILE *io_data, const char *s) {
    int len = strlen(s) + 1;
    fwrite(&len, sizeof(int), 1, io_data);
    fwrite(s, 1, len, io_data);
}

/* Imprime quebras de linha com delay */
void line_break(unsigned int x) {
    if (x == 0) x = 1;
    while (x > 0) {
        printf("\n");
        Sleep(500);
        x--;
    }
}


/* Limpa buffer do stdin (descarta até \n) */
void limpaBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}



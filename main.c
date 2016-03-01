/**********************************
 * TODO
 * main.c
 *
 * Author: Topher Zero
 **********************************/

#include <stdio.h>
#include <stdlib.h>

#define FILEPATH ".todo"

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define RESET   "\x1b[0m"

#define DONE "\x1b[32m\xE2\x9C\x93\x1b[0m"
#define TODO "\x1b[31m\xE2\x9C\x95\x1b[0m"

typedef enum Mode_t {
    List, Append, Done, Fail
} Mode_t;

typedef struct item_t {
    char *description;
    int length;
    int done;
    struct item_t *next;  
} item_t;



item_t *load_list(const char *file);
item_t *readline(FILE *fp);
void save_list(const char *file, item_t **head);
void list_items(item_t *head);
item_t *new_item(const char *description, int des_len, int done);
void append_item(item_t *head, const char *item);

/* int list_items(const char *file); */
/* void print_item(const char *item, int state); */

int main(int argc, const char *argv[])
{
    Mode_t mode = 
            argc == 1 ? List
        :   argc == 2 ? Append
        :   argc == 3 ? Done
        :               Fail;
        
    item_t *head = load_list(FILEPATH);

    switch (mode) {
        case List:
            list_items(head);
            break;
        case Append:
            append_item(head, argv[1]);
            break;
        case Done:
            break;
        case Fail:
            break;
    }

    save_list(FILEPATH, &head);

    return 0;
}

item_t *load_list(const char *file)
{
    FILE *fp = fopen(file, "r");
    if (!fp) {
        fp = fopen(file, "a");
        return NULL;
    }

    item_t *head = readline(fp);
    item_t *ptr = head;

    while (ptr) {
        ptr->next = readline(fp);
        ptr = ptr->next;
    }

    fclose(fp);
    return head;
}

item_t *readline(FILE *fp)
{
    int done = 0;
    int i = 0;
    int ch;
    char buffer[BUFSIZ];

    while ((ch = fgetc(fp)) != EOF && ch != '\n') {
        if (i == 0 && ch == '!') {
           done = 1;
           i++;
        }
        else {
            buffer[i++] = ch;
        }
    }

    if (ch == EOF) {
        return NULL;
    }
    
    buffer[i++] = '\0';
    
    return new_item(buffer, i, done);
}

void save_list(const char *file, item_t **head)
{
    FILE *fp = fopen(file, "w");
    if (!fp) {
        printf("file error");
        return;
    }

    item_t *ptr = *head;
    item_t *prev = ptr;

    while (ptr) {
        if (ptr->done) {
            fprintf(fp, "!");
        }

        fprintf(fp, "%s\n", ptr->description); 
        free(ptr->description);
        ptr->description = NULL;

        prev = ptr;
        ptr = ptr->next;
        free(prev);
    }
    fclose(fp);
    *head = NULL;
}

void append_item(item_t *head, const char *item)
{
    item_t *ptr = head;

    while (ptr->next) {
        ptr = ptr->next;
    }

    int i = 0;
    while (item[i++] != '\0');

    ptr->next = new_item(item, i, 0);
}

item_t *new_item(const char *description, int des_len, int done)
{
    item_t *item = malloc(sizeof(item_t));

    item->next = NULL;
    item->done = done;
    item->description = malloc(des_len+1);

    for (int i = 0; i <= des_len; i++) {
        item->description[i] = description[i];
    }

    return item;
}

void list_items(item_t *head)
{
    item_t *ptr = head;
    while (ptr) {
        printf("[");
        printf(ptr->done ? DONE: TODO);
        printf("] ");
        printf("%s\n", ptr->description);

        ptr = ptr->next;
    }
}

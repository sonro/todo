/**********************************
 * TODO
 * main.c
 *
 * Author: Topher Zero
 **********************************/
static const char USAGE[] =
    "USAGE:\n"
    "todo           (list todo tasks)\n"
    "todo <task>    (add task to list)\n"
    "todo -d <id>   (mark task as done)\n"
    "todo -u <id>   (undo done task)\n"
    "todo -r <id>   (remove task)\n"
    "todo -a        (show all tasks\n"
    "todo -e        (add multiple tasks)\n"
    "todo -c        (clear task list)\n"
    "todo -h        (help)\n";

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#define FILEPATH ".todo"

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define RESET   "\x1b[0m"

#define DONE "\x1b[32m\xE2\x9C\x93\x1b[0m"
#define TODO "\x1b[31m\xE2\x9C\x95\x1b[0m"

typedef enum Mode_t {
    List, ListAll, Edit, Clear, Help, Append, Done, Undo, Remove, Fail
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
void list_items(item_t *head, int list_all);
item_t *new_item(const char *description, int des_len, int done);
void append_item(item_t *head, char *const *argv);
void check_item(item_t *head, int id, int done);
void remove_item(item_t **head, int id);
void clear_list(item_t *head);
char *new_description(const char *buffer, int length);

int main(int argc, char *const argv[])
{
    Mode_t mode;
    
    int c;
    int id;
    opterr = 0;

    if (argc == 1) {
        mode = List;
    }
    else if (argv[1][0] != '-') {
        mode = Append;
    }
    else {
        while ((c = getopt(argc, argv, "aehcd:u:r:")) != -1) {
            switch (c) {
                case 'a':
                    mode = ListAll;
                    break;
                case 'e':
                    mode = Edit;
                    break;
                case 'h':
                    mode = Help;
                    printf("%s", USAGE);
                    return 0;
                case 'c':
                    mode = Clear;
                    break;
                case 'd':
                    mode = Done;
                    if (sscanf(optarg, " %d", &id) != 1) {
                        printf("Invalid ID %s\n", optarg);
                        return 1;
                    }
                    break;
                case 'u':
                    mode = Undo;
                    if (sscanf(optarg, " %d", &id) != 1) {
                        printf("Invalid ID %s\n", optarg);
                        return 1;
                    }
                    break;
                case 'r':
                    mode = Remove;
                    if (sscanf(optarg, " %d", &id) != 1) {
                        printf("Invalid ID %s\n", optarg);
                        return 1;
                    }
                    break;
                case '?':
                    if (optopt == 'd' || optopt == 'u' || optopt == 'r') {
                        fprintf(stderr, "Option -%c requires an id.\n", optopt);
                    }
                    else if (isprint(optopt)) {
                        fprintf(stderr, "Unknown option '-%c'.\n", optopt);
                    }
                    else {
                        fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
                    }
                    printf("%s", USAGE);
                    return 1;
                    break;
                default:
                    return 1;
            }
        }
    }
        
    item_t *head = load_list(FILEPATH);
    
    switch (mode) {
        case List:
            list_items(head, 0);
            break;
        case ListAll:
            list_items(head, 1);
            break;
        case Append:
            append_item(head, argv);
            break;
        case Done:
            check_item(head, id, 1);
            break;
        case Undo:
            check_item(head, id, 0);
            break;
        case Remove:
            remove_item(&head, id);
            break;
        case Edit:
            break;
        case Clear:
            clear_list(head);
            break;
        case Fail:
            break;
        case Help:
            break;
    }

    save_list(FILEPATH, &head);

    return 0;
}

void remove_item(item_t **head, int id)
{
    item_t *ptr = *head;
    item_t *prev = ptr;

    if (id == 1) {
        ptr = ptr->next;
        if (prev->description) {
            free(prev->description);
        }
        prev->next = NULL;
        free(prev);
        *head = ptr;
    }
    else {
        for (int i = 1; i < id; i++) {
            prev = ptr;
            ptr = ptr->next;
            if (ptr == NULL) {
                printf("Error: Not a valid TODO id\n");
                return;
            }
        }

        prev->next = ptr->next;

        free(ptr->description);
        ptr->description = NULL;
        ptr->next = NULL;
        free(ptr);
    }
}

void check_item(item_t *head, int id, int done)
{
    item_t *ptr = head;
    for (int i = 1; i < id; i++) {
        ptr = ptr->next;
        if (ptr == NULL) {
            printf("Error: Not a valid TODO id\n");
            return;
        }
    }

    ptr->done = done;
}

item_t *load_list(const char *file)
{
    FILE *fp = fopen(file, "r");
    if (!fp) {
        fp = fopen(file, "a");
        return NULL;
    }

    item_t *head = readline(fp);
    if (!head) {
        fclose(fp);
        return new_item(NULL, 0, 0);
    }
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

    if ((*head)->next != NULL) {
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
    }
    else {
        if ((*head)->description) {
            fprintf(fp, "%s\n", (*head)->description);
            free((*head)->description);
            (*head)->description = NULL;
        }
        free(*head);
    }
    fclose(fp);
    *head = NULL;
}

void append_item(item_t *head, char *const *argv)
{
    item_t *ptr = head;

    while (ptr->next) {
        ptr = ptr->next;
    }
    char buffer[BUFSIZ];
    int i, j, k = 0;
    for (i = 1; argv[i] != NULL; i++) {
        for (j = 0; argv[i][j] != '\0'; j++) {
            buffer[k++] = argv[i][j];
        }
        buffer[k++] = ' ';
    }
    buffer[k++] = '\0';
    if (ptr->description) {
    ptr->next = new_item(buffer, k, 0);
    }
    else {
        ptr->description = new_description(buffer, k);
    }
}

item_t *new_item(const char *description, int des_len, int done)
{
    item_t *item = malloc(sizeof(item_t));

    item->description = 
        description ? new_description(description, des_len) 
                    : NULL;

    item->next = NULL;
    item->done = done;

    return item;
}

char *new_description(const char *buffer, int length)
{
    char *description = malloc(length+1);

    for (int i = 0; i <= length; i++) {
        description[i] = buffer[i];
    }

    return description;
}

void list_items(item_t *head, int list_all)
{
    item_t *ptr = head;
    int i = 1;
    while (ptr) {
        if (ptr->description) {
            if (list_all || !ptr->done) {
                printf("%d: ", i);
                printf("[");
                printf(ptr->done ? DONE: TODO);
                printf("] ");
                printf("%s\n", ptr->description);
            }
        }
        i++;
        ptr = ptr->next;
    }
}

void clear_list(item_t *head)
{
    item_t *ptr = head;
    item_t *prev = ptr;
    while (ptr) {
        prev = ptr;
        ptr = ptr->next; 
        free(prev->description);
        prev->description = NULL;
        prev->next = NULL;
    }
}

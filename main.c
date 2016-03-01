/**********************************
 * TODO
 * main.c
 *
 * Author: Topher Zero
 **********************************/

#include <stdio.h>

#define FILEPATH ".todo"

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define RESET   "\x1b[0m"

#define DONE "\x1b[32m\xE2\x9C\x93\x1b[0m"
#define TODO "\x1b[31m\xE2\x9C\x95\x1b[0m"

typedef enum Mode_t {
    List, Append, Done, Fail
} Mode_t;

/* typedef struct item { */
/*     char *description; */
/*     int length; */
/*     int done; */
/*     struct item *next; */  
/* } Item_t; */

int append_item(const char *file, const char *item);
int list_items(const char *file);
void print_item(const char *item, int state);

int main(int argc, const char *argv[])
{
    Mode_t mode = 
            argc == 1 ? List
        :   argc == 2 ? Append
        :   argc == 3 ? Done
        :               Fail;
        
    /* int rv; */

    switch (mode) {
        case List:
            list_items(FILEPATH);
            break;
        case Append:
            append_item(FILEPATH, argv[1]);
            break;
        case Done:
            break;
        case Fail:
            break;
    }

    return 0;
}

int append_item(const char *file, const char *item)
{
    FILE *fp = fopen(file, "a");
    if (!fp) {
        printf("file error");
        return -1;
    }

    fprintf(fp, "0%s\n", item);

    fclose(fp);

    return 1;
}

int list_items(const char *file)
{
    FILE *fp = fopen(file, "r");
    if (!fp) {
        printf("No items todo at the moment\n");
        return -1;
    }

    int reading = 1;

    int ch;
    int state;
    char buffer[BUFSIZ];
    int i = -1;

    while (reading) {
        ch = fgetc(fp);

        if (ch == EOF) {
            buffer[i] = '\0';
            if (i > -1) {
                print_item(buffer, state);
            }
            reading = 0;
        }
        else if (ch == '\n') {
            buffer[i] = '\0';
            print_item(buffer, state);
            i = -1;
        }
        else if (i == -1) {
            state = ch - '0'; 
            i++;
        }
        else {
            buffer[i++] = ch;
        }
    }

    fclose(fp);
    return 0;
}

void print_item(const char *item, int state)
{
    printf("[");
    printf(state ? DONE: TODO);
    printf("] ");
    printf("%s\n", item);
}

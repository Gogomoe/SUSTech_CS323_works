#include "symtab.h"

/*
 * symbol table type, hash table (close addressing) impl
 */
#define TABLE_SIZE 0x1003
struct _node {
    entry entry;
    struct _node *next;
};

//typedef struct _node *symtab[TABLE_SIZE];

struct symtab {
    struct _node *nodes[TABLE_SIZE];
};


// ************************************************************
//    Your implementation goes here
// ************************************************************

unsigned int symtab_hash(char *key) {
    if (key[0] == '\0') {
        return 0;
    }
    unsigned int hash = (unsigned char) key[0];
    key++;
    while (key[0] != '\0') {
        hash <<= 8u;
        hash += key[0];
        hash %= TABLE_SIZE;
        key++;
    }
    return hash;
}

symtab *symtab_init() {
    symtab *self = malloc(sizeof(symtab));
    memset(self, '\0', sizeof(symtab));
    for (int i = 0; i < TABLE_SIZE; i++) {
        self->nodes[i] = NULL;
    }
    return self;
}

int symtab_insert(symtab *self, char *key, VAL_T value) {
    unsigned int hash = symtab_hash(key);
    if (self->nodes[hash] == NULL) {
        struct _node *head = malloc(sizeof(struct _node));
        memset(head, '\0', sizeof(struct _node));
        head->next = NULL;

        self->nodes[hash] = head;
    }
    struct _node *head = self->nodes[hash];
    while (head->next != NULL) {
        if (strcmp(head->entry.key, key) == 0)
            return 0;
        head = head->next;
    }
    if (strcmp(head->entry.key, key) == 0) {
        return 0;
    }
    struct _node *node = malloc(sizeof(struct _node));
    memset(node, '\0', sizeof(struct _node));

    entry_init(&node->entry, key, value);
    node->next = NULL;
    head->next = node;

    return 1;
}

VAL_T symtab_lookup(symtab *self, char *key) {
    unsigned int hash = symtab_hash(key);
    if (self->nodes[hash] == NULL) {
        return -1;
    }

    struct _node *head = self->nodes[hash];
    while(head != NULL){
        if(strcmp(head->entry.key, key) == 0)
            return head->entry.value;
        head = head->next;
    }
    return -1;
}

int symtab_remove(symtab *self, char *key) {
    unsigned int hash = symtab_hash(key);
    if (self->nodes[hash] == NULL) {
        return 0;
    }

    struct _node *head = self->nodes[hash], *tmp;
    while(head->next != NULL) {
        if(strcmp(head->next->entry.key, key) == 0){
            tmp = head->next;
            head->next = head->next->next;
            free(tmp);
            return 1;
        }
        head = head->next;
    }
    return 0;
}

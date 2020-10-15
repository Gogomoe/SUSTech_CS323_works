#include "symtab.h"

/*
 * symbol table type, binary tree impl
 */
struct symtab {
    entry entry;
    struct symtab *left, *right;
};

// ************************************************************
//    Your implementation goes here
// ************************************************************

symtab *symtab_init() {
    symtab *self = malloc(sizeof(symtab));
    memset(self, '\0', sizeof(symtab));
    self->left = NULL;
    self->right = NULL;
    return self;
}

int symtab_insert(symtab *self, char *key, VAL_T value) {
    symtab *parent = self;
    while (self != NULL) {
        parent = self;
        int cmp = strcmp(self->entry.key, key);
        if (cmp == 0) {
            return 0;
        } else if (cmp < 0) {
            self = self->left;
        } else {
            self = self->right;
        }
    }
    symtab *node = malloc(sizeof(symtab));
    memset(node, '\0', sizeof(symtab));
    entry_init(&node->entry, key, value);
    node->left = NULL;
    node->right = NULL;

    int cmp = strcmp(parent->entry.key, key);
    if (cmp < 0) {
        parent->left = node;
    } else {
        parent->right = node;
    }

    return 1;
}

VAL_T symtab_lookup(symtab *self, char *key) {
    while (self != NULL) {
        int cmp = strcmp(self->entry.key, key);
        if (cmp == 0) {
            return self->entry.value;
        } else if (cmp < 0) {
            self = self->left;
        } else {
            self = self->right;
        }
    }
    return -1;
}

int symtab_remove(symtab *self, char *key) {
    symtab *parent = self;
    while (self != NULL) {
        int cmp = strcmp(self->entry.key, key);
        if (cmp == 0) {
            break;
        } else if (cmp < 0) {
            parent = self;
            self = self->left;
        } else {
            parent = self;
            self = self->right;
        }
    }
    if (self == NULL){
        return 0;
    }
    int cmp = strcmp(parent->entry.key, key);
    if (self->left == NULL) {
        if (cmp < 0) {
            parent->left = self->right;
        } else {
            parent->right = self->right;
        }
        free(self);
        return 1;
    } else if (self->right == NULL) {
        if (cmp < 0) {
            parent->left = self->left;
        } else {
            parent->right = self->left;
        }
        free(self);
        return 1;
    } else {
        symtab *rmv = self->left;
        while (rmv->right != NULL) {
            rmv = rmv->right;
        }
        char* rmv_key = strdup(rmv->entry.key);
        VAL_T value = rmv->entry.value;

        symtab_remove(self, rmv_key);

        sprintf(self->entry.key, "%s", rmv_key);
        self->entry.value = value;

        free(rmv_key);
        return 1;
    }
}

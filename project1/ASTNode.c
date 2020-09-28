#include <malloc.h>
#include <string.h>
#include "ASTNode.h"

struct ASTNode *make_int_node(char *name, int value) {
    struct ASTNode *node = malloc(sizeof(struct ASTNode));
    node->type = INT_LEAF;
    node->name = strdup(name);
    node->int_value = value;
    return node;
}

struct ASTNode *make_float_node(char *name, float value) {
    struct ASTNode *node = malloc(sizeof(struct ASTNode));
    node->type = FLOAT_LEAF;
    node->name = strdup(name);
    node->float_value = value;
    return node;
}

struct ASTNode *make_string_node(char *name, char *value) {
    struct ASTNode *node = malloc(sizeof(struct ASTNode));
    node->type = STRING_LEAF;
    node->name = strdup(name);
    node->string_value = strdup(value);
    return node;
}
#include <malloc.h>
#include <string.h>
#include "ASTNode.h"

struct ASTNode *make_empty_node(char *name) {
    struct ASTNode *node = calloc(1, sizeof(struct ASTNode));
    node->type = EMPTY_LEAF;
    node->name = strdup(name);
    return node;
}

struct ASTNode *make_int_node(char *name, int value) {
    struct ASTNode *node = calloc(1, sizeof(struct ASTNode));
    node->type = INT_LEAF;
    node->name = strdup(name);
    node->int_value = value;
    return node;
}

struct ASTNode *make_float_node(char *name, float value) {
    struct ASTNode *node = calloc(1, sizeof(struct ASTNode));
    node->type = FLOAT_LEAF;
    node->name = strdup(name);
    node->float_value = value;
    return node;
}

struct ASTNode *make_string_node(char *name, char *value) {
    struct ASTNode *node = calloc(1, sizeof(struct ASTNode));
    node->type = STRING_LEAF;
    node->name = strdup(name);
    node->string_value = strdup(value);
    return node;
}

struct ASTNode *make_internal_node0(
        char *name
) {
    struct ASTNode *node = calloc(1, sizeof(struct ASTNode));
    node->type = INTERNAL_NODE;
    node->name = strdup(name);
    node->child = NULL;
    return node;
}

struct ASTNode *make_internal_node1(
        char *name,
        struct ASTNode *node1) {
    struct ASTNode *node = calloc(1, sizeof(struct ASTNode));
    node->type = INTERNAL_NODE;
    node->name = strdup(name);
    node->child = node1;
    return node;
}

struct ASTNode *make_internal_node2(
        char *name,
        struct ASTNode *node1,
        struct ASTNode *node2
) {
    struct ASTNode *node = calloc(1, sizeof(struct ASTNode));
    node->type = INTERNAL_NODE;
    node->name = strdup(name);
    node->child = node1;
    node1->next = node2;
    return node;
}

struct ASTNode *make_internal_node3(
        char *name,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3
) {
    struct ASTNode *node = calloc(1, sizeof(struct ASTNode));
    node->type = INTERNAL_NODE;
    node->name = strdup(name);
    node->child = node1;
    node1->next = node2;
    node2->next = node3;
    return node;
}

struct ASTNode *make_internal_node4(
        char *name,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3,
        struct ASTNode *node4
) {
    struct ASTNode *node = calloc(1, sizeof(struct ASTNode));
    node->type = INTERNAL_NODE;
    node->name = strdup(name);
    node->child = node1;
    node1->next = node2;
    node2->next = node3;
    node3->next = node4;
    return node;
}

struct ASTNode *make_internal_node5(
        char *name,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3,
        struct ASTNode *node4,
        struct ASTNode *node5
) {
    struct ASTNode *node = calloc(1, sizeof(struct ASTNode));
    node->type = INTERNAL_NODE;
    node->name = strdup(name);
    node->child = node1;
    node1->next = node2;
    node2->next = node3;
    node3->next = node4;
    node4->next = node5;
    return node;
}


struct ASTNode *make_internal_node6(
        char *name,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3,
        struct ASTNode *node4,
        struct ASTNode *node5,
        struct ASTNode *node6
) {
    struct ASTNode *node = calloc(1, sizeof(struct ASTNode));
    node->type = INTERNAL_NODE;
    node->name = strdup(name);
    node->child = node1;
    node1->next = node2;
    node2->next = node3;
    node3->next = node4;
    node4->next = node5;
    node5->next = node6;
    return node;
}

struct ASTNode *make_internal_node7(
        char *name,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3,
        struct ASTNode *node4,
        struct ASTNode *node5,
        struct ASTNode *node6,
        struct ASTNode *node7
) {
    struct ASTNode *node = calloc(1, sizeof(struct ASTNode));
    node->type = INTERNAL_NODE;
    node->name = strdup(name);
    node->child = node1;
    node1->next = node2;
    node2->next = node3;
    node3->next = node4;
    node4->next = node5;
    node5->next = node6;
    node6->next = node7;
    return node;
}

void print_ASTTree(struct ASTNode *root) {
    __print_ASTTree(root, 0);
}

void __print_ASTTree(struct ASTNode *root, int level) {
    if (root == NULL) {
        printf("NullPointException in __print_ASTTree");
        return;
    }

    int should_not_print_self = root->type == INTERNAL_NODE && root->child == NULL;

    if (!should_not_print_self) {

        for (int i = 0; i < level; i++) {
            printf("  ");
        }
        printf("%s", root->name);

        if (root->type != EMPTY_LEAF && root->type != INTERNAL_NODE) {
            printf(": ");
        }
        if (root->type == INT_LEAF) {
            printf("%d", root->int_value);
        } else if (root->type == FLOAT_LEAF) {
            printf("%f", root->float_value);
        } else if (root->type == STRING_LEAF) {
            printf("%s", root->string_value);
        }

        printf("\n");

    }

    if (root->type == INTERNAL_NODE && root->child != NULL) {
        __print_ASTTree(root->child, level + 1);
    }
    if (root->next != NULL) {
        __print_ASTTree(root->next, level);
    }

}


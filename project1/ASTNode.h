#ifndef AST_H
#define AST_H

enum ASTType {
    EMPTY_LEAF,
    INT_LEAF,
    FLOAT_LEAF,
    STRING_LEAF,
    INTERNAL_NODE,
};

struct ASTNode {
    enum ASTType type;
    char *name;
    struct ASTNode *next;
    union {
        int int_value;
        float float_value;
        char *string_value;
        struct ASTNode *child;
    };
};

struct ASTNode *make_empty_node(char *name);

struct ASTNode *make_int_node(char *name, int value);

struct ASTNode *make_float_node(char *name, float value);

struct ASTNode *make_string_node(char *name, char *value);

struct ASTNode *make_internal_node0(
        char *name
);

struct ASTNode *make_internal_node1(
        char *name,
        struct ASTNode *node1
);

struct ASTNode *make_internal_node2(
        char *name,
        struct ASTNode *node1,
        struct ASTNode *node2
);

struct ASTNode *make_internal_node3(
        char *name,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3
);

struct ASTNode *make_internal_node4(
        char *name,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3,
        struct ASTNode *node4
);

struct ASTNode *make_internal_node5(
        char *name,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3,
        struct ASTNode *node4,
        struct ASTNode *node5
);

struct ASTNode *make_internal_node6(
        char *name,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3,
        struct ASTNode *node4,
        struct ASTNode *node5,
        struct ASTNode *node6
);

struct ASTNode *make_internal_node7(
        char *name,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3,
        struct ASTNode *node4,
        struct ASTNode *node5,
        struct ASTNode *node6,
        struct ASTNode *node7
);

void print_ASTTree(struct ASTNode *root);

void __print_ASTTree(struct ASTNode *root, int level);

#endif /* of AST_H */
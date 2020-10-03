#ifndef AST_H
#define AST_H

#include "YYLTYPE.h"

extern int error_happen;

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
    char *display_value;
    struct ASTNode *next;
    struct YYLTYPE position;
    union {
        int int_value;
        float float_value;
        char *string_value;
        struct ASTNode *child;
    };
};

struct ASTNode *make_empty_node(char *name, struct YYLTYPE position);

struct ASTNode *make_int_node(char *name, struct YYLTYPE position, int value);

struct ASTNode *make_float_node(char *name, struct YYLTYPE position, float value);

struct ASTNode *make_string_node(char *name, struct YYLTYPE position, char *value);

struct ASTNode *make_internal_node0(
        char *name,
        struct YYLTYPE position
);

struct ASTNode *make_internal_node1(
        char *name,
        struct YYLTYPE position,
        struct ASTNode *node1
);

struct ASTNode *make_internal_node2(
        char *name,
        struct YYLTYPE position,
        struct ASTNode *node1,
        struct ASTNode *node2
);

struct ASTNode *make_internal_node3(
        char *name,
        struct YYLTYPE position,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3
);

struct ASTNode *make_internal_node4(
        char *name,
        struct YYLTYPE position,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3,
        struct ASTNode *node4
);

struct ASTNode *make_internal_node5(
        char *name,
        struct YYLTYPE position,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3,
        struct ASTNode *node4,
        struct ASTNode *node5
);

struct ASTNode *make_internal_node6(
        char *name,
        struct YYLTYPE position,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3,
        struct ASTNode *node4,
        struct ASTNode *node5,
        struct ASTNode *node6
);

struct ASTNode *make_internal_node7(
        char *name,
        struct YYLTYPE position,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3,
        struct ASTNode *node4,
        struct ASTNode *node5,
        struct ASTNode *node6,
        struct ASTNode *node7
);

struct ASTNode *make_internal_node8(
        char *name,
        struct YYLTYPE position,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3,
        struct ASTNode *node4,
        struct ASTNode *node5,
        struct ASTNode *node6,
        struct ASTNode *node7,
        struct ASTNode *node8
);

struct ASTNode *make_internal_node9(
        char *name,
        struct YYLTYPE position,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3,
        struct ASTNode *node4,
        struct ASTNode *node5,
        struct ASTNode *node6,
        struct ASTNode *node7,
        struct ASTNode *node8,
        struct ASTNode *node9
);

struct ASTNode *make_internal_node10(
        char *name,
        struct YYLTYPE position,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3,
        struct ASTNode *node4,
        struct ASTNode *node5,
        struct ASTNode *node6,
        struct ASTNode *node7,
        struct ASTNode *node8,
        struct ASTNode *node9,
        struct ASTNode *node10
);

void print_ASTTree(struct ASTNode *root);

void __print_ASTTree(struct ASTNode *root, int level);

#endif /* of AST_H */
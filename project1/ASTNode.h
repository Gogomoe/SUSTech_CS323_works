#ifndef AST_H
#define AST_H

enum ASTType {
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

struct ASTNode *make_int_node(char *name, int value);

struct ASTNode *make_float_node(char *name, float value);

struct ASTNode *make_string_node(char *name, char *value);

#endif /* of AST_H */
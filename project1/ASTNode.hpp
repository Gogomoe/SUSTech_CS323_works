#ifndef AST_H
#define AST_H

#include "YYLTYPE.h"
#include <string>
#include <vector>
#include <initializer_list>

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
    std::string name;
    std::string display_value;
    struct YYLTYPE position;

    int int_value;
    float float_value;
    std::string string_value;

    std::vector<ASTNode *> children;
};

struct ASTNode *make_empty_node(const std::string &name, struct YYLTYPE position);

struct ASTNode *make_int_node(const std::string &name, struct YYLTYPE position, int value);

struct ASTNode *make_float_node(const std::string &name, struct YYLTYPE position, float value);

struct ASTNode *make_string_node(const std::string &name, struct YYLTYPE position, const std::string &value);

struct ASTNode *make_internal_node(
        const std::string &name,
        struct YYLTYPE position,
        std::initializer_list<ASTNode *> children
);

void print_ASTTree(struct ASTNode *root);

void __print_ASTTree(struct ASTNode *root, int level);

#endif /* of AST_H */
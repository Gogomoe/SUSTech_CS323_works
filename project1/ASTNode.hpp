#ifndef AST_H
#define AST_H

#include "YYLTYPE.h"
#include <string>
#include <vector>

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

struct ASTNode *make_internal_node0(
        const std::string &name,
        struct YYLTYPE position
);

struct ASTNode *make_internal_node1(
        const std::string &name,
        struct YYLTYPE position,
        struct ASTNode *node1
);

struct ASTNode *make_internal_node2(
        const std::string &name,
        struct YYLTYPE position,
        struct ASTNode *node1,
        struct ASTNode *node2
);

struct ASTNode *make_internal_node3(
        const std::string &name,
        struct YYLTYPE position,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3
);

struct ASTNode *make_internal_node4(
        const std::string &name,
        struct YYLTYPE position,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3,
        struct ASTNode *node4
);

struct ASTNode *make_internal_node5(
        const std::string &name,
        struct YYLTYPE position,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3,
        struct ASTNode *node4,
        struct ASTNode *node5
);

struct ASTNode *make_internal_node6(
        const std::string &name,
        struct YYLTYPE position,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3,
        struct ASTNode *node4,
        struct ASTNode *node5,
        struct ASTNode *node6
);

struct ASTNode *make_internal_node7(
        const std::string &name,
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
        const std::string &name,
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
        const std::string &name,
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
        const std::string &name,
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
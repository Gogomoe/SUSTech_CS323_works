#include <malloc.h>
#include "ASTNode.hpp"

using std::string;
using std::vector;
using std::initializer_list;

int error_happen = 0;

struct ASTNode *make_empty_node(const string &name, struct YYLTYPE position) {
    struct ASTNode *node = new ASTNode;
    node->type = EMPTY_LEAF;
    node->name = name;
    node->position = position;
    return node;
}

struct ASTNode *make_int_node(const string &name, struct YYLTYPE position, int value) {
    struct ASTNode *node = new ASTNode;
    node->type = INT_LEAF;
    node->name = name;
    node->position = position;
    node->int_value = value;
    return node;
}

struct ASTNode *make_float_node(const string &name, struct YYLTYPE position, float value) {
    struct ASTNode *node = new ASTNode;
    node->type = FLOAT_LEAF;
    node->name = name;
    node->position = position;
    node->float_value = value;
    return node;
}

struct ASTNode *make_string_node(const string &name, struct YYLTYPE position, const string &value) {
    struct ASTNode *node = new ASTNode;
    node->type = STRING_LEAF;
    node->name = name;
    node->position = position;
    node->string_value = value;
    return node;
}

struct ASTNode *make_internal_node(
        const string &name,
        struct YYLTYPE position,
        initializer_list<ASTNode *> children
) {
    struct ASTNode *node = new ASTNode;
    node->type = INTERNAL_NODE;
    node->name = name;
    node->position = position;

    for (auto child : children) {
        node->children.push_back(child);
    }

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

    int should_not_print_self = root->type == INTERNAL_NODE && root->children.empty();

    if (!should_not_print_self) {

        for (int i = 0; i < level; i++) {
            printf("  ");
        }
        printf("%s", root->name.data());

        if (root->type != EMPTY_LEAF && root->type != INTERNAL_NODE) {
            printf(": ");
        }

        if (!root->display_value.empty()) {
            printf("%s", root->display_value.data());
        } else if (root->type == INT_LEAF) {
            printf("%d", root->int_value);
        } else if (root->type == FLOAT_LEAF) {
            printf("%f", root->float_value);
        } else if (root->type == STRING_LEAF) {
            printf("%s", root->string_value.data());
        }

        if (root->type == INTERNAL_NODE) {
            printf(" (%d)", root->position.first_line);
        }

        printf("\n");

    }

    for(auto child: root->children){
        __print_ASTTree(child, level + 1);
    }

}


#include <malloc.h>
#include "ASTNode.hpp"

using std::string;
using std::vector;

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

struct ASTNode *make_internal_node0(
        const string &name,
        struct YYLTYPE position
) {
    struct ASTNode *node = new ASTNode;
    node->type = INTERNAL_NODE;
    node->name = name;
    node->position = position;
    return node;
}

struct ASTNode *make_internal_node1(
        const string &name,
        struct YYLTYPE position,
        struct ASTNode *node1) {
    struct ASTNode *node = new ASTNode;
    node->type = INTERNAL_NODE;
    node->name = name;
    node->position = position;
    node->children.push_back(node1);
    return node;
}

struct ASTNode *make_internal_node2(
        const string &name,
        struct YYLTYPE position,
        struct ASTNode *node1,
        struct ASTNode *node2
) {
    struct ASTNode *node = new ASTNode;
    node->type = INTERNAL_NODE;
    node->name = name;
    node->position = position;
    node->children.push_back(node1);
    node->children.push_back(node2);
    return node;
}

struct ASTNode *make_internal_node3(
        const string &name,
        struct YYLTYPE position,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3
) {
    struct ASTNode *node = new ASTNode;
    node->type = INTERNAL_NODE;
    node->name = name;
    node->position = position;
    node->children.push_back(node1);
    node->children.push_back(node2);
    node->children.push_back(node3);
    return node;
}

struct ASTNode *make_internal_node4(
        const string &name,
        struct YYLTYPE position,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3,
        struct ASTNode *node4
) {
    struct ASTNode *node = new ASTNode;
    node->type = INTERNAL_NODE;
    node->name = name;
    node->position = position;
    node->children.push_back(node1);
    node->children.push_back(node2);
    node->children.push_back(node3);
    node->children.push_back(node4);
    return node;
}

struct ASTNode *make_internal_node5(
        const string &name,
        struct YYLTYPE position,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3,
        struct ASTNode *node4,
        struct ASTNode *node5
) {
    struct ASTNode *node = new ASTNode;
    node->type = INTERNAL_NODE;
    node->name = name;
    node->position = position;
    node->children.push_back(node1);
    node->children.push_back(node2);
    node->children.push_back(node3);
    node->children.push_back(node4);
    node->children.push_back(node5);
    return node;
}


struct ASTNode *make_internal_node6(
        const string &name,
        struct YYLTYPE position,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3,
        struct ASTNode *node4,
        struct ASTNode *node5,
        struct ASTNode *node6
) {
    struct ASTNode *node = new ASTNode;
    node->type = INTERNAL_NODE;
    node->name = name;
    node->position = position;
    node->children.push_back(node1);
    node->children.push_back(node2);
    node->children.push_back(node3);
    node->children.push_back(node4);
    node->children.push_back(node5);
    node->children.push_back(node6);
    return node;
}

struct ASTNode *make_internal_node7(
        const string &name,
        struct YYLTYPE position,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3,
        struct ASTNode *node4,
        struct ASTNode *node5,
        struct ASTNode *node6,
        struct ASTNode *node7
) {
    struct ASTNode *node = new ASTNode;
    node->type = INTERNAL_NODE;
    node->name = name;
    node->position = position;
    node->children.push_back(node1);
    node->children.push_back(node2);
    node->children.push_back(node3);
    node->children.push_back(node4);
    node->children.push_back(node5);
    node->children.push_back(node6);
    node->children.push_back(node7);
    return node;
}

struct ASTNode *make_internal_node8(
        const string &name,
        struct YYLTYPE position,
        struct ASTNode *node1,
        struct ASTNode *node2,
        struct ASTNode *node3,
        struct ASTNode *node4,
        struct ASTNode *node5,
        struct ASTNode *node6,
        struct ASTNode *node7,
        struct ASTNode *node8
) {
    struct ASTNode *node = new ASTNode;
    node->type = INTERNAL_NODE;
    node->name = name;
    node->position = position;
    node->children.push_back(node1);
    node->children.push_back(node2);
    node->children.push_back(node3);
    node->children.push_back(node4);
    node->children.push_back(node5);
    node->children.push_back(node6);
    node->children.push_back(node7);
    node->children.push_back(node8);
    return node;
}

struct ASTNode *make_internal_node9(
        const string &name,
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
) {
    struct ASTNode *node = new ASTNode;
    node->type = INTERNAL_NODE;
    node->name = name;
    node->position = position;
    node->children.push_back(node1);
    node->children.push_back(node2);
    node->children.push_back(node3);
    node->children.push_back(node4);
    node->children.push_back(node5);
    node->children.push_back(node6);
    node->children.push_back(node7);
    node->children.push_back(node8);
    node->children.push_back(node9);
    return node;
}

struct ASTNode *make_internal_node10(
        const string &name,
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
) {
    struct ASTNode *node = new ASTNode;
    node->type = INTERNAL_NODE;
    node->name = name;
    node->position = position;
    node->children.push_back(node1);
    node->children.push_back(node2);
    node->children.push_back(node3);
    node->children.push_back(node4);
    node->children.push_back(node5);
    node->children.push_back(node6);
    node->children.push_back(node7);
    node->children.push_back(node8);
    node->children.push_back(node9);
    node->children.push_back(node10);
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


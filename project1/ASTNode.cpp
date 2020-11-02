#include <malloc.h>
#include <any>
#include "ASTNode.hpp"

using std::string;
using std::vector;
using std::initializer_list;
using std::any;
using std::any_cast;

int error_happen = 0;

struct ASTNode *make_empty_node(const string &name, struct YYLTYPE position) {
    auto *node = new ASTNode;
    node->type = EMPTY_LEAF;
    node->name = name;
    node->position = position;
    node->parent = nullptr;
    return node;
}

struct ASTNode *make_int_node(const string &name, struct YYLTYPE position, int value) {
    auto *node = new ASTNode;
    node->type = INT_LEAF;
    node->name = name;
    node->position = position;
    node->attributes["int_value"] = value;
    node->parent = nullptr;
    return node;
}

struct ASTNode *make_float_node(const string &name, struct YYLTYPE position, float value) {
    auto *node = new ASTNode;
    node->type = FLOAT_LEAF;
    node->name = name;
    node->position = position;
    node->attributes["float_value"] = value;
    node->parent = nullptr;
    return node;
}

struct ASTNode *make_string_node(const string &name, struct YYLTYPE position, const string &value) {
    auto *node = new ASTNode;
    node->type = STRING_LEAF;
    node->name = name;
    node->position = position;
    node->attributes["string_value"] = value;
    node->parent = nullptr;
    return node;
}

struct ASTNode *make_internal_node(
        const string &name,
        struct YYLTYPE position,
        initializer_list<ASTNode *> children
) {
    auto *node = new ASTNode;
    node->type = INTERNAL_NODE;
    node->name = name;
    node->position = position;

    for (auto child : children) {
        node->children.push_back(child);
        if (child->parent != nullptr) {
            printf("child already have parent\n");
            error_happen = true;
        } else {
            child->parent = node;
        }
    }

    return node;
}

void ASTVisitor::visit_node(ASTNode *node) {
    if (node->name == "Program") {
        visit_Program(node);
    } else if (node->name == "ImportList") {
        visit_ImportList(node);
    } else if (node->name == "ImportStmt") {
        visit_ImportStmt(node);
    } else if (node->name == "ExtDefList") {
        visit_ExtDefList(node);
    } else if (node->name == "ExtDef") {
        visit_ExtDef(node);
    } else if (node->name == "ExtDecList") {
        visit_ExtDecList(node);
    } else if (node->name == "Specifier") {
        visit_Specifier(node);
    } else if (node->name == "StructSpecifier") {
        visit_StructSpecifier(node);
    } else if (node->name == "VarDec") {
        visit_VarDec(node);
    } else if (node->name == "FunDec") {
        visit_FunDec(node);
    } else if (node->name == "VarList") {
        visit_VarList(node);
    } else if (node->name == "ParamDec") {
        visit_ParamDec(node);
    } else if (node->name == "CompSt") {
        visit_CompSt(node);
    } else if (node->name == "StmtList") {
        visit_StmtList(node);
    } else if (node->name == "Stmt") {
        visit_Stmt(node);
    } else if (node->name == "DefList") {
        visit_DefList(node);
    } else if (node->name == "Def") {
        visit_Def(node);
    } else if (node->name == "DecList") {
        visit_DecList(node);
    } else if (node->name == "Dec") {
        visit_Dec(node);
    } else if (node->name == "Exp") {
        visit_Exp(node);
    } else if (node->name == "Args") {
        visit_Args(node);
    } else if (node->name == "String") {
        visit_String(node);
    } else if (node->name == "StringInternalList") {
        visit_StringInternalList(node);
    } else {
        visit_token(node);
    }
}

void ASTVisitor::visit_children(ASTNode *node) {
    for (auto child : node->children) {
        visit_node(child);
    }
}

void ASTVisitor::visit_token(ASTNode *node) {
}

void ASTVisitor::visit_Program(ASTNode *node) { visit_children(node); }

void ASTVisitor::visit_ImportList(ASTNode *node) { visit_children(node); }

void ASTVisitor::visit_ImportStmt(ASTNode *node) { visit_children(node); }

void ASTVisitor::visit_ExtDefList(ASTNode *node) { visit_children(node); }

void ASTVisitor::visit_ExtDef(ASTNode *node) { visit_children(node); }

void ASTVisitor::visit_ExtDecList(ASTNode *node) { visit_children(node); }

void ASTVisitor::visit_Specifier(ASTNode *node) { visit_children(node); }

void ASTVisitor::visit_StructSpecifier(ASTNode *node) { visit_children(node); }

void ASTVisitor::visit_VarDec(ASTNode *node) { visit_children(node); }

void ASTVisitor::visit_FunDec(ASTNode *node) { visit_children(node); }

void ASTVisitor::visit_VarList(ASTNode *node) { visit_children(node); }

void ASTVisitor::visit_ParamDec(ASTNode *node) { visit_children(node); }

void ASTVisitor::visit_CompSt(ASTNode *node) { visit_children(node); }

void ASTVisitor::visit_StmtList(ASTNode *node) { visit_children(node); }

void ASTVisitor::visit_Stmt(ASTNode *node) { visit_children(node); }

void ASTVisitor::visit_DefList(ASTNode *node) { visit_children(node); }

void ASTVisitor::visit_Def(ASTNode *node) { visit_children(node); }

void ASTVisitor::visit_DecList(ASTNode *node) { visit_children(node); }

void ASTVisitor::visit_Dec(ASTNode *node) { visit_children(node); }

void ASTVisitor::visit_Exp(ASTNode *node) { visit_children(node); }

void ASTVisitor::visit_Args(ASTNode *node) { visit_children(node); }

void ASTVisitor::visit_String(ASTNode *node) { visit_children(node); }

void ASTVisitor::visit_StringInternalList(ASTNode *node) { visit_children(node); }

void ASTPrinter::visit_node(ASTNode *root) {
    int should_not_print_self = root->type == INTERNAL_NODE && root->children.empty();

    if (!should_not_print_self) {

        for (int i = 0; i < this->level; i++) {
            printf("  ");
        }
        printf("%s", root->name.data());

        if (root->type != EMPTY_LEAF && root->type != INTERNAL_NODE) {
            printf(": ");
        }

        if (!root->display_value.empty()) {
            printf("%s", root->display_value.data());
        } else if (root->type == INT_LEAF) {
            printf("%d", any_cast<int>(root->attributes["int_value"]));
        } else if (root->type == FLOAT_LEAF) {
            printf("%f", any_cast<float>(root->attributes["float_value"]));
        } else if (root->type == STRING_LEAF) {
            printf("%s", any_cast<string>(root->attributes["string_value"]).data());
        }

        if (root->type == INTERNAL_NODE) {
            printf(" (%d)", root->position.first_line);
        }

        printf("\n");

    }
    this->level++;
    ASTVisitor::visit_children(root);
    this->level--;
}
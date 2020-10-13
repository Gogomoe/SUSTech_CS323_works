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
    enum ASTType type{};
    std::string name{};
    std::string display_value{};
    struct YYLTYPE position{};

    int int_value{};
    float float_value{};
    std::string string_value{};

    std::vector<ASTNode *> children{};
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

class ASTVisitor {
public:
    virtual void visit_node(ASTNode *);
    virtual void visit_children(ASTNode *);
    virtual void visit_token(ASTNode *);

    virtual void visit_Program(ASTNode *);
    virtual void visit_ImportList(ASTNode *);
    virtual void visit_ImportStmt(ASTNode *);
    virtual void visit_ExtDefList(ASTNode *);
    virtual void visit_ExtDef(ASTNode *);
    virtual void visit_ExtDecList(ASTNode *);
    virtual void visit_Specifier(ASTNode *);
    virtual void visit_StructSpecifier(ASTNode *);
    virtual void visit_VarDec(ASTNode *);
    virtual void visit_FunDec(ASTNode *);
    virtual void visit_VarList(ASTNode *);
    virtual void visit_ParamDec(ASTNode *);
    virtual void visit_CompSt(ASTNode *);
    virtual void visit_StmtList(ASTNode *);
    virtual void visit_Stmt(ASTNode *);
    virtual void visit_DefList(ASTNode *);
    virtual void visit_Def(ASTNode *);
    virtual void visit_DecList(ASTNode *);
    virtual void visit_Dec(ASTNode *);
    virtual void visit_Exp(ASTNode *);
    virtual void visit_Args(ASTNode *);
    virtual void visit_String(ASTNode *);
    virtual void visit_StringInternalList(ASTNode *);
};

class ASTPrinter: ASTVisitor {
private:
    int level = 0;
public:
    void visit_node(ASTNode *) override;
};

#endif /* of AST_H */
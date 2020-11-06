#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include "ASTNode.hpp"
#include <memory>

class Type {
public:
    std::string name;

    explicit Type(std::string);

    virtual std::string getName();
};

class IntType : public Type {
public:
    IntType();
};

class FloatType : public Type {
public:
    FloatType();
};

class CharType : public Type {
public:
    CharType();
};

class StringType : public Type {
public:
    StringType();
};


class ArrayType : public Type {
public:
    std::shared_ptr<Type> type;
    int size;

    ArrayType(const std::shared_ptr<Type>&, int);
};

class StructType : public Type {
public:
    std::string identify;
    std::vector<std::pair<std::string, std::shared_ptr<Type>>> fields;

    explicit StructType(const std::string &, std::vector<std::pair<std::string, std::shared_ptr<Type>>>);
};

class FunctionType : public Type {
public:
    std::shared_ptr<Type> returns;
    std::vector<std::pair<std::string, std::shared_ptr<Type>>> parameters;

    explicit FunctionType(const std::shared_ptr<Type>&, std::vector<std::pair<std::string, std::shared_ptr<Type>>>);
};

class ASTAnalyzer : public ASTVisitor {
    ASTNode *root;
    std::map<std::string, std::shared_ptr<StructType>> typeTable{};
    std::map<std::string, std::shared_ptr<FunctionType>> functionTable{};

    std::vector<std::optional<std::string>> loopTable{};

    std::vector<std::map<std::string, std::shared_ptr<Type>>> symbolTableStack{};

    std::map<std::string, std::shared_ptr<Type>> &getSymbolTable();

    void pushSymbolTable();

    void popSymbolTable();

    std::optional<std::shared_ptr<Type>> findSymbol(const std::string &);

public:
    explicit ASTAnalyzer(ASTNode *);

    void analyse();

//    void visit_node(ASTNode *);
//    void visit_children(ASTNode *);
//    void visit_token(ASTNode *);
//
//    void visit_Program(ASTNode *);
//    void visit_ImportList(ASTNode *);
//    void visit_ImportStmt(ASTNode *);
    void visit_ExtDefList(ASTNode *);

    void visit_ExtDef(ASTNode *);

    void visit_ExtDecList(ASTNode *);

    void visit_Specifier(ASTNode *);

    void visit_StructSpecifier(ASTNode *);

    void visit_VarDec(ASTNode *);

    void visit_FunDec(ASTNode *);

    void visit_VarList(ASTNode *);

    void visit_ParamDec(ASTNode *);

    void visit_CompSt(ASTNode *);

    void visit_StmtList(ASTNode *);

    void visit_Stmt(ASTNode *);

    void visit_DefList(ASTNode *);

    void visit_Def(ASTNode *);

    void visit_DecList(ASTNode *);

    void visit_Dec(ASTNode *);

    void visit_Exp(ASTNode *);

    void visit_Args(ASTNode *);

//    void visit_String(ASTNode *);
//    void visit_StringInternalList(ASTNode *);
};

#endif //SEMANTIC_ANALYZER_HPP

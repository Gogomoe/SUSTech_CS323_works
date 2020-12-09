#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include "ASTNode.hpp"
#include <memory>

class Type {
public:
    std::string name;
    int width;

    Type(std::string, int);
    explicit Type(std::string);

    virtual bool type_equals(std::shared_ptr<Type> type);
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

    ArrayType(const std::shared_ptr<Type> &, int);

    bool type_equals(std::shared_ptr<Type>) override;
};

class StructType : public Type {
public:
    std::string identify;
    std::vector<std::pair<std::string, std::shared_ptr<Type>>> fields;

    explicit StructType(const std::string &, std::vector<std::pair<std::string, std::shared_ptr<Type>>>);

    bool type_equals(std::shared_ptr<Type>) override;
};

class FunctionType : public Type {
public:
    std::shared_ptr<Type> returns;
    std::vector<std::pair<std::string, std::shared_ptr<Type>>> parameters;

    explicit FunctionType(const std::shared_ptr<Type> &, std::vector<std::pair<std::string, std::shared_ptr<Type>>>);
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

    static void set_file(const std::string &);

    void analyse();

//    void visit_node(ASTNode *);
//    void visit_children(ASTNode *);
//    void visit_token(ASTNode *);
//
//    void visit_Program(ASTNode *);
//    void visit_ImportList(ASTNode *);
//    void visit_ImportStmt(ASTNode *);
    void visit_ExtDefList(ASTNode *) override;

    void visit_ExtDef(ASTNode *) override;

    void visit_ExtDecList(ASTNode *) override;

    void visit_Specifier(ASTNode *) override;

    void visit_StructSpecifier(ASTNode *) override;

    void visit_VarDec(ASTNode *) override;

    void visit_FunDec(ASTNode *) override;

    void visit_VarList(ASTNode *) override;

    void visit_ParamDec(ASTNode *) override;

    void visit_CompSt(ASTNode *) override;

    void visit_StmtList(ASTNode *) override;

    void visit_Stmt(ASTNode *) override;

    void visit_DefList(ASTNode *) override;

    void visit_Def(ASTNode *) override;

    void visit_DecList(ASTNode *) override;

    void visit_Dec(ASTNode *) override;

    void visit_Exp(ASTNode *) override;

    void visit_Args(ASTNode *) override;

//    void visit_String(ASTNode *);
//    void visit_StringInternalList(ASTNode *);
};

#endif //SEMANTIC_ANALYZER_HPP

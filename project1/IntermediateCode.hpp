#ifndef INTERMEDIATE_CODE_HPP
#define INTERMEDIATE_CODE_HPP

#include "ASTNode.hpp"
#include "SemanticAnalyzer.hpp"

class Code {
public:
    std::string code;
    std::vector<std::shared_ptr<Code>> codes;

    explicit Code(std::string);

    explicit Code(std::vector<std::shared_ptr<Code>>);

};

class LoopItem {
public:
    std::string name;
    std::string loopLabel;
    std::string endLabel;
};

class IntermediateCode : public ASTVisitor {
    ASTNode *root;
//    std::map<std::string, std::shared_ptr<StructType>> typeTable{};
//    std::map<std::string, std::shared_ptr<FunctionType>> functionTable{};
//
    std::vector<LoopItem> loopTable{};

    int variable_count = 0;
    int temp_count = 0;
    int label_count = 0;

    std::map<std::string, std::string> symbolTable{};
    std::map<std::string, int> tempSymbols{};

    std::string createSymbol(const std::string &);

    std::string createTempSymbol();

    std::string createTempSymbol(const std::string &);

    std::string createLabel();

public:
    explicit IntermediateCode(ASTNode *);

    static void set_file(const std::string &);

    void generate();

//    void visit_node(ASTNode *);
//    void visit_children(ASTNode *);
//    void visit_token(ASTNode *);
//
    void visit_Program(ASTNode *) override;

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

#endif //INTERMEDIATE_CODE_HPP

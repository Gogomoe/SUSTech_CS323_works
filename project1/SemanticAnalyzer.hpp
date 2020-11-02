#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include "ASTNode.hpp"

class ASTAnalyzer : public ASTVisitor {
    ASTNode *root;
public:
    ASTAnalyzer(ASTNode *);

    void analyse();
};

#endif //SEMANTIC_ANALYZER_HPP

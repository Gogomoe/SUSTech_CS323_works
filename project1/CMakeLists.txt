cmake_minimum_required(VERSION 3.16)
project(Project)

set(CMAKE_CXX_STANDARD 17)

add_executable(splc SemanticAnalyzer.cpp syntax.tab.c ASTNode.cpp SemanticAnalyzer.cpp IntermediateCode.cpp)

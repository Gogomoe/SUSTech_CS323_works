#include "SemanticAnalyzer.hpp"
#include <numeric>
#include <utility>
#include <stdexcept>
#include <functional>

using std::string;
using std::vector;
using std::map;
using std::pair;
using std::optional;
using std::shared_ptr;
using std::make_shared;
using std::dynamic_pointer_cast;
using std::any;
using std::any_cast;
using std::runtime_error;

Type::Type(string name_, int width) : name(std::move(name_)), width(width) {}

Type::Type(string name_) : Type(name_, -1) {}


bool Type::type_equals(std::shared_ptr<Type> type) {
    return this->name == type->name;
}

IntType::IntType() : Type("int", 4) {}

FloatType::FloatType() : Type("float", 4) {}

CharType::CharType() : Type("char", 4) {}

StringType::StringType() : Type("string", 4) {}

ArrayType::ArrayType(
        const std::shared_ptr<Type> &type_,
        int size_
) : Type((*type_).name + "[" + std::to_string(size_) + "]", type_->width * size_),
    type(type_),
    size(size_) {}

bool ArrayType::type_equals(std::shared_ptr<Type> other) {
    shared_ptr<ArrayType> other_type = dynamic_pointer_cast<ArrayType>(other);
    if (other_type == nullptr) {
        return false;
    }
    return this->type->type_equals(other_type->type) && this->size == other_type->size;
}


StructType::StructType(
        const string &identify_,
        vector<pair<string, shared_ptr<Type>>> fields_
) : Type(identify_),
    identify(identify_),
    fields(std::move(fields_)) {
    int width = 0;
    for (auto &it:fields) {
        width += it.second->width;
    }
    this->width = width;
}

bool StructType::type_equals(std::shared_ptr<Type> other) {
    shared_ptr<StructType> other_type = dynamic_pointer_cast<StructType>(other);
    if (other_type == nullptr) {
        return false;
    }
    if (this->fields.size() != other_type->fields.size()) {
        return false;
    }
    for (int i = 0; i < this->fields.size(); i++) {
        if (!this->fields[i].second->type_equals(other_type->fields[i].second)) {
            return false;
        }
    }
    return true;
}


template<class T>
string joinToString(const vector<T> &list, const string &separator, const std::function<string(T const &)> &transform) {
    if (list.empty()) {
        return "";
    }
    string result;
    auto it = list.begin();

    result += transform(*it);
    it++;

    while (it != list.end()) {
        result += separator;
        result += transform(*it);
        it++;
    }

    return result;
}


FunctionType::FunctionType(
        const shared_ptr<Type> &returns_,
        vector<pair<string, shared_ptr<Type>>> parameters_
) : Type("(" +
         joinToString<pair<string, shared_ptr<Type>>>(
                 parameters_, ",",
                 [](pair<string, shared_ptr<Type>> const &it) { return it.second->name; }
         ) +
         ")->" + returns_->name),
    returns(returns_),
    parameters(std::move(parameters_)) {}

const shared_ptr<IntType> INT_TYPE = make_shared<IntType>(IntType());
const shared_ptr<FloatType> FLOAT_TYPE = make_shared<FloatType>(FloatType());
const shared_ptr<CharType> CHAR_TYPE = make_shared<CharType>(CharType());
const shared_ptr<StringType> STRING_TYPE = make_shared<StringType>(StringType());

map<string, shared_ptr<Type>> &ASTAnalyzer::getSymbolTable() {
    return symbolTableStack.back();
}

void ASTAnalyzer::pushSymbolTable() {
    symbolTableStack.emplace_back();
}

void ASTAnalyzer::popSymbolTable() {
    symbolTableStack.pop_back();
}

optional<shared_ptr<Type>> ASTAnalyzer::findSymbol(const string &key) {
    for (auto it = symbolTableStack.rbegin(); it != symbolTableStack.rend(); it++) {
        if ((*it).count(key) == 1) {
            return (*it)[key];
        }
    }
    return {};
}

FILE *output;

ASTAnalyzer::ASTAnalyzer(ASTNode *node) : root(node) {
    output = fopen("/dev/stdout", "w+");
}

void ASTAnalyzer::set_file(const std::string &filename) {
    if (filename.substr(filename.length() - 4, 4) == ".spl") {
        string output_file = filename.substr(0, filename.length() - 4) + ".out";
        output = fopen(output_file.data(), "w+");
    } else {
        printf("output to stdout");
    }
}


void ASTAnalyzer::analyse() {
    try {

        functionTable["read"] = make_shared<FunctionType>(
                FunctionType(
                        INT_TYPE,
                        vector<pair<string, shared_ptr<Type>>>()
                )
        );
        functionTable["write"] = make_shared<FunctionType>(
                FunctionType(
                        INT_TYPE,
                        vector<pair<string, shared_ptr<Type>>>(
                                {pair<string, shared_ptr<Type>>("write_char", INT_TYPE)}
                        )
                )
        );

        pushSymbolTable();
        visit_node(this->root);
        popSymbolTable();
    } catch (runtime_error &error) {
        printf("%s\n", error.what());
        error_happen = true;
    } catch (std::out_of_range &error) {
        printf("%s\n", error.what());
        error_happen = true;
    } catch (std::bad_any_cast &error) {
        printf("%s\n", error.what());
        error_happen = true;
    }
}

// definitions: vector<pair<string, shared_ptr<Type>>>
void ASTAnalyzer::visit_ExtDefList(ASTNode *node) {
    if (node->children.empty()) {
        // ExtDefList: %empty
        node->attributes["definitions"] = vector<pair<string, shared_ptr<Type>>>();

    } else if (node->children.size() == 2) {
        // ExtDefList: ExtDef ExtDefList
        ASTNode *ExtDef = node->children[0];
        ASTNode *ExtDefList = node->children[1];

        visit_ExtDef(ExtDef);
        visit_ExtDefList(ExtDefList);

        auto definitions = vector<pair<string, shared_ptr<Type>>>();
        auto defA = any_cast<vector<pair<string, shared_ptr<Type>>>>(ExtDef->attributes.at("definitions"));
        auto defB = any_cast<vector<pair<string, shared_ptr<Type>>>>(ExtDefList->attributes.at("definitions"));
        definitions.insert(definitions.end(), defA.begin(), defA.end());
        definitions.insert(definitions.end(), defB.begin(), defB.end());

        node->attributes["definitions"] = definitions;

    } else {
        throw runtime_error("error ExtDefList children");
    }
}

// definitions: vector<pair<string, shared_ptr<Type>>>
void ASTAnalyzer::visit_ExtDef(ASTNode *node) {
    if (node->children.size() == 2 && node->children[1]->name == "SEMI") {
        // ExtDef: Specifier SEMI
        ASTNode *Specifier = node->children[0];

        visit_Specifier(Specifier);

        node->attributes["definitions"] = vector<pair<string, shared_ptr<Type>>>();

    } else if (node->children.size() == 3 && node->children[2]->name == "SEMI") {
        // ExtDef: Specifier ExtDecList SEMI
        ASTNode *Specifier = node->children[0];
        ASTNode *ExtDecList = node->children[1];

        visit_Specifier(Specifier);

        node->attributes["type"] = Specifier->attributes.at("type");

        visit_ExtDecList(ExtDecList);

        node->attributes["definitions"] = ExtDecList->attributes.at("definitions");

    } else if (node->children.size() == 3 && node->children[1]->name == "FunDec") {
        // ExtDef: Specifier FunDec CompSt
        ASTNode *Specifier = node->children[0];
        ASTNode *FunDec = node->children[1];
        ASTNode *CompSt = node->children[2];

        visit_Specifier(Specifier);

        node->attributes["return_type"] = Specifier->attributes.at("type");

        visit_FunDec(FunDec);

        auto declaration = any_cast<pair<string, shared_ptr<Type>>>(FunDec->attributes.at("declaration"));

        pushSymbolTable();

        auto &table = getSymbolTable();

        auto functionType = dynamic_pointer_cast<FunctionType>(declaration.second);
        if (functionType == nullptr) {
            throw runtime_error("cannot cast to FunctionType");
        }
        for (auto &p: functionType->parameters) {
            table[p.first] = p.second;
        }

        visit_CompSt(CompSt);

        popSymbolTable();


        node->attributes["definitions"] = vector<pair<string, shared_ptr<Type>>>();

    } else {
        throw runtime_error("error ExtDef children");
    }
}

// definitions: vector<pair<string, shared_ptr<Type>>>
void ASTAnalyzer::visit_ExtDecList(ASTNode *node) {
    node->attributes["type"] = node->parent->attributes.at("type");

    if (node->children.size() == 1) {
        // ExtDecList: VarDec
        ASTNode *VarDec = node->children[0];

        visit_VarDec(VarDec);

        auto declaration = any_cast<pair<string, shared_ptr<Type>>>(VarDec->attributes.at("declaration"));
        auto definitions = vector<pair<string, shared_ptr<Type>>>();

        definitions.push_back(declaration);

        node->attributes["definitions"] = definitions;

    } else if (node->children.size() == 3) {
        // ExtDecList: VarDec COMMA ExtDecList
        ASTNode *VarDec = node->children[0];
        ASTNode *ExtDecList = node->children[2];

        visit_VarDec(VarDec);

        auto declaration = any_cast<pair<string, shared_ptr<Type>>>(VarDec->attributes.at("declaration"));

        visit_ExtDecList(ExtDecList);

        auto definitions = vector<pair<string, shared_ptr<Type>>>();
        auto defA = any_cast<vector<pair<string, shared_ptr<Type>>>>(ExtDecList->attributes.at("definitions"));

        definitions.push_back(declaration);
        definitions.insert(definitions.end(), defA.begin(), defA.end());

        node->attributes["definitions"] = definitions;


    } else {
        throw runtime_error("error DecList children");
    }

}

// type: Type*
void ASTAnalyzer::visit_Specifier(ASTNode *node) {
    if (node->children[0]->name == "TYPE") {
        // Specifier: TYPE
        ASTNode *TYPE = node->children[0];
        auto typeName = any_cast<string>(TYPE->attributes.at("string_value"));

        if (typeName == "int") {
            shared_ptr<Type> type = INT_TYPE;
            node->attributes["type"] = type;
        } else if (typeName == "float") {
            shared_ptr<Type> type = FLOAT_TYPE;
            node->attributes["type"] = type;
        } else if (typeName == "char") {
            shared_ptr<Type> type = CHAR_TYPE;
            node->attributes["type"] = type;
        } else if (typeName == "string") {
            shared_ptr<Type> type = STRING_TYPE;
            node->attributes["type"] = type;
        } else {
            throw runtime_error("error TYPE");
        }

    } else if (node->children[0]->name == "StructSpecifier") {
        // Specifier: StructSpecifier
        ASTNode *StructSpecifier = node->children[0];

        visit_StructSpecifier(StructSpecifier);

        node->attributes["type"] = StructSpecifier->attributes.at("type");

    } else {
        throw runtime_error("error Specifier child");
    }

}

// type: Type*
void ASTAnalyzer::visit_StructSpecifier(ASTNode *node) {
    if (node->children.size() == 2) {
        // StructSpecifier: STRUCT ID
        ASTNode *ID = node->children[1];

        auto id = any_cast<string>(ID->attributes.at("string_value"));
        if (typeTable.count(id) == 0) {
            throw runtime_error("struct is used without definition");
        }

        shared_ptr<Type> structType = typeTable[id];
        node->attributes["type"] = structType;

    } else if (node->children.size() == 5) {
        // StructSpecifier: STRUCT ID LC DefList RC
        ASTNode *ID = node->children[1];
        ASTNode *DefList = node->children[3];

        auto id = any_cast<string>(ID->attributes.at("string_value"));

        pushSymbolTable();
        visit_DefList(DefList);
        popSymbolTable();

        auto definitions = any_cast<vector<pair<string, shared_ptr<Type>>>>(DefList->attributes.at("definitions"));

        auto actual = make_shared<StructType>(StructType(id, definitions));
        shared_ptr<Type> structType = actual;

        if (typeTable.count(id) != 0) {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    15,
                    ID->position.first_line,
                    ("redefine struct: " + id).data());
            error_happen = true;
        }

        typeTable[id] = dynamic_pointer_cast<StructType>(structType);
        node->attributes["type"] = structType;

    } else {
        throw runtime_error("error StructSpecifier children");
    }
}

// declaration: pair<string, shared_ptr<Type>>
void ASTAnalyzer::visit_VarDec(ASTNode *node) {
    auto type = any_cast<shared_ptr<Type>>(node->parent->attributes.at("type"));

    node->attributes["type"] = type;

    if (node->children.size() == 1) {
        // VarDec: ID
        ASTNode *ID = node->children[0];

        auto id = any_cast<string>(ID->attributes.at("string_value"));

        node->attributes["declaration"] = pair<string, shared_ptr<Type>>{id, type};

        auto &table = getSymbolTable();
        if (table.count(id) == 0) {
            table[id] = type;
        } else {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    3,
                    ID->position.first_line,
                    ("redefine variable: " + id).data());
            error_happen = true;
        }

    } else if (node->children.size() == 4) {
        // VarDec: VarDec LB INT RB
        ASTNode *VarDec = node->children[0];
        ASTNode *INT = node->children[2];

        visit_VarDec(VarDec);

        auto declaration = any_cast<pair<string, shared_ptr<Type>>>(VarDec->attributes.at("declaration"));

        auto size = any_cast<int>(INT->attributes.at("int_value"));

        string id = declaration.first;
        shared_ptr<Type> inner_type = declaration.second;

        auto actual = make_shared<ArrayType>(ArrayType(inner_type, size));
        shared_ptr<Type> arrType = actual;

        node->attributes["declaration"] = pair<string, shared_ptr<Type>>{id, arrType};

        auto &table = getSymbolTable();
        table[id] = arrType;

    } else {
        throw runtime_error("error VarDec children");
    }
}

// declaration: pair<string, shared_ptr<Type>>
void ASTAnalyzer::visit_FunDec(ASTNode *node) {
    auto returnType = any_cast<shared_ptr<Type>>(node->parent->attributes.at("return_type"));

    if (node->children.size() == 3) {
        // FunDec: ID LP RP
        ASTNode *ID = node->children[0];

        auto id = any_cast<string>(ID->attributes.at("string_value"));

        auto actual = make_shared<FunctionType>(FunctionType(returnType, vector<pair<string, shared_ptr<Type>>>()));
        shared_ptr<Type> type = actual;

        node->attributes["declaration"] = pair(id, type);

        if (functionTable.count(id) == 0) {
            functionTable[id] = dynamic_pointer_cast<FunctionType>(type);
        } else {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    4,
                    node->position.first_line,
                    ("redefine function: " + id).data());
            error_happen = true;
        }

    } else if (node->children.size() == 4) {
        // FunDec: ID LP VarList RP
        ASTNode *ID = node->children[0];
        ASTNode *VarList = node->children[2];

        auto id = any_cast<string>(ID->attributes.at("string_value"));

        pushSymbolTable();
        visit_VarList(VarList);
        popSymbolTable();

        auto parameters = any_cast<vector<pair<string, shared_ptr<Type>>>>(VarList->attributes.at("parameters"));

        auto actual = make_shared<FunctionType>(FunctionType(returnType, parameters));
        shared_ptr<Type> type = actual;

        node->attributes["declaration"] = pair(id, type);

        if (functionTable.count(id) == 0) {
            functionTable[id] = dynamic_pointer_cast<FunctionType>(type);
        } else {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    4,
                    node->position.first_line,
                    ("redefine function: " + id).data());
            error_happen = true;
        }

    } else {
        throw runtime_error("error FunDec children");
    }
}

// parameters: vector<pair<string, shared_ptr<Type>>>
void ASTAnalyzer::visit_VarList(ASTNode *node) {
    if (node->children.size() == 1) {
        // VarList: ParamDec
        ASTNode *ParamDec = node->children[0];

        visit_ParamDec(ParamDec);

        auto declaration = any_cast<pair<string, shared_ptr<Type>>>(ParamDec->attributes.at("declaration"));

        auto parameters = vector<pair<string, shared_ptr<Type>>>();
        parameters.push_back(declaration);

        node->attributes["parameters"] = parameters;

    } else if (node->children.size() == 3) {
        // VarList: ParamDec COMMA VarList
        ASTNode *ParamDec = node->children[0];
        ASTNode *VarList = node->children[2];

        visit_ParamDec(ParamDec);

        auto declaration = any_cast<pair<string, shared_ptr<Type>>>(ParamDec->attributes.at("declaration"));

        visit_VarList(VarList);

        auto paraA = any_cast<vector<pair<string, shared_ptr<Type>>>>(VarList->attributes.at("parameters"));

        auto parameters = vector<pair<string, shared_ptr<Type>>>();
        parameters.push_back(declaration);
        parameters.insert(parameters.end(), paraA.begin(), paraA.end());

        node->attributes["parameters"] = parameters;

    } else {
        throw runtime_error("error VarList children");
    }
}

// declaration: pair<string, shared_ptr<Type>>
// type: Type*
void ASTAnalyzer::visit_ParamDec(ASTNode *node) {
    // ParamDec: Specifier VarDec
    ASTNode *Specifier = node->children[0];
    ASTNode *VarDec = node->children[1];

    visit_Specifier(Specifier);

    node->attributes["type"] = Specifier->attributes.at("type");

    visit_VarDec(VarDec);

    node->attributes["declaration"] = VarDec->attributes.at("declaration");
}

// return_type: Type*
void ASTAnalyzer::visit_CompSt(ASTNode *node) {
    // CompSt: LC DefList StmtList RC
    node->attributes["return_type"] = node->parent->attributes.at("return_type");

    ASTNode *DefList = node->children[1];
    ASTNode *StmtList = node->children[2];

    visit_DefList(DefList);
    visit_StmtList(StmtList);
}

// return_type: Type*
void ASTAnalyzer::visit_StmtList(ASTNode *node) {
    node->attributes["return_type"] = node->parent->attributes.at("return_type");

    if (node->children.empty()) {
        // StmtList: %empty

    } else if (node->children.size() == 2) {
        // StmtList: Stmt StmtList
        ASTNode *Stmt = node->children[0];
        ASTNode *StmtList = node->children[1];

        visit_Stmt(Stmt);

        visit_StmtList(StmtList);

    } else {
        throw runtime_error("error StmtList children");
    }

}


// return_type: Type*
void ASTAnalyzer::visit_Stmt(ASTNode *node) {
    node->attributes["return_type"] = node->parent->attributes.at("return_type");

    if (node->children.size() == 2 && node->children[0]->name == "Exp") {
        // Stmt: Exp SEMI
        ASTNode *Exp = node->children[0];

        visit_Exp(Exp);

    } else if (node->children.size() == 1 && node->children[0]->name == "CompSt") {
        // Stmt: CompSt
        ASTNode *CompSt = node->children[0];

        pushSymbolTable();
        visit_CompSt(CompSt);
        popSymbolTable();

    } else if (node->children.size() == 3 && node->children[0]->name == "RETURN") {
        // Stmt: RETURN Exp SEMI
        ASTNode *Exp = node->children[1];

        visit_Exp(Exp);

        auto type = any_cast<optional<shared_ptr<Type>>>(Exp->attributes.at("type"));

        auto return_type = any_cast<shared_ptr<Type>>(node->attributes.at("return_type"));

        if (type.has_value() && type.value()->name != return_type->name) {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    8,
                    Exp->position.first_line,
                    "incompatiable return type");
            error_happen = true;
        }

    } else if (node->children.size() == 5 && node->children[0]->name == "IF") {
        // Stmt: IF LP Exp RP Stmt
        ASTNode *Exp = node->children[2];
        ASTNode *Stmt = node->children[4];

        visit_Exp(Exp);

        auto type = any_cast<optional<shared_ptr<Type>>>(Exp->attributes.at("type"));
        if (type.has_value() && type.value()->name != "int") {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    7,
                    Exp->position.first_line,
                    "unmatching operands, condition is not int");
            error_happen = true;
        }

        visit_Stmt(Stmt);

    } else if (node->children.size() == 7 && node->children[0]->name == "IF") {
        // Stmt: IF LP Exp RP Stmt ELSE Stmt
        ASTNode *Exp = node->children[2];
        ASTNode *Stmt1 = node->children[4];
        ASTNode *Stmt2 = node->children[6];

        visit_Exp(Exp);

        auto type = any_cast<optional<shared_ptr<Type>>>(Exp->attributes.at("type"));
        if (type.has_value() && type.value()->name != "int") {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    7,
                    Exp->position.first_line,
                    "unmatching operands, condition is not int");
            error_happen = true;
        }

        visit_Stmt(Stmt1);
        visit_Stmt(Stmt2);

    } else if (node->children.size() == 5 && node->children[0]->name == "WHILE") {
        // Stmt: WHILE LP Exp RP Stmt
        ASTNode *Exp = node->children[2];
        ASTNode *Stmt = node->children[4];

        visit_Exp(Exp);

        auto type = any_cast<optional<shared_ptr<Type>>>(Exp->attributes.at("type"));
        if (type.has_value() && type.value()->name != "int") {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    7,
                    Exp->position.first_line,
                    "unmatching operands, condition is not int");
            error_happen = true;
        }

        loopTable.emplace_back();
        visit_Stmt(Stmt);
        loopTable.pop_back();

    } else if (node->children.size() == 7 && node->children[2]->name == "WHILE") {
        // Stmt: ID COLON WHILE LP Exp RP Stmt
        ASTNode *ID = node->children[0];
        ASTNode *Exp = node->children[4];
        ASTNode *Stmt = node->children[6];

        auto loop_label = any_cast<string>(ID->attributes.at("string_value"));
        for (auto it = loopTable.rbegin(); it != loopTable.rend(); it++) {
            if ((*it).has_value() && (*it).value() == loop_label) {
                fprintf(output,
                        "Error type %d at Line %d: %s\n",
                        -1,
                        ID->position.first_line,
                        "redefined loop label");
                error_happen = true;
            }
        }

        visit_Exp(Exp);

        auto type = any_cast<optional<shared_ptr<Type>>>(Exp->attributes.at("type"));
        if (type.has_value() && type.value()->name != "int") {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    7,
                    Exp->position.first_line,
                    "unmatching operands, condition is not int");
            error_happen = true;
        }

        loopTable.emplace_back(loop_label);
        visit_Stmt(Stmt);
        loopTable.pop_back();

    } else if (node->children.size() == 7 && node->children[0]->name == "FOR") {
        // Stmt: FOR ID IN Exp DOUBLE_DOT Exp CompSt
        ASTNode *ID = node->children[1];
        ASTNode *Exp1 = node->children[3];
        ASTNode *Exp2 = node->children[5];
        ASTNode *CompSt = node->children[6];

        auto it_name = any_cast<string>(ID->attributes.at("string_value"));

        visit_Exp(Exp1);
        visit_Exp(Exp2);

        auto type1 = any_cast<optional<shared_ptr<Type>>>(Exp1->attributes.at("type"));
        if (type1.has_value() && type1.value()->name != "int") {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    7,
                    Exp1->position.first_line,
                    "unmatching operands, for loop range is not int");
            error_happen = true;
        }
        auto type2 = any_cast<optional<shared_ptr<Type>>>(Exp2->attributes.at("type"));
        if (type2.has_value() && type2.value()->name != "int") {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    7,
                    Exp2->position.first_line,
                    "unmatching operands, for loop range is not int");
            error_happen = true;
        }

        pushSymbolTable();
        auto &table = getSymbolTable();
        table[it_name] = INT_TYPE;

        loopTable.emplace_back();
        visit_CompSt(CompSt);
        loopTable.pop_back();

        popSymbolTable();

    } else if (node->children.size() == 9 && node->children[2]->name == "FOR") {
        // Stmt: ID COLON FOR ID IN Exp DOUBLE_DOT Exp CompSt
        ASTNode *ID0 = node->children[0];
        ASTNode *ID1 = node->children[3];
        ASTNode *Exp1 = node->children[5];
        ASTNode *Exp2 = node->children[7];
        ASTNode *CompSt = node->children[8];

        auto loop_label = any_cast<string>(ID0->attributes.at("string_value"));
        for (auto it = loopTable.rbegin(); it != loopTable.rend(); it++) {
            if ((*it).has_value() && (*it).value() == loop_label) {
                fprintf(output,
                        "Error type %d at Line %d: %s\n",
                        -1,
                        ID0->position.first_line,
                        "redefined loop label");
                error_happen = true;
            }
        }

        auto it_name = any_cast<string>(ID1->attributes.at("string_value"));

        visit_Exp(Exp1);
        visit_Exp(Exp2);

        auto type1 = any_cast<optional<shared_ptr<Type>>>(Exp1->attributes.at("type"));
        if (type1.has_value() && type1.value()->name != "int") {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    7,
                    Exp1->position.first_line,
                    "unmatching operands, for loop range is not int");
            error_happen = true;
        }
        auto type2 = any_cast<optional<shared_ptr<Type>>>(Exp2->attributes.at("type"));
        if (type2.has_value() && type2.value()->name != "int") {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    7,
                    Exp2->position.first_line,
                    "unmatching operands, for loop range is not int");
            error_happen = true;
        }

        pushSymbolTable();
        auto &table = getSymbolTable();
        table[it_name] = INT_TYPE;

        loopTable.emplace_back(loop_label);
        visit_CompSt(CompSt);
        loopTable.pop_back();

        popSymbolTable();

    } else if (node->children.size() == 2 && node->children[0]->name == "BREAK") {
        // Stmt: BREAK SEMI


        if (loopTable.empty()) {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    -1,
                    node->position.first_line,
                    "cannot break outside loop");
            error_happen = true;
        }

    } else if (node->children.size() == 3 && node->children[0]->name == "BREAK") {
        // Stmt: BREAK ID SEMI
        ASTNode *ID = node->children[1];

        auto loop_label = any_cast<string>(ID->attributes.at("string_value"));

        bool find_label = false;
        for (auto it = loopTable.rbegin(); it != loopTable.rend(); it++) {
            if ((*it).has_value() && (*it).value() == loop_label) {
                find_label = true;
            }
        }
        if (!find_label) {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    -1,
                    ID->position.first_line,
                    "cannot break without label");
            error_happen = true;
        }

    } else if (node->children.size() == 2 && node->children[0]->name == "CONTINUE") {
        // Stmt: CONTINUE SEMI


        if (loopTable.empty()) {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    -1,
                    node->position.first_line,
                    "cannot continue outside loop");
            error_happen = true;
        }

    } else if (node->children.size() == 3 && node->children[0]->name == "CONTINUE") {
        // Stmt: CONTINUE ID SEMI
        ASTNode *ID = node->children[1];

        auto loop_label = any_cast<string>(ID->attributes.at("string_value"));

        bool find_label = false;
        for (auto it = loopTable.rbegin(); it != loopTable.rend(); it++) {
            if ((*it).has_value() && (*it).value() == loop_label) {
                find_label = true;
            }
        }
        if (!find_label) {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    -1,
                    ID->position.first_line,
                    "cannot continue without label");
            error_happen = true;
        }
    } else {
        throw runtime_error("error Stmt children");
    }

}


// definitions: vector<pair<string, shared_ptr<Type>>>
void ASTAnalyzer::visit_DefList(ASTNode *node) {
    if (node->children.empty()) {
        // DefList: %empty
        node->attributes["definitions"] = vector<pair<string, shared_ptr<Type>>>();

    } else if (node->children.size() == 2) {
        // DefList: Def DefList
        ASTNode *Def = node->children[0];
        ASTNode *DefList = node->children[1];

        visit_Def(Def);
        visit_DefList(DefList);

        auto definitions = vector<pair<string, shared_ptr<Type>>>();
        auto defA = any_cast<vector<pair<string, shared_ptr<Type>>>>(Def->attributes.at("definitions"));
        auto defB = any_cast<vector<pair<string, shared_ptr<Type>>>>(DefList->attributes.at("definitions"));
        definitions.insert(definitions.end(), defA.begin(), defA.end());
        definitions.insert(definitions.end(), defB.begin(), defB.end());

        node->attributes["definitions"] = definitions;

    } else {
        throw runtime_error("error DefList children");
    }
}

// definitions: vector<pair<string, shared_ptr<Type>>>
// type: Type*
void ASTAnalyzer::visit_Def(ASTNode *node) {
    // Def: Specifier DecList SEMI
    ASTNode *Specifier = node->children[0];
    ASTNode *DecList = node->children[1];

    visit_Specifier(Specifier);

    node->attributes["type"] = Specifier->attributes.at("type");

    visit_DecList(DecList);

    node->attributes["definitions"] = DecList->attributes.at("definitions");

}

// definitions: vector<pair<string, shared_ptr<Type>>>
// type: Type*
void ASTAnalyzer::visit_DecList(ASTNode *node) {
    node->attributes["type"] = node->parent->attributes.at("type");

    if (node->children.size() == 1) {
        // DecList: Dec
        ASTNode *Dec = node->children[0];

        visit_Dec(Dec);

        auto declaration = any_cast<pair<string, shared_ptr<Type>>>(Dec->attributes.at("declaration"));
        auto definitions = vector<pair<string, shared_ptr<Type>>>();

        definitions.push_back(declaration);

        node->attributes["definitions"] = definitions;

    } else if (node->children.size() == 3) {
        // DecList: Dec COMMA DecList
        ASTNode *Dec = node->children[0];
        ASTNode *DecList = node->children[2];

        visit_Dec(Dec);
        visit_DecList(DecList);

        auto definitions = vector<pair<string, shared_ptr<Type>>>();
        auto declaration = any_cast<pair<string, shared_ptr<Type>>>(Dec->attributes.at("declaration"));
        auto defA = any_cast<vector<pair<string, shared_ptr<Type>>>>(DecList->attributes.at("definitions"));

        definitions.push_back(declaration);
        definitions.insert(definitions.end(), defA.begin(), defA.end());

        node->attributes["definitions"] = definitions;

    } else {
        throw runtime_error("error DecList children");
    }
}

// declaration: pair<string, shared_ptr<Type>>
// type: Type*
void ASTAnalyzer::visit_Dec(ASTNode *node) {
    node->attributes["type"] = node->parent->attributes.at("type");

    if (node->children.size() == 1) {
        // Dec: VarDec
        ASTNode *VarDec = node->children[0];

        visit_VarDec(VarDec);

        node->attributes["declaration"] = VarDec->attributes.at("declaration");

    } else if (node->children.size() == 3) {
        // Dec: VarDec ASSIGN Exp
        ASTNode *VarDec = node->children[0];
        ASTNode *Exp = node->children[2];

        visit_VarDec(VarDec);

        node->attributes["declaration"] = VarDec->attributes.at("declaration");

        visit_Exp(Exp);

        auto type = any_cast<optional<shared_ptr<Type>>>(Exp->attributes.at("type"));
        auto require_type = any_cast<shared_ptr<Type>>(node->attributes.at("type"));

        if (type.has_value() && !type.value()->type_equals(require_type)) {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    5,
                    Exp->position.first_line,
                    "unmatching type on return");
            error_happen = true;
        }

    } else {
        throw runtime_error("error Dec children");
    }
}

// type: optional<shared_ptr<Type>>
// lvalue: bool
void ASTAnalyzer::visit_Exp(ASTNode *node) {
    node->attributes["lvalue"] = false;

    if (node->children.size() == 1 && node->children[0]->name == "ID") {
        // Exp: ID
        ASTNode *ID = node->children[0];

        auto id = any_cast<string>(ID->attributes.at("string_value"));

        optional<shared_ptr<Type>> type = findSymbol(id);
        node->attributes["type"] = type;
        node->attributes["lvalue"] = true;

        if (!type.has_value()) {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    1,
                    ID->position.first_line,
                    ("undefined variable: " + id).data());
            error_happen = true;
        }

    } else if (node->children.size() == 1 && node->children[0]->name == "INT") {
        // Exp: INT

        optional<shared_ptr<Type>> type = INT_TYPE;
        node->attributes["type"] = type;

    } else if (node->children.size() == 1 && node->children[0]->name == "FLOAT") {
        // Exp: FLOAT

        optional<shared_ptr<Type>> type = FLOAT_TYPE;
        node->attributes["type"] = type;

    } else if (node->children.size() == 1 && node->children[0]->name == "CHAR") {
        // Exp: CHAR

        optional<shared_ptr<Type>> type = CHAR_TYPE;
        node->attributes["type"] = type;

    } else if (node->children.size() == 1 && node->children[0]->name == "String") {
        // Exp: String

        optional<shared_ptr<Type>> type = STRING_TYPE;
        node->attributes["type"] = type;
    } else if (node->children.size() == 3 && node->children[0]->name == "Exp" && node->children[2]->name == "Exp") {
        // Exp: Exp ASSIGN Exp
        // Exp: Exp AND Exp
        // Exp: Exp OR Exp
        // Exp: Exp LT Exp
        // Exp: Exp LE Exp
        // Exp: Exp GT Exp
        // Exp: Exp GE Exp
        // Exp: Exp NE Exp
        // Exp: Exp EQ Exp
        // Exp: Exp PLUS Exp
        // Exp: Exp MINUS Exp
        // Exp: Exp MUL Exp
        // Exp: Exp DIV Exp
        ASTNode *Exp1 = node->children[0];
        ASTNode *OP = node->children[1];
        ASTNode *Exp2 = node->children[2];

        visit_Exp(Exp1);
        visit_Exp(Exp2);

        auto type1 = any_cast<optional<shared_ptr<Type>>>(Exp1->attributes.at("type"));
        auto type2 = any_cast<optional<shared_ptr<Type>>>(Exp2->attributes.at("type"));

        if (!type1.has_value() || !type2.has_value()) {
            optional<shared_ptr<Type>> type = optional<shared_ptr<Type>>();
            node->attributes["type"] = type;
            return;
        }

        if (OP->name == "ASSIGN") {
            auto lvalue = any_cast<bool>(Exp1->attributes.at("lvalue"));
            if (!lvalue) {
                fprintf(output,
                        "Error type %d at Line %d: %s\n",
                        6,
                        OP->position.first_line,
                        "left side in assignment is rvalue");
                error_happen = true;

                optional<shared_ptr<Type>> type = optional<shared_ptr<Type>>();
                node->attributes["type"] = type;
                return;
            }

            if (!type1.value()->type_equals(type2.value())) {
                fprintf(output,
                        "Error type %d at Line %d: %s\n",
                        5,
                        OP->position.first_line,
                        "unmatching type on both sides of assignment");
                error_happen = true;

                optional<shared_ptr<Type>> type = optional<shared_ptr<Type>>();
                node->attributes["type"] = type;
                return;
            }

            node->attributes["type"] = type1;

        } else if (OP->name == "AND" || OP->name == "OR") {
            if (type1.value()->name != "int") {
                fprintf(output,
                        "Error type %d at Line %d: %s\n",
                        7,
                        Exp1->position.first_line,
                        "unmatching operands, boolean expr is not int");
                error_happen = true;
            }
            if (type2.value()->name != "int") {
                fprintf(output,
                        "Error type %d at Line %d: %s\n",
                        7,
                        Exp2->position.first_line,
                        "unmatching operands, boolean expr is not int");
                error_happen = true;
            }
            optional<shared_ptr<Type>> type = INT_TYPE;
            node->attributes["type"] = type;

        } else if (OP->name == "LT" || OP->name == "LE" || OP->name == "GT" || OP->name == "GE") {
            if (type1.value()->name != "int" && type1.value()->name != "float") {
                fprintf(output,
                        "Error type %d at Line %d: %s\n",
                        7,
                        Exp1->position.first_line,
                        "unmatching operands, type is not comparable");
                error_happen = true;
            }
            if (type2.value()->name != "int" && type2.value()->name != "float") {
                fprintf(output,
                        "Error type %d at Line %d: %s\n",
                        7,
                        Exp2->position.first_line,
                        "unmatching operands, type is not comparable");
                error_happen = true;
            }
            optional<shared_ptr<Type>> type = INT_TYPE;
            node->attributes["type"] = type;

        } else if (OP->name == "NE" || OP->name == "EQ") {
            optional<shared_ptr<Type>> type = INT_TYPE;
            node->attributes["type"] = type;

        } else if (OP->name == "PLUS" || OP->name == "MINUS" || OP->name == "MUL" || OP->name == "DIV") {
            if (type1.value()->name != "int" && type1.value()->name != "float") {
                fprintf(output,
                        "Error type %d at Line %d: %s\n",
                        7,
                        Exp1->position.first_line,
                        "binary operation on non-number variables");
                error_happen = true;
            }
            if (type2.value()->name != "int" && type2.value()->name != "float") {
                fprintf(output,
                        "Error type %d at Line %d: %s\n",
                        7,
                        Exp2->position.first_line,
                        "unmatching operands, type cannot do arithmetic operations");
                error_happen = true;
            }

            if (type1.value()->name == "int" && type2.value()->name == "int") {
                optional<shared_ptr<Type>> type = INT_TYPE;
                node->attributes["type"] = type;
            } else {
                optional<shared_ptr<Type>> type = FLOAT_TYPE;
                node->attributes["type"] = type;
            }
        }
    } else if (node->children.size() == 3 && node->children[0]->name == "LP" && node->children[2]->name == "RP") {
        // Exp: LP Exp RP
        ASTNode *Exp = node->children[1];

        visit_Exp(Exp);

        auto type = any_cast<optional<shared_ptr<Type>>>(Exp->attributes.at("type"));
        node->attributes["type"] = type;
    } else if (node->children.size() == 2 && node->children[1]->name == "Exp") {
        // Exp: MINUS Exp
        // Exp: NOT Exp
        ASTNode *OP = node->children[0];
        ASTNode *Exp = node->children[1];

        visit_Exp(Exp);

        auto type = any_cast<optional<shared_ptr<Type>>>(Exp->attributes.at("type"));
        node->attributes["type"] = type;

        if (OP->name == "MINUS") {
            if (type.has_value() && type.value()->name != "int" && type.value()->name != "float") {
                fprintf(output,
                        "Error type %d at Line %d: %s\n",
                        7,
                        Exp->position.first_line,
                        "unmatching operands, type cannot do arithmetic operations");
                error_happen = true;
            }
        } else if (OP->name == "NOT") {
            if (type.has_value() && type.value()->name != "int") {
                fprintf(output,
                        "Error type %d at Line %d: %s\n",
                        7,
                        Exp->position.first_line,
                        "unmatching operands, boolean expr is not int");
                error_happen = true;
            }
        }
    } else if (node->children.size() == 3 && node->children[1]->name == "LP" && node->children[2]->name == "RP") {
        // Exp: ID LP RP
        ASTNode *ID = node->children[0];

        auto id = any_cast<string>(ID->attributes.at("string_value"));

        if (functionTable.count(id) == 0) {
            if (findSymbol(id).has_value()) {
                fprintf(output,
                        "Error type %d at Line %d: %s\n",
                        11,
                        ID->position.first_line,
                        ("invoking non-function variable: " + id).data());
                error_happen = true;
            } else {
                fprintf(output,
                        "Error type %d at Line %d: %s\n",
                        2,
                        ID->position.first_line,
                        ("undefined function: " + id).data());
                error_happen = true;
            }
            optional<shared_ptr<Type>> type = optional<shared_ptr<Type>>();
            node->attributes["type"] = type;
            return;
        }

        shared_ptr<FunctionType> type = functionTable.at(id);

        optional<shared_ptr<Type>> return_type = type->returns;
        node->attributes["type"] = return_type;

        if (!type->parameters.empty()) {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    9,
                    ID->position.first_line,
                    ("invalid argument number for compare, expect " + std::to_string(type->parameters.size()) +
                     ", got 0").data());
            error_happen = true;
        }

    } else if (node->children.size() == 4 && node->children[1]->name == "LP" && node->children[3]->name == "RP") {
        // Exp: ID LP Args RP
        ASTNode *ID = node->children[0];
        ASTNode *Args = node->children[2];

        auto id = any_cast<string>(ID->attributes.at("string_value"));

        if (functionTable.count(id) == 0) {
            if (findSymbol(id).has_value()) {
                fprintf(output,
                        "Error type %d at Line %d: %s\n",
                        11,
                        ID->position.first_line,
                        ("invoking non-function variable: " + id).data());
                error_happen = true;
            } else {
                fprintf(output,
                        "Error type %d at Line %d: %s\n",
                        2,
                        ID->position.first_line,
                        ("undefined function: " + id).data());
                error_happen = true;
            }

            optional<shared_ptr<Type>> type = optional<shared_ptr<Type>>();
            node->attributes["type"] = type;
            return;
        }

        shared_ptr<FunctionType> type = functionTable.at(id);

        optional<shared_ptr<Type>> return_type = type->returns;
        node->attributes["type"] = return_type;

        visit_Args(Args);

        auto args_type = any_cast<vector<optional<shared_ptr<Type>>>>(Args->attributes.at("args_type"));

        auto &parameters = type->parameters;

        if (args_type.size() != parameters.size()) {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    9,
                    Args->position.first_line,
                    (
                            "invalid argument number for " + id +
                            ", expect " + std::to_string(parameters.size()) +
                            ", got " + std::to_string(args_type.size())
                    ).data()
            );
            error_happen = true;
            return;
        }

        for (int i = 0; i < parameters.size(); i++) {
            if (args_type[i].has_value() && args_type[i].value()->name != parameters[i].second->name) {
                fprintf(output,
                        "Error type %d at Line %d: %s\n",
                        9,
                        Args->position.first_line,
                        "the function’s arguments mismatch the declared parameters");
                error_happen = true;
                return;
            }
        }
    } else if (node->children.size() == 4 && node->children[1]->name == "LB" && node->children[3]->name == "RB") {
        // Exp: Exp LB Exp RB
        ASTNode *Exp1 = node->children[0];
        ASTNode *Exp2 = node->children[2];

        visit_Exp(Exp1);
        visit_Exp(Exp2);

        auto type1 = any_cast<optional<shared_ptr<Type>>>(Exp1->attributes.at("type"));

        if (!type1.has_value()) {
            optional<shared_ptr<Type>> type = optional<shared_ptr<Type>>();
            node->attributes["type"] = type;
            return;
        }

        if (dynamic_pointer_cast<ArrayType>(type1.value()) == nullptr) {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    10,
                    Exp1->position.first_line,
                    "indexing on non-array variable");
            error_happen = true;

            optional<shared_ptr<Type>> type = optional<shared_ptr<Type>>();
            node->attributes["type"] = type;
            return;
        }

        optional<shared_ptr<Type>> type = (dynamic_pointer_cast<ArrayType>(type1.value()))->type;
        node->attributes["type"] = type;
        node->attributes["lvalue"] = true;

        auto type2 = any_cast<optional<shared_ptr<Type>>>(Exp2->attributes.at("type"));

        if (type2.has_value() && type2.value()->name != "int") {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    12,
                    Exp2->position.first_line,
                    "indexing by non-integer");
            error_happen = true;
        }

    } else if (node->children.size() == 3 && node->children[1]->name == "DOT") {
        // Exp: Exp DOT ID
        ASTNode *Exp = node->children[0];
        ASTNode *ID = node->children[2];

        auto id = any_cast<string>(ID->attributes.at("string_value"));

        visit_Exp(Exp);

        auto exp_type = any_cast<optional<shared_ptr<Type>>>(Exp->attributes.at("type"));

        if (!exp_type.has_value()) {
            optional<shared_ptr<Type>> type = optional<shared_ptr<Type>>();
            node->attributes["type"] = type;
            return;
        }

        if (dynamic_pointer_cast<StructType>(exp_type.value()) == nullptr) {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    13,
                    Exp->position.first_line,
                    "accessing with non-struct variable");
            error_happen = true;

            optional<shared_ptr<Type>> type = optional<shared_ptr<Type>>();
            node->attributes["type"] = type;
            return;
        }

        auto struct_type = dynamic_pointer_cast<StructType>(exp_type.value());

        shared_ptr<Type> find_member = nullptr;
        for (auto &p : struct_type->fields) {
            if (p.first == id) {
                find_member = p.second;
                break;
            }
        }

        if (find_member == nullptr) {
            fprintf(output,
                    "Error type %d at Line %d: %s\n",
                    14,
                    Exp->position.first_line,
                    ("no such member: " + id).data());
            error_happen = true;

            optional<shared_ptr<Type>> type = optional<shared_ptr<Type>>();
            node->attributes["type"] = type;
            return;
        }

        optional<shared_ptr<Type>> type = find_member;
        node->attributes["type"] = type;
        node->attributes["lvalue"] = true;

    } else {
        throw runtime_error("error Exp children");
    }

}

// args_type: vector<optional<shared_ptr<Type>>>
void ASTAnalyzer::visit_Args(ASTNode *node) {
    if (node->children.size() == 1) {
        // Args: Exp
        ASTNode *Exp = node->children[0];

        visit_Exp(Exp);

        auto type = any_cast<optional<shared_ptr<Type>>>(Exp->attributes.at("type"));

        auto args_type = vector<optional<shared_ptr<Type>>>();
        args_type.push_back(type);

        node->attributes["args_type"] = args_type;

    } else if (node->children.size() == 3) {
        // Args: Exp COMMA Args
        ASTNode *Exp = node->children[0];
        ASTNode *Args = node->children[2];

        visit_Exp(Exp);
        visit_Args(Args);

        auto type = any_cast<optional<shared_ptr<Type>>>(Exp->attributes.at("type"));
        auto argsA = any_cast<vector<optional<shared_ptr<Type>>>>(Args->attributes.at("args_type"));

        auto args_type = vector<optional<shared_ptr<Type>>>();
        args_type.push_back(type);
        args_type.insert(args_type.end(), argsA.begin(), argsA.end());

        node->attributes["args_type"] = args_type;

    }
}

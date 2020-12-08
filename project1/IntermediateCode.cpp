
#include "IntermediateCode.hpp"

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

Code::Code(string code) : code(std::move(code)), codes() {}

Code::Code(std::vector<std::shared_ptr<Code>> codes) : code(), codes(codes) {}

std::string IntermediateCode::createSymbol(const std::string &symbol) {
    if (symbolTable.find(symbol) != symbolTable.end()) {
        throw runtime_error("symbol " + symbol + " already exist");
    }
//    string name = "v" + std::to_string(variable_count);
//    variable_count++;
//    symbolTable[symbol] = name;

    string name = symbol;
    symbolTable[symbol] = name;

    return name;
}

std::string IntermediateCode::createTempSymbol() {
    string name = "t" + std::to_string(temp_count);
    temp_count++;
    return name;
}

std::string IntermediateCode::createLabel() {
    string name = "label" + std::to_string(label_count);
    label_count++;
    return name;
}

FILE *ir_output;

IntermediateCode::IntermediateCode(ASTNode *node) : root(node) {
    ir_output = fopen("/dev/stdout", "w+");
}

void IntermediateCode::set_file(const std::string &filename) {
    if (filename.substr(filename.length() - 4, 4) == ".spl") {
        string output_file = filename.substr(0, filename.length() - 4) + ".ir";
        ir_output = fopen(output_file.data(), "w+");
    } else {
        printf("output to stdout");
    }
}

void print_code(shared_ptr<Code> code) {
    if (!code->code.empty()) {
        fprintf(ir_output, "%s\n", code->code.data());
    } else {
        for (auto it: code->codes) {
            print_code(it);
        }
    }
}

void IntermediateCode::generate() {
    try {
        visit_node(this->root);

        auto code = any_cast<shared_ptr<Code>>(this->root->attributes.at("ircode"));
        print_code(code);

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

void IntermediateCode::visit_Program(ASTNode *node) {
    ASTNode *ExtDefList = node->children[1];

    visit_ExtDefList(ExtDefList);

    node->attributes["ircode"] = ExtDefList->attributes.at("ircode");
}


// definitions: vector<pair<string, shared_ptr<Type>>>
void IntermediateCode::visit_ExtDefList(ASTNode *node) {
    if (node->children.empty()) {
        // ExtDefList: %empty
        node->attributes["ircode"] = make_shared<Code>(Code(""));

    } else if (node->children.size() == 2) {
        // ExtDefList: ExtDef ExtDefList
        ASTNode *ExtDef = node->children[0];
        ASTNode *ExtDefList = node->children[1];

        visit_ExtDef(ExtDef);
        visit_ExtDefList(ExtDefList);

        auto codeA = any_cast<shared_ptr<Code>>(ExtDef->attributes.at("ircode"));
        auto codeB = any_cast<shared_ptr<Code>>(ExtDefList->attributes.at("ircode"));

        auto code = make_shared<Code>(Code({codeA, codeB}));

        node->attributes["ircode"] = code;

    } else {
        throw runtime_error("error ExtDefList children");
    }
}

// definitions: vector<pair<string, shared_ptr<Type>>>
void IntermediateCode::visit_ExtDef(ASTNode *node) {
    if (node->children.size() == 2 && node->children[1]->name == "SEMI") {
        // ExtDef: Specifier SEMI
        ASTNode *Specifier = node->children[0];

//        visit_Specifier(Specifier);

        throw runtime_error("not generate code for ExtDef: Specifier SEMI");

    } else if (node->children.size() == 3 && node->children[2]->name == "SEMI") {
        // ExtDef: Specifier ExtDecList SEMI
        ASTNode *Specifier = node->children[0];
        ASTNode *ExtDecList = node->children[1];

//        visit_Specifier(Specifier);
//        visit_ExtDecList(ExtDecList);

        throw runtime_error("not generate code for ExtDef: Specifier ExtDecList SEMI");

    } else if (node->children.size() == 3 && node->children[1]->name == "FunDec") {
        // ExtDef: Specifier FunDec CompSt
        ASTNode *Specifier = node->children[0];
        ASTNode *FunDec = node->children[1];
        ASTNode *CompSt = node->children[2];

//        visit_Specifier(Specifier);

        visit_FunDec(FunDec);
        visit_CompSt(CompSt);

        auto codeA = any_cast<shared_ptr<Code>>(FunDec->attributes.at("ircode"));
        auto codeB = any_cast<shared_ptr<Code>>(CompSt->attributes.at("ircode"));

        auto code = make_shared<Code>(Code({codeA, codeB}));

        node->attributes["ircode"] = code;

    } else {
        throw runtime_error("error ExtDef children");
    }
}

// definitions: vector<pair<string, shared_ptr<Type>>>
void IntermediateCode::visit_ExtDecList(ASTNode *node) {
    if (node->children.size() == 1) {
        // ExtDecList: VarDec
        ASTNode *VarDec = node->children[0];

//        visit_VarDec(VarDec);

        throw runtime_error("not generate code for ExtDecList");

    } else if (node->children.size() == 3) {
        // ExtDecList: VarDec COMMA ExtDecList
        ASTNode *VarDec = node->children[0];
        ASTNode *ExtDecList = node->children[2];

//        visit_VarDec(VarDec);
//        visit_ExtDecList(ExtDecList);

        throw runtime_error("not generate code for ExtDecList");

    } else {
        throw runtime_error("error DecList children");
    }

}

// type: Type*
void IntermediateCode::visit_Specifier(ASTNode *node) {
    if (node->children[0]->name == "TYPE") {
        // Specifier: TYPE
        ASTNode *TYPE = node->children[0];

        throw runtime_error("not generate code for Specifier");

    } else if (node->children[0]->name == "StructSpecifier") {
        // Specifier: StructSpecifier
        ASTNode *StructSpecifier = node->children[0];

//        visit_StructSpecifier(StructSpecifier);

        throw runtime_error("not generate code for Specifier");


    } else {
        throw runtime_error("error Specifier child");
    }

}

// type: Type*
void IntermediateCode::visit_StructSpecifier(ASTNode *node) {
    if (node->children.size() == 2) {
        // StructSpecifier: STRUCT ID
        ASTNode *ID = node->children[1];

        throw runtime_error("not generate code for StructSpecifier");

    } else if (node->children.size() == 5) {
        // StructSpecifier: STRUCT ID LC DefList RC
        ASTNode *ID = node->children[1];
        ASTNode *DefList = node->children[3];

//        visit_DefList(DefList);

        throw runtime_error("not generate code for StructSpecifier");

    } else {
        throw runtime_error("error StructSpecifier children");
    }
}

// declaration: pair<string, shared_ptr<Type>>
void IntermediateCode::visit_VarDec(ASTNode *node) {
    if (node->children.size() == 1) {
        // VarDec: ID
        ASTNode *ID = node->children[0];

        auto id = any_cast<string>(ID->attributes.at("string_value"));

        createSymbol(id);

        node->attributes["irname"] = id;
        node->attributes["ircode"] = make_shared<Code>(Code(""));

    } else if (node->children.size() == 4) {
        // VarDec: VarDec LB INT RB
        ASTNode *VarDec = node->children[0];
        ASTNode *INT = node->children[2];

//        visit_VarDec(VarDec);

        throw runtime_error("not generate code for VarDec: VarDec LB INT RB");

    } else {
        throw runtime_error("error VarDec children");
    }
}

// declaration: pair<string, shared_ptr<Type>>
void IntermediateCode::visit_FunDec(ASTNode *node) {
    auto returnType = any_cast<shared_ptr<Type>>(node->parent->attributes.at("return_type"));

    if (node->children.size() == 3) {
        // FunDec: ID LP RP
        ASTNode *ID = node->children[0];

        auto id = any_cast<string>(ID->attributes.at("string_value"));

        node->attributes["ircode"] = make_shared<Code>(Code("FUNCTION " + id + " :"));

    } else if (node->children.size() == 4) {
        // FunDec: ID LP VarList RP
        ASTNode *ID = node->children[0];
        ASTNode *VarList = node->children[2];

        auto id = any_cast<string>(ID->attributes.at("string_value"));

//        visit_VarList(VarList);

        auto parameters = any_cast<vector<pair<string, shared_ptr<Type>>>>(VarList->attributes.at("parameters"));

        vector<shared_ptr<Code>> codes{};

        codes.push_back(make_shared<Code>(Code("FUNCTION " + id + " :")));

        for (const auto &it:parameters) {
            codes.push_back(make_shared<Code>(Code("PARAM " + createSymbol(it.first))));
        }

        node->attributes["ircode"] = make_shared<Code>(Code(codes));

    } else {
        throw runtime_error("error FunDec children");
    }
}

// parameters: vector<pair<string, shared_ptr<Type>>>
void IntermediateCode::visit_VarList(ASTNode *node) {
    if (node->children.size() == 1) {
        // VarList: ParamDec
        ASTNode *ParamDec = node->children[0];

//        visit_ParamDec(ParamDec);

        throw runtime_error("not generate code for VarList");

    } else if (node->children.size() == 3) {
        // VarList: ParamDec COMMA VarList
        ASTNode *ParamDec = node->children[0];
        ASTNode *VarList = node->children[2];

//        visit_ParamDec(ParamDec);
//        visit_VarList(VarList);

        throw runtime_error("not generate code for VarList");

    } else {
        throw runtime_error("error VarList children");
    }
}

// declaration: pair<string, shared_ptr<Type>>
// type: Type*
void IntermediateCode::visit_ParamDec(ASTNode *node) {
    // ParamDec: Specifier VarDec
    ASTNode *Specifier = node->children[0];
    ASTNode *VarDec = node->children[1];

//    visit_Specifier(Specifier);
//    visit_VarDec(VarDec);

    throw runtime_error("not generate code for ParamDec");

}

// return_type: Type*
void IntermediateCode::visit_CompSt(ASTNode *node) {
    // CompSt: LC DefList StmtList RC
    ASTNode *DefList = node->children[1];
    ASTNode *StmtList = node->children[2];

    visit_DefList(DefList);
    visit_StmtList(StmtList);

    auto codeA = any_cast<shared_ptr<Code>>(DefList->attributes.at("ircode"));
    auto codeB = any_cast<shared_ptr<Code>>(StmtList->attributes.at("ircode"));

    auto code = make_shared<Code>(Code({codeA, codeB}));

    node->attributes["ircode"] = code;

}

// return_type: Type*
void IntermediateCode::visit_StmtList(ASTNode *node) {
    node->attributes["return_type"] = node->parent->attributes.at("return_type");

    if (node->children.empty()) {
        // StmtList: %empty

        node->attributes["ircode"] = make_shared<Code>(Code(""));

    } else if (node->children.size() == 2) {
        // StmtList: Stmt StmtList
        ASTNode *Stmt = node->children[0];
        ASTNode *StmtList = node->children[1];

        visit_Stmt(Stmt);

        visit_StmtList(StmtList);

        auto codeA = any_cast<shared_ptr<Code>>(Stmt->attributes.at("ircode"));
        auto codeB = any_cast<shared_ptr<Code>>(StmtList->attributes.at("ircode"));

        auto code = make_shared<Code>(Code({codeA, codeB}));

        node->attributes["ircode"] = code;

    } else {
        throw runtime_error("error StmtList children");
    }

}


// return_type: Type*
void IntermediateCode::visit_Stmt(ASTNode *node) {
    node->attributes["return_type"] = node->parent->attributes.at("return_type");

    if (node->children.size() == 2 && node->children[0]->name == "Exp") {
        // Stmt: Exp SEMI
        ASTNode *Exp = node->children[0];

        string place = createTempSymbol();
        node->attributes["irplace"] = place;

        visit_Exp(Exp);

        node->attributes["ircode"] = Exp->attributes.at("ircode");

    } else if (node->children.size() == 1 && node->children[0]->name == "CompSt") {
        // Stmt: CompSt
        ASTNode *CompSt = node->children[0];

        visit_CompSt(CompSt);

        node->attributes["ircode"] = CompSt->attributes.at("ircode");

    } else if (node->children.size() == 3 && node->children[0]->name == "RETURN") {
        // Stmt: RETURN Exp SEMI
        ASTNode *Exp = node->children[1];

        string place = createTempSymbol();
        node->attributes["irplace"] = place;

        visit_Exp(Exp);

        auto codeA = any_cast<shared_ptr<Code>>(Exp->attributes.at("ircode"));
        auto codeB = make_shared<Code>(Code("RETURN " + place));

        auto code = make_shared<Code>(Code({codeA, codeB}));

        node->attributes["ircode"] = code;

    } else if (node->children.size() == 5 && node->children[0]->name == "IF") {
        // Stmt: IF LP Exp RP Stmt
        ASTNode *Exp = node->children[2];
        ASTNode *Stmt = node->children[4];

        string label_true = createLabel();
        string label_false = createLabel();

        string place = createTempSymbol();
        node->attributes["irplace"] = place;

        visit_Exp(Exp);
        visit_Stmt(Stmt);

        vector<shared_ptr<Code>> codes{
                any_cast<shared_ptr<Code>>(Exp->attributes.at("ircode")),
                make_shared<Code>(Code("IF " + place + " != #0 GOTO " + label_true)),
                make_shared<Code>(Code("GOTO " + label_false)),
                make_shared<Code>(Code("LABEL " + label_true + " :")),
                any_cast<shared_ptr<Code>>(Stmt->attributes.at("ircode")),
                make_shared<Code>(Code("LABEL " + label_false + " :"))
        };

        auto code = make_shared<Code>(Code(codes));

        node->attributes["ircode"] = code;

    } else if (node->children.size() == 7 && node->children[0]->name == "IF") {
        // Stmt: IF LP Exp RP Stmt ELSE Stmt
        ASTNode *Exp = node->children[2];
        ASTNode *Stmt1 = node->children[4];
        ASTNode *Stmt2 = node->children[6];

        string label_true = createLabel();
        string label_false = createLabel();
        string label_end = createLabel();

        string place = createTempSymbol();
        node->attributes["irplace"] = place;

        visit_Exp(Exp);

        visit_Stmt(Stmt1);
        visit_Stmt(Stmt2);

        vector<shared_ptr<Code>> codes{
                any_cast<shared_ptr<Code>>(Exp->attributes.at("ircode")),
                make_shared<Code>(Code("IF " + place + " != #0 GOTO " + label_true)),
                make_shared<Code>(Code("GOTO " + label_false)),
                make_shared<Code>(Code("LABEL " + label_true + " :")),
                any_cast<shared_ptr<Code>>(Stmt1->attributes.at("ircode")),
                make_shared<Code>(Code("GOTO " + label_end)),
                make_shared<Code>(Code("LABEL " + label_false + " :")),
                any_cast<shared_ptr<Code>>(Stmt2->attributes.at("ircode")),
                make_shared<Code>(Code("LABEL " + label_end + " :"))
        };

        auto code = make_shared<Code>(Code(codes));

        node->attributes["ircode"] = code;

    } else if (node->children.size() == 5 && node->children[0]->name == "WHILE") {
        // Stmt: WHILE LP Exp RP Stmt
        ASTNode *Exp = node->children[2];
        ASTNode *Stmt = node->children[4];

        string label_loop = createLabel();
        string label_end = createLabel();

        string place = createTempSymbol();
        node->attributes["irplace"] = place;

        visit_Exp(Exp);

        loopTable.emplace_back(LoopItem{"", label_loop, label_end});
        visit_Stmt(Stmt);
        loopTable.pop_back();

        vector<shared_ptr<Code>> codes{
                make_shared<Code>(Code("LABEL " + label_loop + " :")),
                any_cast<shared_ptr<Code>>(Exp->attributes.at("ircode")),
                make_shared<Code>(Code("IF " + place + " == #0 GOTO " + label_end)),
                any_cast<shared_ptr<Code>>(Stmt->attributes.at("ircode")),
                make_shared<Code>(Code("GOTO " + label_loop)),
                make_shared<Code>(Code("LABEL " + label_end + " :"))
        };

        auto code = make_shared<Code>(Code(codes));

        node->attributes["ircode"] = code;

    } else if (node->children.size() == 7 && node->children[2]->name == "WHILE") {
        // Stmt: ID COLON WHILE LP Exp RP Stmt
        ASTNode *ID = node->children[0];
        ASTNode *Exp = node->children[4];
        ASTNode *Stmt = node->children[6];

        auto loop_label = any_cast<string>(ID->attributes.at("string_value"));

        string label_loop = createLabel();
        string label_end = createLabel();

        string place = createTempSymbol();
        node->attributes["irplace"] = place;

        visit_Exp(Exp);

        loopTable.emplace_back(LoopItem{loop_label, label_loop, label_end});
        visit_Stmt(Stmt);
        loopTable.pop_back();

        vector<shared_ptr<Code>> codes{
                make_shared<Code>(Code("LABEL " + label_loop + " :")),
                any_cast<shared_ptr<Code>>(Exp->attributes.at("ircode")),
                make_shared<Code>(Code("IF " + place + " == #0 GOTO " + label_end)),
                any_cast<shared_ptr<Code>>(Stmt->attributes.at("ircode")),
                make_shared<Code>(Code("GOTO " + label_loop)),
                make_shared<Code>(Code("LABEL " + label_end + " :"))
        };

        auto code = make_shared<Code>(Code(codes));

        node->attributes["ircode"] = code;

    } else if (node->children.size() == 7 && node->children[0]->name == "FOR") {
        // Stmt: FOR ID IN Exp DOUBLE_DOT Exp CompSt
        ASTNode *ID = node->children[1];
        ASTNode *Exp1 = node->children[3];
        ASTNode *Exp2 = node->children[5];
        ASTNode *CompSt = node->children[6];

        auto it_name = any_cast<string>(ID->attributes.at("string_value"));

        string place1 = createTempSymbol();
        string place2 = createTempSymbol();
        string place_it = createSymbol(it_name);

        node->attributes["irplace"] = place1;
        visit_Exp(Exp1);

        node->attributes["irplace"] = place2;
        visit_Exp(Exp2);

        string label_loop = createLabel();
        string label_end = createLabel();
        string label_start = createLabel();

        loopTable.emplace_back(LoopItem{"", label_loop, label_end});
        visit_Stmt(CompSt);
        loopTable.pop_back();

        vector<shared_ptr<Code>> codes{
                any_cast<shared_ptr<Code>>(Exp1->attributes.at("ircode")),
                any_cast<shared_ptr<Code>>(Exp2->attributes.at("ircode")),
                make_shared<Code>(Code(place_it + " := " + place1)),
                make_shared<Code>(Code("LABEL " + label_start + " :")),
                make_shared<Code>(Code("IF " + place_it + " > " + place2 + " #0 GOTO " + label_end)),
                any_cast<shared_ptr<Code>>(CompSt->attributes.at("ircode")),
                make_shared<Code>(Code("LABEL " + label_loop + " :")),
                make_shared<Code>(Code(place_it + " := " + place_it + " + #1")),
                make_shared<Code>(Code("GOTO " + label_start)),
                make_shared<Code>(Code("LABEL " + label_end + " :"))
        };

        auto code = make_shared<Code>(Code(codes));

        node->attributes["ircode"] = code;

    } else if (node->children.size() == 9 && node->children[2]->name == "FOR") {
        // Stmt: ID COLON FOR ID IN Exp DOUBLE_DOT Exp CompSt
        ASTNode *ID0 = node->children[0];
        ASTNode *ID1 = node->children[3];
        ASTNode *Exp1 = node->children[5];
        ASTNode *Exp2 = node->children[7];
        ASTNode *CompSt = node->children[8];

        auto loop_label = any_cast<string>(ID0->attributes.at("string_value"));
        auto it_name = any_cast<string>(ID1->attributes.at("string_value"));

        string place1 = createTempSymbol();
        string place2 = createTempSymbol();
        string place_it = createSymbol(it_name);

        node->attributes["irplace"] = place1;
        visit_Exp(Exp1);

        node->attributes["irplace"] = place2;
        visit_Exp(Exp2);

        string label_loop = createLabel();
        string label_end = createLabel();
        string label_start = createLabel();

        loopTable.emplace_back(LoopItem{loop_label, label_loop, label_end});
        visit_Stmt(CompSt);
        loopTable.pop_back();

        vector<shared_ptr<Code>> codes{
                any_cast<shared_ptr<Code>>(Exp1->attributes.at("ircode")),
                any_cast<shared_ptr<Code>>(Exp2->attributes.at("ircode")),
                make_shared<Code>(Code(place_it + " := " + place1)),
                make_shared<Code>(Code("LABEL " + label_start + " :")),
                make_shared<Code>(Code("IF " + place_it + " > " + place2 + " #0 GOTO " + label_end)),
                any_cast<shared_ptr<Code>>(CompSt->attributes.at("ircode")),
                make_shared<Code>(Code("LABEL " + label_loop + " :")),
                make_shared<Code>(Code(place_it + " := " + place_it + " + #1")),
                make_shared<Code>(Code("GOTO " + label_start)),
                make_shared<Code>(Code("LABEL " + label_end + " :"))
        };

        auto code = make_shared<Code>(Code(codes));

        node->attributes["ircode"] = code;

    } else if (node->children.size() == 2 && node->children[0]->name == "BREAK") {
        // Stmt: BREAK SEMI

        const LoopItem &item = loopTable.back();

        auto code = make_shared<Code>(Code("GOTO " + item.endLabel));

        node->attributes["ircode"] = code;

    } else if (node->children.size() == 3 && node->children[0]->name == "BREAK") {
        // Stmt: BREAK ID SEMI
        ASTNode *ID = node->children[1];

        auto loop_label = any_cast<string>(ID->attributes.at("string_value"));

        const LoopItem *item = nullptr;

        for (auto it = loopTable.rbegin(); it != loopTable.rend(); it++) {
            if (it->name == loop_label) {
                item = &*it;
                break;
            }
        }

        auto code = make_shared<Code>(Code("GOTO " + item->endLabel));

        node->attributes["ircode"] = code;

    } else if (node->children.size() == 2 && node->children[0]->name == "CONTINUE") {
        // Stmt: CONTINUE SEMI

        const LoopItem &item = loopTable.back();

        auto code = make_shared<Code>(Code("GOTO " + item.loopLabel));

        node->attributes["ircode"] = code;

    } else if (node->children.size() == 3 && node->children[0]->name == "CONTINUE") {
        // Stmt: CONTINUE ID SEMI
        ASTNode *ID = node->children[1];

        auto loop_label = any_cast<string>(ID->attributes.at("string_value"));

        const LoopItem *item = nullptr;

        for (auto it = loopTable.rbegin(); it != loopTable.rend(); it++) {
            if (it->name == loop_label) {
                item = &*it;
                break;
            }
        }

        auto code = make_shared<Code>(Code("GOTO " + item->loopLabel));

        node->attributes["ircode"] = code;

    } else {
        throw runtime_error("error Stmt children");
    }

}


// definitions: vector<pair<string, shared_ptr<Type>>>
void IntermediateCode::visit_DefList(ASTNode *node) {
    if (node->children.empty()) {
        // DefList: %empty

        node->attributes["ircode"] = make_shared<Code>(Code(""));

    } else if (node->children.size() == 2) {
        // DefList: Def DefList
        ASTNode *Def = node->children[0];
        ASTNode *DefList = node->children[1];

        visit_Def(Def);
        visit_DefList(DefList);

        auto codeA = any_cast<shared_ptr<Code>>(Def->attributes.at("ircode"));
        auto codeB = any_cast<shared_ptr<Code>>(DefList->attributes.at("ircode"));

        auto code = make_shared<Code>(Code({codeA, codeB}));

        node->attributes["ircode"] = code;

    } else {
        throw runtime_error("error DefList children");
    }
}

// definitions: vector<pair<string, shared_ptr<Type>>>
// type: Type*
void IntermediateCode::visit_Def(ASTNode *node) {
    // Def: Specifier DecList SEMI
    ASTNode *Specifier = node->children[0];
    ASTNode *DecList = node->children[1];

//    visit_Specifier(Specifier);

    visit_DecList(DecList);

    node->attributes["ircode"] = DecList->attributes.at("ircode");

}

// definitions: vector<pair<string, shared_ptr<Type>>>
// type: Type*
void IntermediateCode::visit_DecList(ASTNode *node) {
    node->attributes["type"] = node->parent->attributes.at("type");

    if (node->children.size() == 1) {
        // DecList: Dec
        ASTNode *Dec = node->children[0];

        visit_Dec(Dec);

        node->attributes["ircode"] = Dec->attributes.at("ircode");

    } else if (node->children.size() == 3) {
        // DecList: Dec COMMA DecList
        ASTNode *Dec = node->children[0];
        ASTNode *DecList = node->children[2];

        visit_Dec(Dec);
        visit_DecList(DecList);

        auto codeA = any_cast<shared_ptr<Code>>(Dec->attributes.at("ircode"));
        auto codeB = any_cast<shared_ptr<Code>>(DecList->attributes.at("ircode"));

        auto code = make_shared<Code>(Code({codeA, codeB}));

        node->attributes["ircode"] = code;

    } else {
        throw runtime_error("error DecList children");
    }
}

// declaration: pair<string, shared_ptr<Type>>
// type: Type*
void IntermediateCode::visit_Dec(ASTNode *node) {
    node->attributes["type"] = node->parent->attributes.at("type");

    if (node->children.size() == 1) {
        // Dec: VarDec
        ASTNode *VarDec = node->children[0];

        visit_VarDec(VarDec);

        node->attributes["ircode"] = VarDec->attributes.at("ircode");

    } else if (node->children.size() == 3) {
        // Dec: VarDec ASSIGN Exp
        ASTNode *VarDec = node->children[0];
        ASTNode *Exp = node->children[2];

        visit_VarDec(VarDec);

        auto id = any_cast<string>(VarDec->attributes.at("irname"));
        node->attributes["irplace"] = symbolTable.at(id);

        visit_Exp(Exp);

        auto codeA = any_cast<shared_ptr<Code>>(VarDec->attributes.at("ircode"));
        auto codeB = any_cast<shared_ptr<Code>>(Exp->attributes.at("ircode"));

        auto code = make_shared<Code>(Code({codeA, codeB}));

        node->attributes["ircode"] = code;

    } else {
        throw runtime_error("error Dec children");
    }
}

// type: optional<shared_ptr<Type>>
// lvalue: bool
void IntermediateCode::visit_Exp(ASTNode *node) {
    auto target = any_cast<string>(node->parent->attributes.at("irplace"));

    if (node->children.size() == 1 && node->children[0]->name == "ID") {
        // Exp: ID
        ASTNode *ID = node->children[0];

        auto id = any_cast<string>(ID->attributes.at("string_value"));

        node->attributes["irname"] = id;
        node->attributes["ircode"] = make_shared<Code>(Code(target + " := " + symbolTable.at(id)));

    } else if (node->children.size() == 1 && node->children[0]->name == "INT") {
        // Exp: INT
        ASTNode *INT = node->children[0];

        auto value = any_cast<int>(INT->attributes.at("int_value"));

        node->attributes["ircode"] = make_shared<Code>(Code(target + " := #" + std::to_string(value)));

    } else if (node->children.size() == 1 && node->children[0]->name == "FLOAT") {
        // Exp: FLOAT

        throw runtime_error("not generate code for Exp: FLOAT");

    } else if (node->children.size() == 1 && node->children[0]->name == "CHAR") {
        // Exp: CHAR

        throw runtime_error("not generate code for Exp: CHAR");

    } else if (node->children.size() == 1 && node->children[0]->name == "String") {
        // Exp: String

        throw runtime_error("not generate code for Exp: String");

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

        string place1 = createTempSymbol();
        string place2 = createTempSymbol();

        node->attributes["irplace"] = place1;
        visit_Exp(Exp1);

        node->attributes["irplace"] = place2;
        visit_Exp(Exp2);

        if (OP->name == "ASSIGN") {
            auto id = any_cast<string>(Exp1->attributes.at("irname"));

            vector<shared_ptr<Code>> codes{
                    any_cast<shared_ptr<Code>>(Exp2->attributes.at("ircode")),
                    make_shared<Code>(Code(symbolTable.at(id) + " := " + place2)),
                    make_shared<Code>(Code(target + " := " + place2)),
            };

            auto code = make_shared<Code>(Code(codes));

            node->attributes["ircode"] = code;

        } else if (OP->name == "AND") {
            string label_false = createLabel();
            string label_end = createLabel();

            vector<shared_ptr<Code>> codes{
                    any_cast<shared_ptr<Code>>(Exp1->attributes.at("ircode")),
                    make_shared<Code>(Code("IF " + place1 + " == #0 GOTO " + label_false)),
                    any_cast<shared_ptr<Code>>(Exp2->attributes.at("ircode")),
                    make_shared<Code>(Code("IF " + place2 + " == #0 GOTO " + label_false)),
                    make_shared<Code>(Code(target + " := #1")),
                    make_shared<Code>(Code("GOTO " + label_end)),
                    make_shared<Code>(Code("LABEL " + label_false + " :")),
                    make_shared<Code>(Code(target + " := #0")),
                    make_shared<Code>(Code("LABEL " + label_end + " :")),
            };

            auto code = make_shared<Code>(Code(codes));

            node->attributes["ircode"] = code;

        } else if (OP->name == "OR") {
            string label_true = createLabel();
            string label_end = createLabel();

            vector<shared_ptr<Code>> codes{
                    any_cast<shared_ptr<Code>>(Exp1->attributes.at("ircode")),
                    make_shared<Code>(Code("IF " + place1 + " != #0 GOTO " + label_true)),
                    any_cast<shared_ptr<Code>>(Exp2->attributes.at("ircode")),
                    make_shared<Code>(Code("IF " + place2 + " != #0 GOTO " + label_true)),
                    make_shared<Code>(Code(target + " := #0")),
                    make_shared<Code>(Code("GOTO " + label_end)),
                    make_shared<Code>(Code("LABEL " + label_true + " :")),
                    make_shared<Code>(Code(target + " := #1")),
                    make_shared<Code>(Code("LABEL " + label_end + " :")),
            };

            auto code = make_shared<Code>(Code(codes));

            node->attributes["ircode"] = code;

        } else if (OP->name == "LT" || OP->name == "LE" || OP->name == "GT" || OP->name == "GE" ||
                   OP->name == "NE" || OP->name == "EQ") {
            string label_true = createLabel();
            string label_end = createLabel();

            string op;
            if (OP->name == "LT") {
                op = "<";
            } else if (OP->name == "LE") {
                op = "<=";
            } else if (OP->name == "GT") {
                op = ">";
            } else if (OP->name == "GE") {
                op = ">=";
            } else if (OP->name == "NE") {
                op = "!=";
            } else if (OP->name == "EQ") {
                op = "==";
            } else {
                throw runtime_error("unexpected operation " + OP->name);
            }

            vector<shared_ptr<Code>> codes{
                    any_cast<shared_ptr<Code>>(Exp1->attributes.at("ircode")),
                    any_cast<shared_ptr<Code>>(Exp2->attributes.at("ircode")),
                    make_shared<Code>(Code("IF " + place1 + " " + op + " " + place2 + " GOTO " + label_true)),
                    make_shared<Code>(Code(target + " := #0")),
                    make_shared<Code>(Code("GOTO " + label_end)),
                    make_shared<Code>(Code("LABEL " + label_true + " :")),
                    make_shared<Code>(Code(target + " := #1")),
                    make_shared<Code>(Code("LABEL " + label_end + " :")),
            };

            auto code = make_shared<Code>(Code(codes));

            node->attributes["ircode"] = code;

        } else if (OP->name == "PLUS" || OP->name == "MINUS" || OP->name == "MUL" || OP->name == "DIV") {

            string op;
            if (OP->name == "PLUS") {
                op = "+";
            } else if (OP->name == "MINUS") {
                op = "-";
            } else if (OP->name == "MUL") {
                op = "*";
            } else if (OP->name == "DIV") {
                op = "/";
            } else {
                throw runtime_error("unexpected operation " + OP->name);
            }

            vector<shared_ptr<Code>> codes{
                    any_cast<shared_ptr<Code>>(Exp1->attributes.at("ircode")),
                    any_cast<shared_ptr<Code>>(Exp2->attributes.at("ircode")),
                    make_shared<Code>(Code(target + " := " + place1 + " " + op + " " + place2)),
            };

            auto code = make_shared<Code>(Code(codes));

            node->attributes["ircode"] = code;
        }
    } else if (node->children.size() == 3 && node->children[0]->name == "LP" && node->children[2]->name == "RP") {
        // Exp: LP Exp RP
        ASTNode *Exp = node->children[1];

        node->attributes["irplace"] = target;
        visit_Exp(Exp);

        node->attributes["ircode"] = Exp->attributes.at("ircode");

    } else if (node->children.size() == 2 && node->children[1]->name == "Exp") {
        // Exp: MINUS Exp
        // Exp: NOT Exp
        ASTNode *OP = node->children[0];
        ASTNode *Exp = node->children[1];

        string place = createTempSymbol();

        node->attributes["irplace"] = place;
        visit_Exp(Exp);

        if (OP->name == "MINUS") {
            vector<shared_ptr<Code>> codes{
                    any_cast<shared_ptr<Code>>(Exp->attributes.at("ircode")),
                    make_shared<Code>(Code(target + " := #0 - " + place)),
            };

            auto code = make_shared<Code>(Code(codes));

            node->attributes["ircode"] = code;

        } else if (OP->name == "NOT") {

            string label_false = createLabel();
            string label_end = createLabel();

            vector<shared_ptr<Code>> codes{
                    any_cast<shared_ptr<Code>>(Exp->attributes.at("ircode")),
                    make_shared<Code>(Code("IF " + place + " != #0 GOTO " + label_false)),
                    make_shared<Code>(Code(target + " := #1")),
                    make_shared<Code>(Code("GOTO " + label_end)),
                    make_shared<Code>(Code("LABEL " + label_false + " :")),
                    make_shared<Code>(Code(target + " := #0")),
                    make_shared<Code>(Code("LABEL " + label_end + " :")),
            };

            auto code = make_shared<Code>(Code(codes));

            node->attributes["ircode"] = code;
        }
    } else if (node->children.size() == 3 && node->children[1]->name == "LP" && node->children[2]->name == "RP") {
        // Exp: ID LP RP
        ASTNode *ID = node->children[0];

        auto id = any_cast<string>(ID->attributes.at("string_value"));

        if (id == "read") {
            auto code = make_shared<Code>(Code("READ " + target));

            node->attributes["ircode"] = code;
        } else {
            auto code = make_shared<Code>(Code(target + " := CALL " + id));

            node->attributes["ircode"] = code;
        }

    } else if (node->children.size() == 4 && node->children[1]->name == "LP" && node->children[3]->name == "RP") {
        // Exp: ID LP Args RP
        ASTNode *ID = node->children[0];
        ASTNode *Args = node->children[2];

        auto id = any_cast<string>(ID->attributes.at("string_value"));

        visit_Args(Args);

        auto args = any_cast<vector<string>>(Args->attributes.at("irargs"));

        if (id == "write") {
            vector<shared_ptr<Code>> codes{
                    any_cast<shared_ptr<Code>>(Args->attributes.at("ircode")),
                make_shared<Code>(Code("WRITE " + args[0]))
            };

            auto code = make_shared<Code>(codes);

            node->attributes["ircode"] = code;
        } else {

            vector<shared_ptr<Code>> codes{};

            codes.push_back(any_cast<shared_ptr<Code>>(Args->attributes.at("ircode")));

            for (auto it = args.rbegin(); it != args.rend(); it++) {
                codes.push_back(make_shared<Code>(Code("ARG " + *it)));
            }

            codes.push_back(make_shared<Code>(Code(target + " := CALL " + id)));

            auto code = make_shared<Code>(Code(codes));

            node->attributes["ircode"] = code;
        }

    } else if (node->children.size() == 4 && node->children[1]->name == "LB" && node->children[3]->name == "RB") {
        // Exp: Exp LB Exp RB
        ASTNode *Exp1 = node->children[0];
        ASTNode *Exp2 = node->children[2];

//        visit_Exp(Exp1);
//        visit_Exp(Exp2);

        throw runtime_error("not generate code for Exp: Exp LB Exp RB");


    } else if (node->children.size() == 3 && node->children[1]->name == "DOT") {
        // Exp: Exp DOT ID
        ASTNode *Exp = node->children[0];
        ASTNode *ID = node->children[2];

        auto id = any_cast<string>(ID->attributes.at("string_value"));

//        visit_Exp(Exp);

        throw runtime_error("not generate code for Exp: Exp DOT ID");


    } else {
        throw runtime_error("error Exp children");
    }

}

// args_type: vector<optional<shared_ptr<Type>>>
void IntermediateCode::visit_Args(ASTNode *node) {
    if (node->children.size() == 1) {
        // Args: Exp
        ASTNode *Exp = node->children[0];

        string place = createTempSymbol();

        node->attributes["irplace"] = place;
        visit_Exp(Exp);

        auto ir_args = vector<string>();
        ir_args.push_back(place);

        node->attributes["irargs"] = ir_args;
        node->attributes["ircode"] = Exp->attributes.at("ircode");

    } else if (node->children.size() == 3) {
        // Args: Exp COMMA Args
        ASTNode *Exp = node->children[0];
        ASTNode *Args = node->children[2];

        string place = createTempSymbol();

        node->attributes["irplace"] = place;
        visit_Exp(Exp);

        visit_Args(Args);

        auto ir_args = vector<string>();
        ir_args.push_back(place);

        auto args = any_cast<vector<string>>(Args->attributes.at("irargs"));
        ir_args.insert(ir_args.end(), args.begin(), args.end());

        node->attributes["irargs"] = ir_args;

        auto codeA = any_cast<shared_ptr<Code>>(Exp->attributes.at("ircode"));
        auto codeB = any_cast<shared_ptr<Code>>(Args->attributes.at("ircode"));

        auto code = make_shared<Code>(Code({codeA, codeB}));

        node->attributes["ircode"] = code;

    }
}



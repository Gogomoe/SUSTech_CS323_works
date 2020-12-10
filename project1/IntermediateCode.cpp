
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

template<typename ... Args>
std::string string_format(const std::string &format, Args ... args) {
    size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
    if (size <= 0) { throw std::runtime_error("Error during formatting."); }
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

shared_ptr<Code> dropEmpty(const string &target, shared_ptr<Code> code) {
    if (target.empty()) {
        return make_shared<Code>(Code(""));
    } else {
        return code;
    }
}

shared_ptr<Code> dropEmpty(const string &target, Code code) {
    return dropEmpty(target, make_shared<Code>(code));
}

Code::Code(string code) : code(std::move(code)), codes() {}

Code::Code(std::vector<std::shared_ptr<Code>> codes) : code(), codes(std::move(codes)) {}

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

std::string IntermediateCode::createTempSymbol(const std::string &prefix) {
    if (tempSymbols.find(prefix) == tempSymbols.end()) {
        tempSymbols[prefix] = 0;
    }
    string symbol = "t" + string_format("%d", temp_count) + prefix;
    tempSymbols[prefix]++;
    temp_count++;
    return symbol;
}

std::string IntermediateCode::createTempSymbol() {
    return createTempSymbol("");
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

int _debug_ = 0;
int _level_ = 0;

void _debug_start_(ASTNode *node) {
    for (int i = 0; i < _level_; i++) {
        if (_debug_) {
            printf(" ");
        }
    }
    if (_debug_) {
        printf("+%s: ", node->name.data());
    }
    for (auto &_it_:node->children) {
        if (_debug_) {
            printf("%s ", _it_->name.data());
        }
    }
    if (_debug_) {
        printf("\n");
    }

    _level_++;
}

void _debug_end_(ASTNode *node) {
    _level_--;

    for (int i = 0; i < _level_; i++) {
        if (_debug_) {
            printf(" ");
        }
    }
    if (_debug_) {
        printf("-%s: ", node->name.data());
    }
    for (auto &_it_:node->children) {
        if (_debug_) {
            printf("%s ", _it_->name.data());
        }
    }
    if (_debug_) {
        printf("\n");
    }
}

void IntermediateCode::visit_Program(ASTNode *node) {
    //_debug_start_(node);
    ASTNode *ExtDefList = node->children[1];

    visit_ExtDefList(ExtDefList);

    node->attributes["ircode"] = ExtDefList->attributes.at("ircode");
    //_debug_end_(node);
}


// definitions: vector<pair<string, shared_ptr<Type>>>
void IntermediateCode::visit_ExtDefList(ASTNode *node) {
    //_debug_start_(node);
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
    //_debug_end_(node);
}

// definitions: vector<pair<string, shared_ptr<Type>>>
void IntermediateCode::visit_ExtDef(ASTNode *node) {
    //_debug_start_(node);
    if (node->children.size() == 2 && node->children[1]->name == "SEMI") {
        // ExtDef: Specifier SEMI
        ASTNode *Specifier = node->children[0];

//        visit_Specifier(Specifier);

        auto code = make_shared<Code>(Code(""));

        node->attributes["ircode"] = code;

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
    //_debug_end_(node);
}

// definitions: vector<pair<string, shared_ptr<Type>>>
void IntermediateCode::visit_ExtDecList(ASTNode *node) {
    //_debug_start_(node);
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
    //_debug_end_(node);
}

// type: Type*
void IntermediateCode::visit_Specifier(ASTNode *node) {
    //_debug_start_(node);
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
    //_debug_end_(node);
}

// type: Type*
void IntermediateCode::visit_StructSpecifier(ASTNode *node) {
    //_debug_start_(node);
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
    //_debug_end_(node);
}

// declaration: pair<string, shared_ptr<Type>>
void IntermediateCode::visit_VarDec(ASTNode *node) {
    //_debug_start_(node);
    if (node->children.size() == 1) {
        // VarDec: ID
        ASTNode *ID = node->children[0];

        auto declaration = any_cast<pair<string, shared_ptr<Type>>>(node->attributes.at("declaration"));

        string id = declaration.first;
        shared_ptr<Type> type = declaration.second;

        string irname = createSymbol(id);

        if (dynamic_pointer_cast<IntType>(type) != nullptr) {
            node->attributes["irname"] = id;
            node->attributes["ircode"] = make_shared<Code>(Code(""));
        } else {
            auto struct_type = dynamic_pointer_cast<StructType>(type);
            int width = struct_type->width;

            node->attributes["irname"] = id;
            node->attributes["ircode"] = make_shared<Code>(
                    Code("DEC " + irname + " " + std::to_string(width))
            );
        }


    } else if (node->children.size() == 4) {
        // VarDec: VarDec LB INT RB
        ASTNode *VarDec = node->children[0];
        ASTNode *INT = node->children[2];

//        visit_VarDec(VarDec);

        auto declaration = any_cast<pair<string, shared_ptr<Type>>>(node->attributes.at("declaration"));

        string id = declaration.first;
        shared_ptr<Type> type = declaration.second;

        string irname = createSymbol(id);

        int size = any_cast<int>(INT->attributes.at("int_value"));
        int width = type->width;

        node->attributes["irname"] = id;
        node->attributes["ircode"] = make_shared<Code>(
                Code("DEC " + irname + " " + std::to_string(size * width))
        );

    } else {
        throw runtime_error("error VarDec children");
    }
    //_debug_end_(node);
}

// declaration: pair<string, shared_ptr<Type>>
void IntermediateCode::visit_FunDec(ASTNode *node) {
    //_debug_start_(node);
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
    //_debug_end_(node);
}

// parameters: vector<pair<string, shared_ptr<Type>>>
void IntermediateCode::visit_VarList(ASTNode *node) {
    //_debug_start_(node);
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
    //_debug_end_(node);
}

// declaration: pair<string, shared_ptr<Type>>
// type: Type*
void IntermediateCode::visit_ParamDec(ASTNode *node) {
    //_debug_start_(node);
    // ParamDec: Specifier VarDec
    ASTNode *Specifier = node->children[0];
    ASTNode *VarDec = node->children[1];

//    visit_Specifier(Specifier);
//    visit_VarDec(VarDec);

    throw runtime_error("not generate code for ParamDec");
    //_debug_end_(node);
}

// return_type: Type*
void IntermediateCode::visit_CompSt(ASTNode *node) {
    //_debug_start_(node);
    // CompSt: LC DefList StmtList RC
    ASTNode *DefList = node->children[1];
    ASTNode *StmtList = node->children[2];

    visit_DefList(DefList);
    visit_StmtList(StmtList);

    auto codeA = any_cast<shared_ptr<Code>>(DefList->attributes.at("ircode"));
    auto codeB = any_cast<shared_ptr<Code>>(StmtList->attributes.at("ircode"));

    auto code = make_shared<Code>(Code({codeA, codeB}));

    node->attributes["ircode"] = code;
    //_debug_end_(node);
}

// return_type: Type*
void IntermediateCode::visit_StmtList(ASTNode *node) {
    //_debug_start_(node);
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
    //_debug_end_(node);
}


// return_type: Type*
void IntermediateCode::visit_Stmt(ASTNode *node) {
    //_debug_start_(node);
    node->attributes["return_type"] = node->parent->attributes.at("return_type");

    if (node->children.size() == 2 && node->children[0]->name == "Exp") {
        // Stmt: Exp SEMI
        ASTNode *Exp = node->children[0];

        string place;
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

        string place;
        pair<string, string> cond{label_true, label_false};

        node->attributes["irplace"] = place;
        node->attributes["ircond"] = cond;
        visit_Exp(Exp);

        visit_Stmt(Stmt);

        vector<shared_ptr<Code>> codes{
                any_cast<shared_ptr<Code>>(Exp->attributes.at("ircode")),
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

        string place;
        pair<string, string> cond{label_true, label_false};

        node->attributes["irplace"] = place;
        node->attributes["ircond"] = cond;
        visit_Exp(Exp);

        visit_Stmt(Stmt1);
        visit_Stmt(Stmt2);

        vector<shared_ptr<Code>> codes{
                any_cast<shared_ptr<Code>>(Exp->attributes.at("ircode")),
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
        string label_body = createLabel();
        string label_end = createLabel();

        string place;
        pair<string, string> cond{label_body, label_end};

        node->attributes["irplace"] = place;
        node->attributes["ircond"] = cond;
        visit_Exp(Exp);

        loopTable.emplace_back(LoopItem{"", label_loop, label_end});
        visit_Stmt(Stmt);
        loopTable.pop_back();

        vector<shared_ptr<Code>> codes{
                make_shared<Code>(Code("LABEL " + label_loop + " :")),
                any_cast<shared_ptr<Code>>(Exp->attributes.at("ircode")),
                make_shared<Code>(Code("LABEL " + label_body + " :")),
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
        string label_body = createLabel();
        string label_end = createLabel();

        string place;
        pair<string, string> cond{label_body, label_end};

        node->attributes["irplace"] = place;
        node->attributes["ircond"] = cond;
        visit_Exp(Exp);

        loopTable.emplace_back(LoopItem{loop_label, label_loop, label_end});
        visit_Stmt(Stmt);
        loopTable.pop_back();

        vector<shared_ptr<Code>> codes{
                make_shared<Code>(Code("LABEL " + label_loop + " :")),
                any_cast<shared_ptr<Code>>(Exp->attributes.at("ircode")),
                make_shared<Code>(Code("LABEL " + label_body + " :")),
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

        string place1 = createTempSymbol("start");
        string place2 = createTempSymbol("end");
        string place_it = createSymbol(it_name);

        node->attributes["irplace"] = place1;
        visit_Exp(Exp1);

        node->attributes["irplace"] = place2;
        visit_Exp(Exp2);

        string label_loop = createLabel();
        string label_end = createLabel();
        string label_start = createLabel();

        loopTable.emplace_back(LoopItem{"", label_loop, label_end});
        visit_CompSt(CompSt);
        loopTable.pop_back();

        vector<shared_ptr<Code>> codes{
                any_cast<shared_ptr<Code>>(Exp1->attributes.at("ircode")),
                any_cast<shared_ptr<Code>>(Exp2->attributes.at("ircode")),
                make_shared<Code>(Code(place_it + " := " + place1)),
                make_shared<Code>(Code("LABEL " + label_start + " :")),
                make_shared<Code>(Code("IF " + place_it + " > " + place2 + " GOTO " + label_end)),
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

        string place1 = createTempSymbol("start");
        string place2 = createTempSymbol("end");
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
                make_shared<Code>(Code("IF " + place_it + " > " + place2 + " GOTO " + label_end)),
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
    //_debug_end_(node);
}


// definitions: vector<pair<string, shared_ptr<Type>>>
void IntermediateCode::visit_DefList(ASTNode *node) {
    //_debug_start_(node);
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
    //_debug_end_(node);
}

// definitions: vector<pair<string, shared_ptr<Type>>>
// type: Type*
void IntermediateCode::visit_Def(ASTNode *node) {
    //_debug_start_(node);
    // Def: Specifier DecList SEMI
    ASTNode *Specifier = node->children[0];
    ASTNode *DecList = node->children[1];

//    visit_Specifier(Specifier);

    visit_DecList(DecList);

    node->attributes["ircode"] = DecList->attributes.at("ircode");
    //_debug_end_(node);
}

// definitions: vector<pair<string, shared_ptr<Type>>>
// type: Type*
void IntermediateCode::visit_DecList(ASTNode *node) {
    //_debug_start_(node);
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
    //_debug_end_(node);
}

// declaration: pair<string, shared_ptr<Type>>
// type: Type*
void IntermediateCode::visit_Dec(ASTNode *node) {
    //_debug_start_(node);
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
    //_debug_end_(node);
}

string mapArithmeticOperation(const string &OP) {
    if (OP == "PLUS") {
        return "+";
    } else if (OP == "MINUS") {
        return "-";
    } else if (OP == "MUL") {
        return "*";
    } else if (OP == "DIV") {
        return "/";
    } else {
        throw runtime_error("unexpected arithmetic operation " + OP);
    }
}

string mapRelationOperation(const string &OP) {
    if (OP == "LT") {
        return "<";
    } else if (OP == "LE") {
        return "<=";
    } else if (OP == "GT") {
        return ">";
    } else if (OP == "GE") {
        return ">=";
    } else if (OP == "NE") {
        return "!=";
    } else if (OP == "EQ") {
        return "==";
    } else {
        throw runtime_error("unexpected relation operation " + OP);
    }
}

// type: optional<shared_ptr<Type>>
// lvalue: bool
void IntermediateCode::visit_Exp(ASTNode *node) {
    //_debug_start_(node);
    auto target = any_cast<string>(node->parent->attributes.at("irplace"));

    if (node->children.size() == 1 && node->children[0]->name == "ID") {
        // Exp: ID
        ASTNode *ID = node->children[0];

        auto id = any_cast<string>(ID->attributes.at("string_value"));
        auto type = any_cast<optional<shared_ptr<Type>>>(node->attributes.at("type"));

        node->attributes["irname"] = id;

        vector<shared_ptr<Code>> codes{};

        if (target.empty() && node->parent->attributes.find("ircond") != node->parent->attributes.end()) {
            target = createTempSymbol();
        }

        if (dynamic_pointer_cast<IntType>(type.value()) != nullptr) {
            codes.push_back(dropEmpty(target, Code(target + " := " + symbolTable.at(id))));
        } else {
            string iraddr = symbolTable.at(id);
            codes.push_back(dropEmpty(target, Code(target + " := *" + iraddr)));
            node->attributes["iraddr"] = iraddr;
        }

        if (node->parent->attributes.find("ircond") != node->parent->attributes.end()) {
            auto cond = any_cast<pair<string, string>>(node->parent->attributes.at("ircond"));
            codes.push_back(make_shared<Code>(Code("IF " + target + " != #0 GOTO " + cond.first)));
            codes.push_back(make_shared<Code>(Code("GOTO " + cond.second)));
        }

        node->attributes["ircode"] = make_shared<Code>(Code(codes));

    } else if (node->children.size() == 1 && node->children[0]->name == "INT") {
        // Exp: INT
        ASTNode *INT = node->children[0];

        auto value = any_cast<int>(INT->attributes.at("int_value"));

        vector<shared_ptr<Code>> codes{};

        if (target.empty() && node->parent->attributes.find("ircond") != node->parent->attributes.end()) {
            target = createTempSymbol();
        }

        codes.push_back(dropEmpty(target, Code(target + " := #" + std::to_string(value))));

        if (node->parent->attributes.find("ircond") != node->parent->attributes.end()) {
            auto cond = any_cast<pair<string, string>>(node->parent->attributes.at("ircond"));
            codes.push_back(make_shared<Code>(Code("IF " + target + " != #0 GOTO " + cond.first)));
            codes.push_back(make_shared<Code>(Code("GOTO " + cond.second)));
        }

        node->attributes["ircode"] = make_shared<Code>(Code(codes));

    } else if (node->children.size() == 1 && node->children[0]->name == "FLOAT") {
        // Exp: FLOAT

        throw runtime_error("not generate code for Exp: FLOAT");

    } else if (node->children.size() == 1 && node->children[0]->name == "CHAR") {
        // Exp: CHAR

        throw runtime_error("not generate code for Exp: CHAR");

    } else if (node->children.size() == 1 && node->children[0]->name == "String") {
        // Exp: String

        throw runtime_error("not generate code for Exp: String");

    } else if (node->children.size() == 3 && node->children[1]->name == "ASSIGN") {
        // Exp: Exp ASSIGN Exp

        ASTNode *Exp1 = node->children[0];
        ASTNode *OP = node->children[1];
        ASTNode *Exp2 = node->children[2];

        string place1 = createTempSymbol();
        string place2 = createTempSymbol();

        node->attributes["irplace"] = place1;
        visit_Exp(Exp1);

        node->attributes["irplace"] = place2;
        visit_Exp(Exp2);

        if (target.empty() && node->parent->attributes.find("ircond") != node->parent->attributes.end()) {
            target = createTempSymbol();
        }

        if (Exp1->attributes.find("irname") != Exp1->attributes.end()) {
            auto id = any_cast<string>(Exp1->attributes.at("irname"));

            vector<shared_ptr<Code>> codes{
                    any_cast<shared_ptr<Code>>(Exp2->attributes.at("ircode")),
                    make_shared<Code>(Code(symbolTable.at(id) + " := " + place2)),
                    dropEmpty(target, Code(target + " := " + place2))
            };

            if (node->parent->attributes.find("ircond") != node->parent->attributes.end()) {
                auto cond = any_cast<pair<string, string>>(node->parent->attributes.at("ircond"));
                codes.push_back(make_shared<Code>(Code("IF " + target + " != #0 GOTO " + cond.first)));
                codes.push_back(make_shared<Code>(Code("GOTO " + cond.second)));
            }

            auto code = make_shared<Code>(Code(codes));

            node->attributes["ircode"] = code;

        } else {
            auto iraddr = any_cast<string>(Exp1->attributes.at("iraddr"));

            vector<shared_ptr<Code>> codes{
                    any_cast<shared_ptr<Code>>(Exp1->attributes.at("ircode")),
                    any_cast<shared_ptr<Code>>(Exp2->attributes.at("ircode")),
                    make_shared<Code>(Code("*" + iraddr + " := " + place2)),
                    dropEmpty(target, Code(target + " := " + place2))
            };

            if (node->parent->attributes.find("ircond") != node->parent->attributes.end()) {
                auto cond = any_cast<pair<string, string>>(node->parent->attributes.at("ircond"));
                codes.push_back(make_shared<Code>(Code("IF " + target + " != #0 GOTO " + cond.first)));
                codes.push_back(make_shared<Code>(Code("GOTO " + cond.second)));
            }

            auto code = make_shared<Code>(Code(codes));

            node->attributes["ircode"] = code;

        }
    } else if (node->children.size() == 3 && (node->children[1]->name == "AND" || node->children[1]->name == "OR")) {
        // Exp: Exp AND Exp
        // Exp: Exp OR Exp
        ASTNode *Exp1 = node->children[0];
        ASTNode *OP = node->children[1];
        ASTNode *Exp2 = node->children[2];

        if (node->parent->attributes.find("ircond") != node->parent->attributes.end()) {
            auto cond = any_cast<pair<string, string>>(node->parent->attributes.at("ircond"));

            string place = createTempSymbol();
            string next = createLabel();

            pair<string, string> cond1;
            pair<string, string> cond2;

            if (OP->name == "AND") {
                cond1 = {next, cond.second};
                cond2 = {cond.first, cond.second};
            } else if (OP->name == "OR") {
                cond1 = {cond.first, next};
                cond2 = {cond.first, cond.second};
            }

            node->attributes["irplace"] = place;
            node->attributes["ircond"] = cond1;
            visit_Exp(Exp1);
            node->attributes["irplace"] = place;
            node->attributes["ircond"] = cond2;
            visit_Exp(Exp2);

            vector<shared_ptr<Code>> codes{
                    any_cast<shared_ptr<Code>>(Exp1->attributes.at("ircode")),
                    make_shared<Code>(Code("LABEL " + next + " :")),
                    any_cast<shared_ptr<Code>>(Exp2->attributes.at("ircode"))
            };

            auto code = make_shared<Code>(Code(codes));

            node->attributes["ircode"] = code;

        } else {
            string place1 = createTempSymbol();
            string place2 = target.empty() ? "" : createTempSymbol();

            node->attributes["irplace"] = place1;
            visit_Exp(Exp1);

            node->attributes["irplace"] = place2;
            visit_Exp(Exp2);

            string quick_judge;

            string quick_res;
            string slow_res;
            if (OP->name == "AND") {
                quick_judge = " == #0 GOTO ";
                quick_res = " := #0";
                slow_res = " := #1";
            } else if (OP->name == "OR") {
                quick_judge = " != #0 GOTO ";
                quick_res = " := #1";
                slow_res = " := #0";
            }

            string label_quick = createLabel();
            string label_end = createLabel();

            vector<shared_ptr<Code>> codes{
                    any_cast<shared_ptr<Code>>(Exp1->attributes.at("ircode")),
                    make_shared<Code>(Code("IF " + place1 + quick_judge + label_quick)),
                    any_cast<shared_ptr<Code>>(Exp2->attributes.at("ircode")),
                    make_shared<Code>(Code("IF " + place2 + quick_judge + label_quick)),
                    make_shared<Code>(Code(target + slow_res)),
                    make_shared<Code>(Code("GOTO " + label_end)),
                    make_shared<Code>(Code("LABEL " + label_quick + " :")),
                    make_shared<Code>(Code(target + quick_res)),
                    make_shared<Code>(Code("LABEL " + label_end + " :"))
            };

            if (target.empty()) {
                codes = {
                        any_cast<shared_ptr<Code>>(Exp1->attributes.at("ircode")),
                        make_shared<Code>(Code("IF " + place1 + quick_judge + label_end)),
                        any_cast<shared_ptr<Code>>(Exp2->attributes.at("ircode")),
                        make_shared<Code>(Code("LABEL " + label_end + " :"))
                };
            }

            auto code = make_shared<Code>(Code(codes));

            node->attributes["ircode"] = code;
        }

    } else if (node->children.size() == 3 && node->children[0]->name == "Exp" && node->children[2]->name == "Exp") {
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
        if (node->parent->attributes.find("ircond") != node->parent->attributes.end()) {
            auto cond = any_cast<pair<string, string>>(node->parent->attributes.at("ircond"));

            string place1 = createTempSymbol();
            string place2 = createTempSymbol();

            if (OP->name == "LT" || OP->name == "LE" || OP->name == "GT" || OP->name == "GE" ||
                OP->name == "NE" || OP->name == "EQ") {

                node->attributes["irplace"] = place1;
                visit_Exp(Exp1);

                node->attributes["irplace"] = place2;
                visit_Exp(Exp2);

                string op = mapRelationOperation(OP->name);

                vector<shared_ptr<Code>> codes{
                        any_cast<shared_ptr<Code>>(Exp1->attributes.at("ircode")),
                        any_cast<shared_ptr<Code>>(Exp2->attributes.at("ircode")),
                        make_shared<Code>(Code("IF " + place1 + " " + op + " " + place2 + " GOTO " + cond.first)),
                        make_shared<Code>(Code("GOTO " + cond.second))
                };

                auto code = make_shared<Code>(Code(codes));

                node->attributes["ircode"] = code;

            } else if (OP->name == "PLUS" || OP->name == "MINUS" || OP->name == "MUL" || OP->name == "DIV") {

                string op = mapArithmeticOperation(OP->name);

                if (target.empty()) {
                    target = createTempSymbol();
                }

                vector<shared_ptr<Code>> codes{
                        any_cast<shared_ptr<Code>>(Exp1->attributes.at("ircode")),
                        any_cast<shared_ptr<Code>>(Exp2->attributes.at("ircode")),
                        make_shared<Code>(Code(target + " := " + place1 + " " + op + " " + place2)),
                        make_shared<Code>(Code("IF " + target + " != #0 GOTO " + cond.first)),
                        make_shared<Code>(Code("GOTO " + cond.second))
                };

                auto code = make_shared<Code>(Code(codes));

                node->attributes["ircode"] = code;
            }

        } else {
            string place1;
            string place2;

            if (!target.empty()) {
                place1 = createTempSymbol();
                place2 = createTempSymbol();
            }

            node->attributes["irplace"] = place1;
            visit_Exp(Exp1);

            node->attributes["irplace"] = place2;
            visit_Exp(Exp2);

            if (OP->name == "LT" || OP->name == "LE" || OP->name == "GT" || OP->name == "GE" ||
                OP->name == "NE" || OP->name == "EQ") {
                string label_true = createLabel();
                string label_end = createLabel();

                string op = mapRelationOperation(OP->name);

                vector<shared_ptr<Code>> codes{
                        any_cast<shared_ptr<Code>>(Exp1->attributes.at("ircode")),
                        any_cast<shared_ptr<Code>>(Exp2->attributes.at("ircode")),
                        make_shared<Code>(Code("IF " + place1 + " " + op + " " + place2 + " GOTO " + label_true)),
                        make_shared<Code>(Code(target + " := #0")),
                        make_shared<Code>(Code("GOTO " + label_end)),
                        make_shared<Code>(Code("LABEL " + label_true + " :")),
                        make_shared<Code>(Code(target + " := #1")),
                        make_shared<Code>(Code("LABEL " + label_end + " :"))
                };

                if (target.empty()) {
                    codes = {
                            any_cast<shared_ptr<Code>>(Exp1->attributes.at("ircode")),
                            any_cast<shared_ptr<Code>>(Exp2->attributes.at("ircode"))
                    };
                }

                auto code = make_shared<Code>(Code(codes));

                node->attributes["ircode"] = code;
            } else if (OP->name == "PLUS" || OP->name == "MINUS" || OP->name == "MUL" || OP->name == "DIV") {

                string op = mapArithmeticOperation(OP->name);

                vector<shared_ptr<Code>> codes{
                        any_cast<shared_ptr<Code>>(Exp1->attributes.at("ircode")),
                        any_cast<shared_ptr<Code>>(Exp2->attributes.at("ircode")),
                        dropEmpty(target, Code(target + " := " + place1 + " " + op + " " + place2))
                };

                auto code = make_shared<Code>(Code(codes));

                node->attributes["ircode"] = code;
            }
        }
    } else if (node->children.size() == 3 && node->children[0]->name == "LP" && node->children[2]->name == "RP") {
        // Exp: LP Exp RP
        ASTNode *Exp = node->children[1];

        if (node->parent->attributes.find("ircond") != node->parent->attributes.end()) {
            auto cond = any_cast<pair<string, string>>(node->parent->attributes.at("ircond"));
            node->attributes["ircond"] = cond;
        }

        node->attributes["irplace"] = target;
        visit_Exp(Exp);

        node->attributes["ircode"] = Exp->attributes.at("ircode");

    } else if (node->children.size() == 2 && node->children[1]->name == "Exp") {
        // Exp: MINUS Exp
        // Exp: NOT Exp
        ASTNode *OP = node->children[0];
        ASTNode *Exp = node->children[1];

        if (node->parent->attributes.find("ircond") != node->parent->attributes.end()) {
            auto cond = any_cast<pair<string, string>>(node->parent->attributes.at("ircond"));
            string place;

            if (OP->name == "MINUS") {
                node->attributes["ircond"] = cond;
                node->attributes["irplace"] = place;
            } else if (OP->name == "NOT") {
                pair<string, string> not_cond{cond.second, cond.first};

                node->attributes["ircond"] = not_cond;
                node->attributes["irplace"] = place;
            }

            visit_Exp(Exp);

            auto code = any_cast<shared_ptr<Code>>(Exp->attributes.at("ircode"));

            node->attributes["ircode"] = code;

        } else {
            string place;
            if (!target.empty()) {
                place = createTempSymbol();
            }

            node->attributes["irplace"] = place;
            visit_Exp(Exp);

            if (OP->name == "MINUS") {
                vector<shared_ptr<Code>> codes{
                        any_cast<shared_ptr<Code>>(Exp->attributes.at("ircode")),
                        dropEmpty(target, Code(target + " := #0 - " + place)),
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

                if (target.empty()) {
                    codes = {
                            any_cast<shared_ptr<Code>>(Exp->attributes.at("ircode"))
                    };
                }

                auto code = make_shared<Code>(Code(codes));

                node->attributes["ircode"] = code;
            }
        }
    } else if (node->children.size() == 3 && node->children[1]->name == "LP" && node->children[2]->name == "RP") {
        // Exp: ID LP RP
        ASTNode *ID = node->children[0];

        if (target.empty()) {
            target = createTempSymbol();
        }

        auto id = any_cast<string>(ID->attributes.at("string_value"));

        vector<shared_ptr<Code>> codes{};

        if (id == "read") {
            codes.push_back(make_shared<Code>(Code("READ " + target)));
        } else {
            codes.push_back(make_shared<Code>(Code(target + " := CALL " + id)));
        }

        if (node->parent->attributes.find("ircond") != node->parent->attributes.end()) {
            auto cond = any_cast<pair<string, string>>(node->parent->attributes.at("ircond"));
            codes.push_back(make_shared<Code>(Code("IF " + target + " != #0 GOTO " + cond.first)));
            codes.push_back(make_shared<Code>(Code("GOTO " + cond.second)));
        }

        shared_ptr<Code> code = make_shared<Code>(Code(codes));

        node->attributes["ircode"] = code;

    } else if (node->children.size() == 4 && node->children[1]->name == "LP" && node->children[3]->name == "RP") {
        // Exp: ID LP Args RP
        ASTNode *ID = node->children[0];
        ASTNode *Args = node->children[2];

        if (target.empty()) {
            target = createTempSymbol();
        }

        auto id = any_cast<string>(ID->attributes.at("string_value"));

        visit_Args(Args);

        auto args = any_cast<vector<string>>(Args->attributes.at("irargs"));

        vector<shared_ptr<Code>> codes{};

        if (id == "write") {
            codes = {
                    any_cast<shared_ptr<Code>>(Args->attributes.at("ircode")),
                    make_shared<Code>(Code("WRITE " + args[0]))
            };
            if (node->parent->attributes.find("ircond") != node->parent->attributes.end()) {
                make_shared<Code>(Code(target + " := #0"));
            }
        } else {
            codes.push_back(any_cast<shared_ptr<Code>>(Args->attributes.at("ircode")));

            for (auto it = args.rbegin(); it != args.rend(); it++) {
                codes.push_back(make_shared<Code>(Code("ARG " + *it)));
            }

            codes.push_back(make_shared<Code>(Code(target + " := CALL " + id)));
        }

        if (node->parent->attributes.find("ircond") != node->parent->attributes.end()) {
            auto cond = any_cast<pair<string, string>>(node->parent->attributes.at("ircond"));
            codes.push_back(make_shared<Code>(Code("IF " + target + " != #0 GOTO " + cond.first)));
            codes.push_back(make_shared<Code>(Code("GOTO " + cond.second)));
        }

        auto code = make_shared<Code>(codes);

        node->attributes["ircode"] = code;

    } else if (node->children.size() == 4 && node->children[1]->name == "LB" && node->children[3]->name == "RB") {
        // Exp: Exp LB Exp RB
        ASTNode *Exp1 = node->children[0];
        ASTNode *Exp2 = node->children[2];

        string place;
        string index = createTempSymbol("index");

        node->attributes["irplace"] = place;
        visit_Exp(Exp1);
        node->attributes["irplace"] = index;
        visit_Exp(Exp2);


        auto type = any_cast<optional<shared_ptr<Type>>>(Exp1->attributes.at("type"));
        auto array_type = dynamic_pointer_cast<ArrayType>(type.value());
        auto inner_width = array_type->type->width;
        auto start_addr = any_cast<string>(Exp1->attributes.at("iraddr"));

        string offset = createTempSymbol("offset");
        string iraddr = createTempSymbol("addr");

        if (target.empty() && node->parent->attributes.find("ircond") != node->parent->attributes.end()) {
            target = createTempSymbol();
        }

        vector<shared_ptr<Code>> codes{
                any_cast<shared_ptr<Code>>(Exp1->attributes.at("ircode")),
                any_cast<shared_ptr<Code>>(Exp2->attributes.at("ircode")),
                make_shared<Code>(Code(offset + " := " + index + " * #" + std::to_string(inner_width))),
                make_shared<Code>(Code(iraddr + " := " + start_addr + " + " + offset)),
                dropEmpty(target, Code(target + " := *" + iraddr))
        };

        if (node->parent->attributes.find("ircond") != node->parent->attributes.end()) {
            auto cond = any_cast<pair<string, string>>(node->parent->attributes.at("ircond"));
            codes.push_back(make_shared<Code>(Code("IF " + target + " != #0 GOTO " + cond.first)));
            codes.push_back(make_shared<Code>(Code("GOTO " + cond.second)));
        }

        node->attributes["ircode"] = make_shared<Code>(Code(codes));
        node->attributes["iraddr"] = iraddr;


    } else if (node->children.size() == 3 && node->children[1]->name == "DOT") {
        // Exp: Exp DOT ID
        ASTNode *Exp = node->children[0];
        ASTNode *ID = node->children[2];

        auto id = any_cast<string>(ID->attributes.at("string_value"));

        string place;

        node->attributes["irplace"] = place;
        visit_Exp(Exp);

        auto type = any_cast<optional<shared_ptr<Type>>>(Exp->attributes.at("type"));
        auto struct_type = dynamic_pointer_cast<StructType>(type.value());
        auto start_addr = any_cast<string>(Exp->attributes.at("iraddr"));

        string iraddr = createTempSymbol("addr");

        int offset = 0;
        for (auto &it:struct_type->fields) {
            if (it.first == id) {
                break;
            }
            offset += it.second->width;
        }

        if (target.empty() && node->parent->attributes.find("ircond") != node->parent->attributes.end()) {
            target = createTempSymbol();
        }

        vector<shared_ptr<Code>> codes{
                any_cast<shared_ptr<Code>>(Exp->attributes.at("ircode")),
                make_shared<Code>(Code(iraddr + " := " + start_addr + " + #" + std::to_string(offset))),
                dropEmpty(target, Code(target + " := *" + iraddr))
        };

        if (node->parent->attributes.find("ircond") != node->parent->attributes.end()) {
            auto cond = any_cast<pair<string, string>>(node->parent->attributes.at("ircond"));
            codes.push_back(make_shared<Code>(Code("IF " + target + " != #0 GOTO " + cond.first)));
            codes.push_back(make_shared<Code>(Code("GOTO " + cond.second)));
        }

        node->attributes["ircode"] = make_shared<Code>(Code(codes));
        node->attributes["iraddr"] = iraddr;

    } else {
        throw runtime_error("error Exp children");
    }
    //_debug_end_(node);
}

// args_type: vector<optional<shared_ptr<Type>>>
void IntermediateCode::visit_Args(ASTNode *node) {
    //_debug_start_(node);
    if (node->children.size() == 1) {
        // Args: Exp
        ASTNode *Exp = node->children[0];

        string place = createTempSymbol("arg");

        node->attributes["irplace"] = place;
        visit_Exp(Exp);

        auto ir_args = vector<string>();

        auto type = any_cast<optional<shared_ptr<Type>>>(Exp->attributes.at("type"));

        if (dynamic_pointer_cast<IntType>(type.value()) != nullptr) {
            ir_args.push_back(place);
        } else {
            auto iraddr = any_cast<string>(Exp->attributes.at("iraddr"));
            ir_args.push_back(iraddr);
        }

        node->attributes["irargs"] = ir_args;
        node->attributes["ircode"] = Exp->attributes.at("ircode");

    } else if (node->children.size() == 3) {
        // Args: Exp COMMA Args
        ASTNode *Exp = node->children[0];
        ASTNode *Args = node->children[2];

        string place = createTempSymbol("arg");

        node->attributes["irplace"] = place;
        visit_Exp(Exp);

        visit_Args(Args);

        auto ir_args = vector<string>();

        auto type = any_cast<optional<shared_ptr<Type>>>(Exp->attributes.at("type"));

        if (dynamic_pointer_cast<IntType>(type.value()) != nullptr) {
            ir_args.push_back(place);
        } else {
            auto iraddr = any_cast<string>(Exp->attributes.at("iraddr"));
            ir_args.push_back(iraddr);
        }

        auto args = any_cast<vector<string>>(Args->attributes.at("irargs"));
        ir_args.insert(ir_args.end(), args.begin(), args.end());

        node->attributes["irargs"] = ir_args;

        auto codeA = any_cast<shared_ptr<Code>>(Exp->attributes.at("ircode"));
        auto codeB = any_cast<shared_ptr<Code>>(Args->attributes.at("ircode"));

        auto code = make_shared<Code>(Code({codeA, codeB}));

        node->attributes["ircode"] = code;

    }
    //_debug_end_(node);
}



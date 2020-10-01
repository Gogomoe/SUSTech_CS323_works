%locations
%{
    #include "ASTNode.h"
    #include "lex.yy.c"
    void yyerror(const char*);
    struct ASTNode *program;
%}
%define api.value.type {struct ASTNode *}

%token INT
%token FLOAT
%token CHAR
%token ID
%token TYPE
%token STRUCT
%token IF ELSE
%token WHILE
%token RETURN
%token DOT SEMI COMMA
%token ASSIGN
%token LT LE GT GE NE EQ
%token PLUS MINUS MUL DIV AND OR NOT
%token LP RP LB RB LC RC

%right ASSIGN
%left OR
%left AND
%left GT GE LT LE EQ NE
%left PLUS MINUS
%left MUL DIV
%right NOT
%left LC RC LB RB DOT

%nonassoc LOWER_ELSE
%nonassoc ELSE

%%

Program: 
      ExtDefList                { $$ = make_internal_node1("Program", $1); program = $$; }
    ; 
ExtDefList: 
      ExtDef ExtDefList         { $$ = make_internal_node2("ExtDefList", $1, $2); }
    | %empty                    { $$ = make_internal_node0("ExtDefList"); }
    ;
ExtDef: 
      Specifier ExtDecList SEMI { $$ = make_internal_node3("ExtDef", $1, $2, $3); }
    | Specifier SEMI            { $$ = make_internal_node2("ExtDef", $1, $2); }
    | Specifier FunDec CompSt   { $$ = make_internal_node3("ExtDef", $1, $2, $3); }
    ;
ExtDecList: 
      VarDec                    { $$ = make_internal_node1("ExtDecList", $1); }
    | VarDec COMMA ExtDecList   { $$ = make_internal_node3("ExtDecList", $1, $2, $3); }
    ;

Specifier: 
      TYPE                      { $$ = make_internal_node1("Specifier", $1); }
    | StructSpecifier           { $$ = make_internal_node1("Specifier", $1); }
    ;
StructSpecifier: 
      STRUCT ID LC DefList RC   { $$ = make_internal_node5("StructSpecifier", $1, $2, $3, $4, $5); }
    | STRUCT ID                 { $$ = make_internal_node2("StructSpecifier", $1, $2); }
    ;

VarDec:
      ID                        { $$ = make_internal_node1("VarDec", $1); }
    | VarDec LB INT RB          { $$ = make_internal_node4("VarDec", $1, $2, $3, $4); }
    ;
FunDec: 
      ID LP VarList RP          { $$ = make_internal_node4("FunDec", $1, $2, $3, $4); }
    | ID LP RP                  { $$ = make_internal_node3("FunDec", $1, $2, $3); }
    ;
VarList: 
      ParamDec COMMA VarList    { $$ = make_internal_node3("VarList", $1, $2, $3); }
    | ParamDec                  { $$ = make_internal_node1("VarList", $1); }
    ;
ParamDec: 
      Specifier VarDec          { $$ = make_internal_node2("ParamDec", $1, $2); }
    ;

CompSt: 
      LC DefList StmtList RC    { $$ = make_internal_node4("CompSt", $1, $2, $3, $4); }
    ;
StmtList: 
      Stmt StmtList             { $$ = make_internal_node2("StmtList", $1, $2); }
    | %empty                    { $$ = make_internal_node0("StmtList"); }
    ;
Stmt: 
      Exp SEMI                  { $$ = make_internal_node2("Stmt", $1, $2); }
    | CompSt                    { $$ = make_internal_node1("Stmt", $1); }
    | RETURN Exp SEMI           { $$ = make_internal_node3("Stmt", $1, $2, $3); }
    | IF LP Exp RP Stmt %prec LOWER_ELSE    { $$ = make_internal_node5("Stmt", $1, $2, $3, $4, $5); }
    | IF LP Exp RP Stmt ELSE Stmt           { $$ = make_internal_node7("Stmt", $1, $2, $3, $4, $5, $6, $7); }
    | WHILE LP Exp RP Stmt      { $$ = make_internal_node5("Stmt", $1, $2, $3, $4, $5); }
    ;

DefList: 
      Def DefList               { $$ = make_internal_node2("DefList", $1, $2); }
    | %empty                    { $$ = make_internal_node0("DefList"); }
    ;
Def: 
      Specifier DecList SEMI    { $$ = make_internal_node3("Def", $1, $2, $3); }
    ;
DecList: 
      Dec                       { $$ = make_internal_node1("DecList", $1); }
    | Dec COMMA DecList         { $$ = make_internal_node3("DecList", $1, $2, $3); }
    ;
Dec: 
      VarDec                    { $$ = make_internal_node1("Dec", $1); }
    | VarDec ASSIGN Exp         { $$ = make_internal_node3("Dec", $1, $2, $3); }
    ;

Exp: 
      Exp ASSIGN Exp            { $$ = make_internal_node3("Exp", $1, $2, $3); }
    | Exp AND Exp               { $$ = make_internal_node3("Exp", $1, $2, $3); }
    | Exp OR Exp                { $$ = make_internal_node3("Exp", $1, $2, $3); }
    | Exp LT Exp                { $$ = make_internal_node3("Exp", $1, $2, $3); }
    | Exp LE Exp                { $$ = make_internal_node3("Exp", $1, $2, $3); }
    | Exp GT Exp                { $$ = make_internal_node3("Exp", $1, $2, $3); }
    | Exp GE Exp                { $$ = make_internal_node3("Exp", $1, $2, $3); }
    | Exp NE Exp                { $$ = make_internal_node3("Exp", $1, $2, $3); }
    | Exp EQ Exp                { $$ = make_internal_node3("Exp", $1, $2, $3); }
    | Exp PLUS Exp              { $$ = make_internal_node3("Exp", $1, $2, $3); }
    | Exp MINUS Exp             { $$ = make_internal_node3("Exp", $1, $2, $3); }
    | Exp MUL Exp               { $$ = make_internal_node3("Exp", $1, $2, $3); }
    | Exp DIV Exp               { $$ = make_internal_node3("Exp", $1, $2, $3); }
    | LP Exp RP                 { $$ = make_internal_node3("Exp", $1, $2, $3); }
    | MINUS Exp                 { $$ = make_internal_node2("Exp", $1, $2); }
    | NOT Exp                   { $$ = make_internal_node2("Exp", $1, $2); }
    | ID LP Args RP             { $$ = make_internal_node4("Exp", $1, $2, $3, $4); }
    | ID LP RP                  { $$ = make_internal_node3("Exp", $1, $2, $3); }
    | Exp LB Exp RB             { $$ = make_internal_node4("Exp", $1, $2, $3, $4); }
    | Exp DOT ID                { $$ = make_internal_node3("Exp", $1, $2, $3); }
    | ID                        { $$ = make_internal_node1("Exp", $1); }
    | INT                       { $$ = make_internal_node1("Exp", $1); }
    | FLOAT                     { $$ = make_internal_node1("Exp", $1); }
    | CHAR                      { $$ = make_internal_node1("Exp", $1); }
    ;
Args: 
      Exp COMMA Args            { $$ = make_internal_node3("Args", $1, $2, $3); }
    | Exp                       { $$ = make_internal_node1("Args", $1); }
    ;

%%

void yyerror(const char *s){
    fprintf(stderr, "Syntax error: %s\n", s);
}

int main(int argc, char **argv){
    char *file_path;
    if(argc < 2){
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return 1;
    }
    else if(argc == 2){
        file_path = argv[1];
        if(!(yyin = fopen(file_path, "r"))){
            perror(argv[1]);
            return 1;
        }
        // yydebug = 1;
        yyparse();
        print_ASTTree(program);
        return 0;
    }
    else{
        fputs("Too much arguments!\n", stderr);
        return 1;
    }
}
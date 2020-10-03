%locations
%{
    #include "ASTNode.h"
    #include "lex.yy.c"
    void yyerror(const char *s);
    struct ASTNode *program;
%}
%define api.value.type {struct ASTNode *}

%initial-action
{
  @$.first_line = 1;
  @$.first_column = 1;
  @$.last_line = 1;
  @$.last_column = 1;
};

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

%token STRING STRING_BEGIN STRING_INTERNAL STRING_END
%token BREAK CONTINUE FOR IN
%token COLON DOUBLE_DOT

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
      ExtDefList                { $$ = make_internal_node1("Program", @$, $1); program = $$; }
    ; 
ExtDefList: 
      ExtDef ExtDefList         { $$ = make_internal_node2("ExtDefList", @$, $1, $2); }
    | %empty                    { $$ = make_internal_node0("ExtDefList", @$); }
    ;
ExtDef: 
      Specifier ExtDecList SEMI { $$ = make_internal_node3("ExtDef", @$, $1, $2, $3); }
    | Specifier SEMI            { $$ = make_internal_node2("ExtDef", @$, $1, $2); }
    | Specifier FunDec CompSt   { $$ = make_internal_node3("ExtDef", @$, $1, $2, $3); }

    | Specifier ExtDecList error{ printf("Error type B at Line %d: Missing semicolon ';'\n", @2.last_line); }
    | Specifier error           { printf("Error type B at Line %d: Missing semicolon ';'\n", @1.last_line); }
    | error ExtDecList SEMI     { printf("Error type B at Line %d: Missing specifier\n", @1.first_line); }
    | error SEMI                { printf("Error type B at Line %d: Missing specifier\n", @1.first_line); }
    ;
ExtDecList: 
      VarDec                    { $$ = make_internal_node1("ExtDecList", @$, $1); }
    | VarDec COMMA ExtDecList   { $$ = make_internal_node3("ExtDecList", @$, $1, $2, $3); }
    ;

Specifier: 
      TYPE                      { $$ = make_internal_node1("Specifier", @$, $1); }
    | StructSpecifier           { $$ = make_internal_node1("Specifier", @$, $1); }
    ;
StructSpecifier: 
      STRUCT ID LC DefList RC   { $$ = make_internal_node5("StructSpecifier", @$, $1, $2, $3, $4, $5); }
    | STRUCT ID                 { $$ = make_internal_node2("StructSpecifier", @$, $1, $2); }
    ;

VarDec:
      ID                        { $$ = make_internal_node1("VarDec", @$, $1); }
    | VarDec LB INT RB          { $$ = make_internal_node4("VarDec", @$, $1, $2, $3, $4); }
    ;
FunDec: 
      ID LP VarList RP          { $$ = make_internal_node4("FunDec", @$, $1, $2, $3, $4); }
    | ID LP RP                  { $$ = make_internal_node3("FunDec", @$, $1, $2, $3); }

    | ID LP VarList error       { printf("Error type B at Line %d: Missing closing parenthesis ')'\n", @3.last_line); }
    | ID LP error               { printf("Error type B at Line %d: Missing closing parenthesis ')'\n", @2.last_line); }
    ;
VarList: 
      ParamDec COMMA VarList    { $$ = make_internal_node3("VarList", @$, $1, $2, $3); }
    | ParamDec                  { $$ = make_internal_node1("VarList", @$, $1); }
    ;
ParamDec: 
      Specifier VarDec          { $$ = make_internal_node2("ParamDec", @$, $1, $2); }
    ;

CompSt: 
      LC DefList StmtList RC    { $$ = make_internal_node4("CompSt", @$, $1, $2, $3, $4); }
    ;
StmtList: 
      Stmt StmtList             { $$ = make_internal_node2("StmtList", @$, $1, $2); }
    | %empty                    { $$ = make_internal_node0("StmtList", @$); }
    ;
Stmt: 
      Exp SEMI                  { $$ = make_internal_node2("Stmt", @$, $1, $2); }
    | CompSt                    { $$ = make_internal_node1("Stmt", @$, $1); }
    | RETURN Exp SEMI           { $$ = make_internal_node3("Stmt", @$, $1, $2, $3); }
    | IF LP Exp RP Stmt %prec LOWER_ELSE    { $$ = make_internal_node5("Stmt", @$, $1, $2, $3, $4, $5); }
    | IF LP Exp RP Stmt ELSE Stmt           { $$ = make_internal_node7("Stmt", @$, $1, $2, $3, $4, $5, $6, $7); }
    
    | WHILE LP Exp RP Stmt                  { $$ = make_internal_node5("Stmt", @$, $1, $2, $3, $4, $5); }
    | ID COLON WHILE LP Exp RP Stmt         { $$ = make_internal_node7("Stmt", @$, $1, $2, $3, $4, $5, $6, $7); }
    | FOR ID IN INT DOUBLE_DOT INT Stmt               { $$ = make_internal_node7("Stmt", @$, $1, $2, $3, $4, $5, $6, $7); }
    | ID COLON FOR ID IN INT DOUBLE_DOT INT Stmt      { $$ = make_internal_node9("Stmt", @$, $1, $2, $3, $4, $5, $6, $7, $8, $9); }

    | BREAK SEMI                { $$ = make_internal_node2("Stmt", @$, $1, $2); }
    | BREAK ID SEMI             { $$ = make_internal_node3("Stmt", @$, $1, $2, $3); }
    | CONTINUE SEMI             { $$ = make_internal_node2("Stmt", @$, $1, $2); }
    | CONTINUE ID SEMI          { $$ = make_internal_node3("Stmt", @$, $1, $2, $3); }

    | Exp error                 { printf("Error type B at Line %d: Missing semicolon ';'\n", @1.last_line); }
    | RETURN Exp error          { printf("Error type B at Line %d: Missing semicolon ';'\n", @2.last_line); }
    | IF LP Exp error Stmt %prec LOWER_ELSE { printf("Error type B at Line %d: Missing closing parenthesis ')'\n", @3.last_line); }
    | IF LP Exp error Stmt ELSE Stmt        { printf("Error type B at Line %d: Missing closing parenthesis ')'\n", @3.last_line); }
    ;

DefList: 
      Def DefList               { $$ = make_internal_node2("DefList", @$, $1, $2); }
    | %empty                    { $$ = make_internal_node0("DefList", @$); }
    ;
Def: 
      Specifier DecList SEMI    { $$ = make_internal_node3("Def", @$, $1, $2, $3); }

    | Specifier DecList error   { printf("Error type B at Line %d: Missing semicolon ';'\n", @2.last_line); }
    | error DecList SEMI        { printf("Error type B at Line %d: Missing specifier\n", @1.first_line); }
    ;
DecList: 
      Dec                       { $$ = make_internal_node1("DecList", @$, $1); }
    | Dec COMMA DecList         { $$ = make_internal_node3("DecList", @$, $1, $2, $3); }
    ;
Dec: 
      VarDec                    { $$ = make_internal_node1("Dec", @$, $1); }
    | VarDec ASSIGN Exp         { $$ = make_internal_node3("Dec", @$, $1, $2, $3); }
    ;

Exp: 
      Exp ASSIGN Exp            { $$ = make_internal_node3("Exp", @$, $1, $2, $3); }
    | Exp AND Exp               { $$ = make_internal_node3("Exp", @$, $1, $2, $3); }
    | Exp OR Exp                { $$ = make_internal_node3("Exp", @$, $1, $2, $3); }
    | Exp LT Exp                { $$ = make_internal_node3("Exp", @$, $1, $2, $3); }
    | Exp LE Exp                { $$ = make_internal_node3("Exp", @$, $1, $2, $3); }
    | Exp GT Exp                { $$ = make_internal_node3("Exp", @$, $1, $2, $3); }
    | Exp GE Exp                { $$ = make_internal_node3("Exp", @$, $1, $2, $3); }
    | Exp NE Exp                { $$ = make_internal_node3("Exp", @$, $1, $2, $3); }
    | Exp EQ Exp                { $$ = make_internal_node3("Exp", @$, $1, $2, $3); }
    | Exp PLUS Exp              { $$ = make_internal_node3("Exp", @$, $1, $2, $3); }
    | Exp MINUS Exp             { $$ = make_internal_node3("Exp", @$, $1, $2, $3); }
    | Exp MUL Exp               { $$ = make_internal_node3("Exp", @$, $1, $2, $3); }
    | Exp DIV Exp               { $$ = make_internal_node3("Exp", @$, $1, $2, $3); }
    | LP Exp RP                 { $$ = make_internal_node3("Exp", @$, $1, $2, $3); }
    | MINUS Exp                 { $$ = make_internal_node2("Exp", @$, $1, $2); }
    | NOT Exp                   { $$ = make_internal_node2("Exp", @$, $1, $2); }
    | ID LP Args RP             { $$ = make_internal_node4("Exp", @$, $1, $2, $3, $4); }
    | ID LP RP                  { $$ = make_internal_node3("Exp", @$, $1, $2, $3); }
    | Exp LB Exp RB             { $$ = make_internal_node4("Exp", @$, $1, $2, $3, $4); }
    | Exp DOT ID                { $$ = make_internal_node3("Exp", @$, $1, $2, $3); }
    | ID                        { $$ = make_internal_node1("Exp", @$, $1); }
    | INT                       { $$ = make_internal_node1("Exp", @$, $1); }
    | FLOAT                     { $$ = make_internal_node1("Exp", @$, $1); }
    | CHAR                      { $$ = make_internal_node1("Exp", @$, $1); }
    | String                    { $$ = make_internal_node1("Exp", @$, $1); }

    | LP Exp error              { printf("Error type B at Line %d: Missing closing parenthesis ')'\n", @2.last_line); }
    | ID LP Args error          { printf("Error type B at Line %d: Missing closing parenthesis ')'\n", @3.last_line); }
    | ID LP error               { printf("Error type B at Line %d: Missing closing parenthesis ')'\n", @2.last_line); }
    ;
Args: 
      Exp COMMA Args            { $$ = make_internal_node3("Args", @$, $1, $2, $3); }
    | Exp                       { $$ = make_internal_node1("Args", @$, $1); }
    ;

String:
      STRING                                      { $$ = make_internal_node1("String", @$, $1); }
    | STRING_BEGIN StringInternalList STRING_END  { $$ = make_internal_node3("String", @$, $1, $2, $3); }
    ;

StringInternalList:
      Exp                                         { $$ = make_internal_node1("StringInternalList", @$, $1); }
    | Exp STRING_INTERNAL StringInternalList      { $$ = make_internal_node3("StringInternalList", @$, $1, $2, $3); }
    ;

%%

void yyerror(const char *s) {
    error_happen = 1;
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
        int error = yyparse();
        if (!error && !error_happen) {
            print_ASTTree(program);
        }
        if (error) {
            fprintf(stderr, "Syntax error\n");
        }
        return 0;
    }
    else{
        fputs("Too much arguments!\n", stderr);
        return 1;
    }
}

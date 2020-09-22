%{
    #include"lex.yy.c"
    int result = 0;
    void yyerror(const char *s);

%}
%token LP RP LB RB LC RC
%%

String: %empty {}
    | String LP String RP { result = 1; }
    | String LB String RB { result = 1; }
    | String LC String RC { result = 1; }
    ;
%%

void yyerror(const char *s){
    result = 0;
}

int validParentheses(char *expr){
    result = 0;
    yy_scan_string(expr);
    yyparse();
    return result;
}

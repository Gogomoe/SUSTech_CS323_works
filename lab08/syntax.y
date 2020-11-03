%{
    #include "json.h"
    #include"lex.yy.c"
    void yyerror(const char*);
%}
%define api.value.type {JsonValue}

%token LC RC LB RB COLON COMMA
%token STRING NUMBER
%token TRUE FALSE VNULL
%%

Json:
      Value { $$ = $1; }
    ;
Value:
      Object { $$ = $1; }
    | Array { $$ = $1; }
    | STRING { $$ = $1; }
    | NUMBER { $$ = $1; }
    | TRUE { $$ = $1; }
    | FALSE { $$ = $1; }
    | VNULL { $$ = $1; }
    ;
Object:
      LC RC {
        $$.type = JSON_OBJECT;
        $$.object = calloc(1, sizeof(JsonObject));
        $$.object->fields = NULL;
      }
    | LC Members RC {
        $$ = $2;
    }
    ;
Members:
      Member { $$ = $1; }
    | Member COMMA Members {
        $$ = $1;
        $$.object->fields->next = $3.object->fields;

        char* key = $$.object->fields->name;

        JsonObjectField* field = $3.object->fields;
        while (field != NULL) {
            if(strcmp(key, field->name) == 0) {
                printf("duplicate key: %s\n", key);
            }
            field = field->next;
        }
      }
    ;
Member:
      STRING COLON Value { 
        $$.type = JSON_OBJECT; 
        $$.object = calloc(1, sizeof(JsonObject)); 
        $$.object->fields = calloc(1, sizeof(JsonObjectField));
        $$.object->fields->name = $1.string;
        $$.object->fields->value = &$3;
        $$.object->fields->next = NULL;
      }
    ;
Array:
      LB RB {
        $$.type = JSON_ARRAY; 
        $$.array = calloc(1, sizeof(JsonArray));
        $$.array->values = NULL;
      }
    | LB Values RB {
        $$ = $2;
      }
    ;
Values:
      Value {
        $$.type = JSON_ARRAY; 
        $$.array = calloc(1, sizeof(JsonArray));
        $$.array->values = calloc(1, sizeof(JsonArrayField));
        $$.array->values->value = &$1;
        $$.array->values->next = NULL;
      }
    | Value COMMA Values  {
        $$.type = JSON_ARRAY; 
        $$.array = calloc(1, sizeof(JsonArray));
        $$.array->values = calloc(1, sizeof(JsonArrayField));
        $$.array->values->value = &$1;
        $$.array->values->next = $3.array->values;
      }
    ;
%%

void yyerror(const char *s){
    printf("syntax error: ");
}

int main(int argc, char **argv){
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        exit(-1);
    }
    else if(!(yyin = fopen(argv[1], "r"))) {
        perror(argv[1]);
        exit(-1);
    }
    yyparse();
    return 0;
}

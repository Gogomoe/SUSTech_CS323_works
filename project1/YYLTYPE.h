# define YYLTYPE_IS_DECLARED 1
typedef struct YYLTYPE YYLTYPE;

struct YYLTYPE {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
};

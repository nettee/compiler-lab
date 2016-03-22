enum AstNodeType {
    PROGRAM = 400,
    EXT_DEF_LIST = 401,
    EXT_DEF = 402,
    EXT_DEC_LIST = 403,
    SPECIFIER = 404,
    STRUCT_SPECIFIER = 405,
    OPT_TAG = 406,
    TAG = 407,
    VAR_DEC = 408,
    FUN_DEC = 409,
    VAR_LIST = 410,
    PARAM_DEC = 411,
    COMP_ST = 412,
    STMT_LIST = 413,
    STMT = 414,
    DEF_LIST = 415,
    DEF = 416,
    DEC_LIST = 417,
    DEC = 418,
    EXP = 419,
    ARGS = 420,
};

typedef struct {
    int type;
    void *parent;
    // TODO
} VarDec;

typedef struct {
    int type;
    void *parent;
} ExtDef;

typedef struct {
    int type;
    void *parent;
    ExtDef *list;  /* list of extDef */
} ExtDefList;

typedef struct {
    int type;
    void *parent;
    void *extDefList;
} Program;

Program *newProgram(void *);

ExtDefList *newExtDefList();
ExtDefList *ExtDefList_add(void *, void *);

VarDec *newVarDec_1(void *); /* VarDec : ID */


void print_ast();

extern void *root;

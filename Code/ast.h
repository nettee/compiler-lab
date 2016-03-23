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

struct VarDec_;

typedef struct {
    int type;
    void *parent;
    // TODO
} Args;

typedef struct {
    int type;
    void *parent;
    // TODO
} Exp;

typedef struct {
    int type;
    void *parent;
    struct VarDec_ *varDec;
    // TODO
} Dec;

typedef struct {
    int type;
    void *parent;
    // TODO
} DecList;

typedef struct {
    int type;
    void *parent;
    // TODO
} Def;

typedef struct {
    int type;
    void *parent;
    // TODO
} DefList;

typedef struct {
    int type;
    void *parent;
    // TODO
} Stmt;

typedef struct {
    int type;
    void *parent;
    // TODO
} StmtList;

typedef struct {
    int type;
    void *parent;
    // TODO
} CompSt;

typedef struct {
    int type;
    void *parent;
    // TODO
} ParamDec;

typedef struct {
    int type;
    void *parent;
    // TODO
} VarList;

typedef struct {
    int type;
    void *parent;
    // TODO
} FunDec;

typedef struct VarDec_ {
    int type;
    void *parent;
    int id_index;
    // TODO
} VarDec;

typedef struct {
    int type;
    void *parent;
    // TODO
} Tag;

typedef struct {
    int type;
    void *parent;
    // TODO
} OptTag;

typedef struct {
    int type;
    void *parent;
    // TODO
} StructSpecifier;

typedef struct {
    int type;
    void *parent;
    // TODO
} Specifier;

typedef struct {
    int type;
    void *parent;
    // TODO
} ExtDecList;

typedef struct {
    int type;
    void *parent;
    // TODO
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

VarDec *newVarDec_1(int); /* VarDec : ID */

DecList *newDecList(void *); /* DecList : Dec */
DecList *DecList_insert(void *, void *); /* DecList : Dec, DecList */

Dec *newDec_1(void *); /* Dec : VarDec */

void print_ast();

extern void *root;

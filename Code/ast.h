enum ExpType {
    EXP_T_INFIX = 2100,
    EXP_T_ID,
    EXP_T_INT,
    EXP_T_FLOAT,
    // TODO
};

enum StmtType {
    STMT_T_EXP = 2200,
    STMT_T_RETURN,
    // TODO
};

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

typedef struct ListNode_ {
    struct ListNode_ *next;
    void *child;
} ListNode;

struct Program_;
struct ExtDefList_;
struct ExtDef_;
struct ExtDecList_;
struct Specifier_;
struct StructSpecifier_;
struct OptTag_;
struct Tag_;
struct VarDec_;
struct FunDec_;
struct VarList_;
struct ParamDec_;
struct CompSt_;
struct StmtList_;
struct Stmt_;
struct DefList_;
struct Def_;
struct DecList_;
struct Dec_;
struct Exp_;
struct Args_;

typedef struct {
    int type;
    // TODO
} Args;

typedef struct Exp_ {
    int type;
    int exp_type;
    union {
        struct {
            struct Exp_ *exp_left;
            int op;
            struct Exp_ *exp_right;
        };
        int id_index;
        int int_index;
        int float_index;
    };
} Exp;

typedef struct {
    int type;
    struct VarDec_ *varDec;
    // TODO
} Dec;

typedef struct DecList_ {
    int type;
    ListNode *list_dec;
} DecList;

typedef struct Def_ {
    int type;
    struct Specifier_ *specifier;
    struct DecList_ *decList;
} Def;

typedef struct DefList_ {
    int type;
    ListNode *list_def;
} DefList;

typedef struct Stmt_ {
    int type;
    int stmt_type;
    union {
        Exp *exp; // STMT_T_EXP, STMT_T_RETURN
    };
} Stmt;

typedef struct StmtList_ {
    int type;
    ListNode *list_stmt;
} StmtList;

typedef struct CompSt_ {
    int type;
    DefList *defList;
    StmtList *stmtList;
} CompSt;

typedef struct {
    int type;
    // TODO
} ParamDec;

typedef struct {
    int type;
    // TODO
} VarList;

typedef struct {
    int type;
    // TODO
} FunDec;

typedef struct VarDec_ {
    int type;
    int id_index;
    // TODO
} VarDec;

typedef struct {
    int type;
    // TODO
} Tag;

typedef struct {
    int type;
    // TODO
} OptTag;

typedef struct {
    int type;
    // TODO
} StructSpecifier;

typedef struct Specifier_ {
    int type;
    int type_value;
    // TODO
} Specifier;

typedef struct {
    int type;
    // TODO
} ExtDecList;

typedef struct {
    int type;
    // TODO
} ExtDef;

typedef struct {
    int type;
    ExtDef *list;  /* list of extDef */
} ExtDefList;

typedef struct {
    int type;
    void *extDefList;
} Program;

Program *newProgram(void *);

ExtDefList *newExtDefList();
ExtDefList *ExtDefList_add(void *, void *);

Specifier *newSpecifier_1(int);

VarDec *newVarDec_1(int); /* VarDec : ID */

CompSt *newCompSt(void *, void *); // CompSt : { DefList StmtList }

StmtList *newStmtList(); // StmtList : (epsilon)
StmtList *StmtList_insert(void *, void *); // StmtList : Stmt StmtList

Stmt *newStmt_exp(void *); // Stmt : Exp ;
Stmt *newStmt_RETURN(void *); // Stmt : return Exp ;

DefList *newDefList(); // DefList : (epsilon)
DefList *DefList_insert(void *, void *); // DefList : Def DefList

Def *newDef(void *, void *); // Def : Specifier DecList ;

DecList *newDecList(void *); // DecList : Dec
DecList *DecList_insert(void *, void *); // DecList : Dec, DecList

Dec *newDec_1(void *); /* Dec : VarDec */

Exp *newExp_infix(int, void *, void *);
Exp *newExp_ID(int); // Exp : ID
Exp *newExp_INT(int); // Exp : INT
Exp *newExp_FLOAT(int); // Exp : FLOAT


extern void *root;

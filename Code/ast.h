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

typedef struct {
    int type;
    // TODO
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

typedef struct {
    int type;
    // TODO
} Stmt;

typedef struct {
    int type;
    // TODO
} StmtList;

typedef struct {
    int type;
    // TODO
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

DefList *newDefList(); // DefList : (epsilon)
DefList *DefList_insert(void *, void *); // DefList : Def DefList

Def *newDef(void *, void *); // Def : Specifier DecList ;

DecList *newDecList(void *); // DecList : Dec
DecList *DecList_insert(void *, void *); // DecList : Dec, DecList

Dec *newDec_1(void *); /* Dec : VarDec */


void print_ast();

extern void *root;

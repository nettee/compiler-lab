enum ExpType {
    EXP_T_INFIX = 2100,
    EXP_T_PAREN,
    EXP_T_UNARY,
    EXP_T_CALL,
    EXP_T_SUBSCRIPT,
    EXP_T_DOT,
    EXP_T_ID,
    EXP_T_INT,
    EXP_T_FLOAT,
};

enum StmtType {
    STMT_T_EXP = 2200,
    STMT_T_COMP_ST,
    STMT_T_RETURN,
    STMT_T_IF,
    STMT_T_IF_ELSE,
    STMT_T_WHILE,
};

enum SpecifierType {
    SPECIFIER_T_TYPE = 2300,
    SPECIFIER_T_STRUCT,
};

enum ExtDefType {
    EXT_DEF_T_VAR = 2400,
    EXT_DEF_T_FUN,
    EXT_DEF_T_STRUCT,
};

enum StructSpecifierType {
    STRUCT_SPECIFIER_T_DEC = 2500,
    STRUCT_SPECIFIER_T_DEF,
};

enum VarDecType {
    VAR_DEC_T_ID = 2600,
    VAR_DEC_T_DIM,
};

enum NonterminalType {
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

typedef struct IntListNode_ {
    struct IntListNode_ *next;
    int value;
} IntListNode;

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

typedef struct Args_ {
    int type;
    int lineno;
    struct Exp_ *exp;
    struct Args_ *args; // may be NULL
} Args;

typedef struct Exp_ {
    int type;
    int lineno;
    int exp_type;
    union {
        // for Exp_infix
        struct {
            struct Exp_ *exp_left;
            int op;
            int op_yylval;
            struct Exp_ *exp_right;
        } infix;

        // for Exp_unary
        struct {
            int op;
            struct Exp_ *exp;
        } unary;

        // for Exp : ( Exp )
        struct {
            struct Exp_ *exp;
        } paren;

        // for Exp_subscript
        struct {
            struct Exp_ *array;
            struct Exp_ *index;
        } subscript;

        // for Exp : Exp . ID
        struct {
            struct Exp_ *exp;
            int id_index;
        } dot;

        // for Exp : ID ( Args )
        // and Exp : ID ( )
        struct {
            int id_index;
            struct Args_ *args;
        } call;

        int id_index; // for Exp : ID
        int int_index; // for Exp : INT
        int float_index; // for Exp : FLOAT
    };
} Exp;

typedef struct Dec_ {
    int type;
    int lineno;
    struct VarDec_ *varDec;
    struct Exp_ *exp; // can be NULL
} Dec;

typedef struct DecList_ {
    int type;
    int lineno;
    struct Dec_ *dec;
    struct DecList_ *decList; // may be NULL
} DecList;

typedef struct Def_ {
    int type;
    int lineno;
    struct Specifier_ *specifier;
    struct DecList_ *decList;
} Def;

typedef struct DefList_ {
    int type;
    int lineno;
    struct Def_ *def;
    struct DefList_ *defList;
} DefList;

typedef struct Stmt_ {
    int type;
    int lineno;
    int stmt_type;
    
    union {
        // For Stmt : Exp ;
        struct {
            struct Exp_ *exp; 
        } exp;
        // For Stmt : CompSt
        struct {
            struct CompSt_ *compSt;
        } compst;
        // For Stmt : return Exp ;
        struct {
            struct Exp_ *exp;
        } return_;
        // For Stmt : if ( Exp ) Stmt
        struct {
            struct Exp_ *exp;
            struct Stmt_ *then_stmt;
        } if_;
        // For Stmt : if ( Exp ) Stmt else Stmt
        struct {
            struct Exp_ *exp;
            struct Stmt_ *then_stmt;
            struct Stmt_ *else_stmt;
        } ifelse;
        // For Stmt : while ( Exp ) Stmt
        struct {
            struct Exp_ *exp;
            struct Stmt_ *stmt;
        } while_;
    };
} Stmt;

typedef struct StmtList_ {
    int type;
    int lineno;
    struct Stmt_ *stmt;
    struct StmtList_ *stmtList;
} StmtList;

typedef struct CompSt_ {
    int type;
    int lineno;
    DefList *defList;
    StmtList *stmtList;
} CompSt;

typedef struct ParamDec_ {
    int type;
    int lineno;
    struct Specifier_ *specifier;
    struct VarDec_ *varDec;
} ParamDec;

typedef struct VarList_ {
    int type;
    int lineno;
    struct ParamDec_ *paramDec;
    struct VarList_ *varList; // may be NULL
} VarList;

typedef struct FunDec_ {
    int type;
    int lineno;
    int id_index;
    struct VarList_ *varList;
} FunDec;

typedef struct VarDec_ {
    int type;
    int lineno;
    int vardec_type;
    union {
        // for VarDec : ID
        int id_index;
        // for VarDec : VarDec [ INT ]
        struct {
            struct VarDec_ *varDec;
            int int_index;
        } dim;
    };
} VarDec;

typedef struct Tag_ {
    int type;
    int lineno;
    int id_index;
} Tag;

typedef struct OptTag_ {
    int type;
    int lineno;
    int id_index; // id_index == -1 indicates None
} OptTag;

typedef struct StructSpecifier_ {
    int type;
    int lineno;
    int structspecifier_type;
    union {
        // for StructSpecifier : STRUCT Tag
        struct {
            struct Tag_ *tag;
        } dec;
        // for StructSpecifier : struct OptTag { DefList }
        struct {
            struct OptTag_ *optTag;
            struct DefList_ *defList;
        } def;
    };
} StructSpecifier;

typedef struct Specifier_ {
    int type;
    int lineno;
    int specifier_type;
    union {
        int type_index;
        struct StructSpecifier_ *structSpecifier;
    };
} Specifier;

typedef struct ExtDecList_ {
    int type;
    int lineno;
    struct VarDec_ *varDec;
    struct ExtDecList_ *extDecList; // may be NULL
} ExtDecList;

typedef struct ExtDef_ {
    int type;
    int lineno;
    int extdef_type;
    union {
        // for ExtDef : Specifier FunDec CompSt
        struct {
            struct Specifier_ *specifier;
            struct FunDec_ *funDec;
            struct CompSt_ *compSt;
        } fun;
        // for ExtDef : Specifier ;
        struct {
            struct Specifier_ *specifier;
        } struct_;
        // for ExtDef : Specifier ExtDecList ;
        struct {
            struct Specifier_ *specifier;
            struct ExtDecList_ *extDecList;
        } var;
    };
} ExtDef;

typedef struct ExtDefList_ {
    int type;
    int lineno;
    struct ExtDef_ *extDef; // may be NULL
    struct ExtDefList_ *extDefList; // may be NULL
} ExtDefList;

typedef struct Program_ {
    int type;
    int lineno;
    void *extDefList;
} Program;

Program *newProgram(void *, int);

ExtDefList *newExtDefList(void *, void *, int);

// ExtDef : Specifier ExtDecList ;
ExtDef *newExtDef_var(void *, void *, int); 
// ExtDef : Specifier ;
ExtDef *newExtDef_struct(void *, int); 
// ExtDef : Specifier FunDec CompSt
ExtDef *newExtDef_fun(void *, void *, void *, int); 

ExtDecList *newExtDecList(void *, void *, int); 

Specifier *newSpecifier_TYPE(int, int);
Specifier *newSpecifier_struct(void *, int);

StructSpecifier *newStructSpecifier_dec(void *, int);
StructSpecifier *newStructSpecifier_def(void *, void *, int);

OptTag *newOptTag(int, int);

Tag *newTag(int, int);

// VarDec : ID 
VarDec *newVarDec_ID(int, int); 
// VarDec : VarDec [ INT ]
VarDec *newVarDec_dim(void *, int, int); 

FunDec *newFunDec(int, void *, int);

VarList *newVarList(void *, void *, int); 

ParamDec *newParamDec(void *, void *, int); 

CompSt *newCompSt(void *, void *, int); 

StmtList *newStmtList(void *, void *, int); 

// Stmt : Exp ;
Stmt *newStmt_exp(void *, int); 
// Stmt : CompSt
Stmt *newStmt_COMP_ST(void *, int); 
// Stmt : return Exp ;
Stmt *newStmt_RETURN(void *, int); 
// Stmt : if ( Exp ) Stmt
Stmt *newStmt_if(void *, void *, int); 
// Stmt : if ( Exp ) Stmt else Stmt
Stmt *newStmt_ifelse(void *, void *, void *, int); 
// Stmt : while ( Exp ) Stmt
Stmt *newStmt_WHILE(void *, void *, int); 

DefList *newDefList(void *, void *, int); 

Def *newDef(void *, void *, int); 

DecList *newDecList(void *, void *, int); 

Dec *newDec(void *, void *, int);

// Exp : Exp = Exp
// Exp : Exp [+-*/] Exp
// Exp : Exp && Exp
// Exp : Exp || Exp
// Exp : Exp < Exp
// Exp : Exp <= Exp
// Exp : Exp > Exp
// Exp : Exp >= Exp
// Exp : Exp == Exp
// Exp : Exp != Exp
Exp *newExp_infix(int, void *, int, void *, int);
// Exp : ( Exp )
Exp *newExp_paren(void *, int); 
// Exp : - Exp | Exp : ! Exp
Exp *newExp_unary(int, void *, int);
Exp *newExp_call(int, void *, int); // Exp : ID ( Args )
// Exp: Exp [ Exp ]
Exp *newExp_subscript(void *, void *, int); 
// Exp : Exp . ID
Exp *newExp_DOT(void *, int, int); 
// Exp : ID
Exp *newExp_ID(int, int); 
// Exp : INT
Exp *newExp_INT(int, int); 
// Exp : FLOAT
Exp *newExp_FLOAT(int, int); 

Args *newArgs(void *, void *, int); 

extern void *root;

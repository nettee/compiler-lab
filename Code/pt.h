#ifndef __PT_H__
#define __PT_H__

#include "common.h"
#include "st.h"

enum ExpKind {
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

enum StmtKind {
    STMT_T_EXP = 2200,
    STMT_T_COMP_ST,
    STMT_T_RETURN,
    STMT_T_IF,
    STMT_T_IF_ELSE,
    STMT_T_WHILE,
};

enum SpecifierKind {
    SPECIFIER_T_BASIC = 2300,
    SPECIFIER_T_STRUCT,
};

enum ExtDefKind {
    EXT_DEF_T_VAR = 2400,
    EXT_DEF_T_FUN,
    EXT_DEF_T_STRUCT,
};

enum StructSpecifierKind {
    STRUCT_SPECIFIER_T_DEC = 2500,
    STRUCT_SPECIFIER_T_DEF,
};

enum VarDecKind {
    VAR_DEC_T_ID = 2600,
    VAR_DEC_T_DIM,
};

enum NonterminalKind {
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
    int kind;
    int lineno;
    struct Exp_ *exp;
    struct Args_ *args; // may be NULL
    TypeNode *attr_argTypeListTail;
} Args;

typedef struct Exp_ {
    int kind;
    int lineno;
    int exp_kind;
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
            char *id_text;
        } dot;

        // for Exp : ID ( Args )
        // and Exp : ID ( )
        struct {
            char *id_text;
            struct Args_ *args;
        } call;

        char *id_text; // for Exp : ID
        int int_value; // for Exp : INT
        int float_value; // for Exp : FLOAT
    };

    Type *attr_type;
    bool attr_lvalue;
} Exp;

typedef struct Dec_ {
    int kind;
    int lineno;
    struct VarDec_ *varDec;
    struct Exp_ *exp; // can be NULL
    Type *attr_type;
} Dec;

typedef struct DecList_ {
    int kind;
    int lineno;
    struct Dec_ *dec;
    struct DecList_ *decList; // may be NULL
    Type *attr_type;
} DecList;

typedef struct Def_ {
    int kind;
    int lineno;
    struct Specifier_ *specifier;
    struct DecList_ *decList;
} Def;

typedef struct DefList_ {
    int kind;
    int lineno;
    struct Def_ *def;
    struct DefList_ *defList;
} DefList;

typedef struct Stmt_ {
    int kind;
    int lineno;
    int stmt_kind;
    
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
    int kind;
    int lineno;
    struct Stmt_ *stmt;
    struct StmtList_ *stmtList;
} StmtList;

typedef struct CompSt_ {
    int kind;
    int lineno;
    DefList *defList;
    StmtList *stmtList;
} CompSt;

typedef struct ParamDec_ {
    int kind;
    int lineno;
    struct Specifier_ *specifier;
    struct VarDec_ *varDec;
    Type *attr_paramType;
} ParamDec;

typedef struct VarList_ {
    int kind;
    int lineno;
    struct ParamDec_ *paramDec;
    struct VarList_ *varList; // may be NULL
    TypeNode *attr_paramTypeListTail;
} VarList;

typedef struct FunDec_ {
    int kind;
    int lineno;
    char *id_text;
    struct VarList_ *varList;
    Type *attr_return_type;
} FunDec;

typedef struct VarDec_ {
    int kind;
    int lineno;
    int vardec_kind;
    union {
        // for VarDec : ID
        char *id_text;
        // for VarDec : VarDec [ INT ]
        struct {
            struct VarDec_ *varDec;
            int int_value;
        } dim;
    };
    Type *attr_type;
} VarDec;

typedef struct Tag_ {
    int kind;
    int lineno;
    char *id_text;
} Tag;

typedef struct OptTag_ {
    int kind;
    int lineno;
    char *id_text; // id == NULL indicates None
} OptTag;

typedef struct StructSpecifier_ {
    int kind;
    int lineno;
    int structspecifier_kind;
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
    int kind;
    int lineno;
    int specifier_kind;
    union {
        int type_index;
        struct StructSpecifier_ *structSpecifier;
    };
    Type *attr_type;
} Specifier;

typedef struct ExtDecList_ {
    int kind;
    int lineno;
    struct VarDec_ *varDec;
    struct ExtDecList_ *extDecList; // may be NULL
    Type *attr_type;
} ExtDecList;

typedef struct ExtDef_ {
    int kind;
    int lineno;
    int extdef_kind;
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
    int kind;
    int lineno;
    struct ExtDef_ *extDef; // may be NULL
    struct ExtDefList_ *extDefList; // may be NULL
} ExtDefList;

typedef struct Program_ {
    int kind;
    int lineno;
    void *extDefList;
} Program;

extern void *root;

Program *newProgram(void *arg0, int lineno);
ExtDefList *newExtDefList(void *arg0, void *arg1, int lineno);
ExtDef *newExtDef_var(void *arg0, void *arg1, int lineno);
ExtDef *newExtDef_struct(void *arg0, int lineno);
ExtDef *newExtDef_fun(void *arg0, void *arg1, void *arg2, int lineno);
ExtDecList *newExtDecList(void *arg0, void *arg1, int lineno);
Specifier *newSpecifier_basic(int type_index, int lineno);
Specifier *newSpecifier_struct(void *arg0, int lineno);
StructSpecifier *newStructSpecifier_dec(void *arg0, int lineno);
StructSpecifier *newStructSpecifier_def(void *arg0, void *arg1, int lineno);
OptTag *newOptTag(char *id_text, int lineno);
Tag *newTag(char *id_text, int lineno);
VarDec *newVarDec_ID(char *id_text, int lineno);
VarDec *newVarDec_dim(void *arg0, int int_value, int lineno);
FunDec *newFunDec(char *id_text, void *arg0, int lineno);
VarList *newVarList(void *arg0, void *arg1, int lineno);
ParamDec *newParamDec(void *arg0, void *arg1, int lineno);
CompSt *newCompSt(void *arg0, void *arg1, int lineno);
StmtList *newStmtList(void *arg0, void *arg1, int lineno);
Stmt *newStmt_exp(void *arg0, int lineno);
Stmt *newStmt_COMP_ST(void *arg0, int lineno);
Stmt *newStmt_RETURN(void *arg0, int lineno);
Stmt *newStmt_if(void *arg0, void *arg1, int lineno);
Stmt *newStmt_ifelse(void *arg0, void *arg1, void *arg2, int lineno);
Stmt *newStmt_WHILE(void *arg0, void *arg1, int lineno);
DefList *newDefList(void *arg0, void *arg1, int lineno);
Def *newDef(void *arg0, void *arg1, int lineno);
DecList *newDecList(void *arg0, void *arg1, int lineno);
Dec *newDec(void *arg0, void *arg1, int lineno);
Exp *newExp_infix(int op, void *arg0, int op_yylval, void *arg1, int lineno);
Exp *newExp_paren(void *arg0, int lineno);
Exp *newExp_unary(int op, void *arg0, int lineno);
Exp *newExp_call(char *id_text, void *arg0, int lineno);
Exp *newExp_subscript(void *arg0, void *arg1, int lineno);
Exp *newExp_DOT(void *arg0, char *id_text, int lineno);
Exp *newExp_ID(char *id_text, int lineno);
Exp *newExp_INT(int int_value, int lineno);
Exp *newExp_FLOAT(float float_value, int lineno);
Args *newArgs(void *arg0, void *arg1, int lineno);

#endif

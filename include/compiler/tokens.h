#ifndef COMPILER_TOKENS_H
#define COMPILER_TOKENS_H

#include <compiler/common.h>


typedef enum EToken {
    TK_INTCONST = -1,
    TK_FLOATCONST = -2,
    TK_IDENTIFIER = -3,
    TK_STRING = -4,
    TK_STRING_WIDE = -5,
    TK_ILLEGAL = -6,
    TK_EOL = -7,
    TK_END = 0,
    TK_MACRO_ARG_1 = 1,
    TK_MACRO_ARG_2 = 2,
    TK_MACRO_ARG_HASH = 3, // #something, followed by the arg index
    TK_4 = 4,
    TK_5 = 5,
    TK_AUTO = 256,
    TK_REGISTER,
    TK_STATIC,
    TK_EXTERN,
    TK_TYPEDEF,
    TK_INLINE,
    TK_VOID,
    TK_CHAR,
    TK_SHORT,
    TK_INT,
    TK_LONG,
    TK_FLOAT,
    TK_DOUBLE,
    TK_SIGNED,
    TK_UNSIGNED,
    TK_STRUCT,
    TK_UNION,
    TK_ENUM,
    TK_CLASS,
    TK_UNK_113,
    TK_UNK_114,
    TK_UNK_115,
    TK_UNK_116,
    TK_UNK_117,
    TK_UNK_118,
    TK_UNK_119,
    TK_UNK_11A,
    TK_UU_VECTOR,
    TK_UU_TYPEOF_UU,
    TK_BOOL,
    TK_WCHAR_T,
    TK_U_COMPLEX,
    TK_U_IMAGINARY,
    TK_TYPENAME,
    TK_CONST,
    TK_VOLATILE,
    TK_PASCAL,
    TK_UU_DECLSPEC,
    TK_UU_STDCALL,
    TK_UU_CDECL,
    TK_UU_FASTCALL,
    TK_UU_FAR,
    TK_EXPLICIT,
    TK_MUTABLE,
    TK_ONEWAY,
    TK_IN,
    TK_INOUT,
    TK_OUT,
    TK_BYCOPY,
    TK_BYREF,
    TK_ASM = 310,
    TK_CASE,
    TK_DEFAULT,
    TK_IF,
    TK_ELSE,
    TK_SWITCH,
    TK_WHILE,
    TK_DO,
    TK_FOR,
    TK_GOTO,
    TK_CONTINUE,
    TK_BREAK,
    TK_RETURN,
    TK_SIZEOF,
    TK_CATCH,
    TK_DELETE,
    TK_FRIEND,
    TK_NEW,
    TK_OPERATOR,
    TK_PRIVATE,
    TK_PROTECTED,
    TK_PUBLIC,
    TK_TEMPLATE,
    TK_THIS,
    TK_THROW,
    TK_TRY,
    TK_VIRTUAL,
    TK_INHERITED,
    TK_CONST_CAST,
    TK_DYNAMIC_CAST,
    TK_NAMESPACE,
    TK_REINTERPRET_CAST,
    TK_STATIC_CAST,
    TK_USING,
    TK_TRUE,
    TK_FALSE,
    TK_TYPEID,
    TK_EXPORT,
    TK_MULT_ASSIGN,
    TK_DIV_ASSIGN,
    TK_MOD_ASSIGN,
    TK_ADD_ASSIGN,
    TK_SUB_ASSIGN,
    TK_SHL_ASSIGN,
    TK_SHR_ASSIGN,
    TK_AND_ASSIGN,
    TK_XOR_ASSIGN,
    TK_OR_ASSIGN,
    TK_LOGICAL_OR,
    TK_LOGICAL_AND,
    TK_LOGICAL_EQ,
    TK_LOGICAL_NE,
    TK_LESS_EQUAL,
    TK_GREATER_EQUAL,
    TK_SHL,
    TK_SHR,
    TK_INCREMENT,
    TK_DECREMENT,
    TK_ARROW,
    TK_ELLIPSIS,
    TK_DOT_STAR,
    TK_ARROW_STAR,
    TK_COLON_COLON,
    TK_AT_INTERFACE,
    TK_AT_IMPLEMENTATION,
    TK_AT_PROTOCOL,
    TK_AT_END,
    TK_AT_PRIVATE,
    TK_AT_PROTECTED,
    TK_AT_PUBLIC,
    TK_AT_CLASS,
    TK_AT_SELECTOR,
    TK_AT_ENCODE,
    TK_AT_DEFS,
    TK_SELF,
    TK_SUPER,
    TK_NEW_ARRAY,
    TK_DELETE_ARRAY,
    TK_UU_ALIGNOF_UU,
    TK_RESTRICT,
    TK_UU_ATTRIBUTE_UU,
    TK_UU_UUIDOF
} EToken;

struct FileOffsetInfo {
    CPrepFileInfo *file;
    SInt32 tokenline;
    SInt32 tokenoffset;
    Boolean is_inline;
};

typedef union TData {
    HashNameNode *tkidentifier;
    CInt64 tkintconst;
    Float tkfloatconst;
    struct {
        char *data;
        SInt32 size;
    } tkstring;
} TData;

struct TStreamElement {
    SInt16 tokentype;
    SInt16 subtype;
    CPrepFileInfo *tokenfile;
    SInt32 tokenoffset;
    SInt32 tokenline;
    TData data;
};

struct TokenStream {
    SInt32 tokens;
    TStreamElement *firsttoken;
};

#endif


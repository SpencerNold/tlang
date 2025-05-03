#ifndef TOKENS_H
#define TOKENS_H

#define IS_TYPE(mask, x) (mask | x) == mask

#define ERROR          0x000
#define WHITESPACE     0x100
#define NOP            0x100
#define STRING_LIT     0x101
#define FLOATING_POINT 0x102
#define INTEGER        0x103
#define IDENTIFIER     0x104

#define O_PARENTHESES  0x105
#define C_PARENTHESES  0x106
#define O_C_BRACKET    0x107
#define C_C_BRACKET    0x108
#define O_BRACKET      0x109
#define C_BRACKET      0x10A
#define DOT            0x10B
#define COMMA          0x10C

#define TRUE_VALUE     0x110
#define FALSE_VALUE    0x111
#define NULL_VALUE     0x112

// types
#define TYPE_MASK      0x11FF
#define INT8           0x1100
#define INT16          0x1101
#define INT32          0x1102
#define INT64          0x1103
#define UINT8          0x1104
#define UINT16         0x1105
#define UINT32         0x1106
#define UINT64         0x1107
#define CHAR           0x1108
#define BOOL           0x1109
#define FLOAT32        0x110A
#define FLOAT64        0x110B
#define FUNCTION       0x110C
#define VOID           0x110D
#define STRING         0x110E

// operations
#define OPER_MASK     0x21FF
#define EQ_OPER       0x2100
#define ADD_OPER      0x2101
#define SUB_OPER      0x2102
#define MUL_OPER      0x2103
#define DIV_OPER      0x2104
#define MOD_OPER      0x2105
#define INCR_OPER     0x2106
#define DECR_OPER     0x2107
#define ADD_EQ_OPER   0x2108
#define SUB_EQ_OPER   0x2109
#define MUL_EQ_OPER   0x210A
#define DIV_EQ_OPER   0x210B
#define MOD_EQ_OPER   0x210C
#define POW_OPER      0x210D
#define GT_OPER       0x210E
#define LT_OPER       0x210F
#define GT_EQ_OPER    0x2110
#define LT_EQ_OPER    0x2111
#define BIT_AND_OPER  0x2112
#define BIT_OR_OPER   0x2113
#define BIT_XOR_OPER  0x2114
#define BIT_NOT_OPER  0x2115
#define BIT_SHL_OPER  0x2116
#define BIT_SHR_OPER  0x2117
#define AND_OPER      0x2118
#define OR_OPER       0x2119
#define NOT_OPER      0x211A
#define TERNARY_OPER  0x211B

// keywords
#define KEYWORD_MASK  0x31FF
#define INCLUDE       0x3100
#define PRIVATE       0x3101
#define PROTECTED     0x3102
#define CLASS         0x3103
#define ENUM          0x3104
#define INTERFACE     0x3105
#define NEW           0x3106
#define EXTENDS       0x3107
#define IMPLEMENTS    0x3108
#define ABSTRACT      0x3109
#define FINAL         0x310A
#define ASSERT        0x310B
#define THIS          0x310C
#define RETURN        0x310D
#define IF            0x310E
#define ELSE          0x310F
#define FOR           0x3110
#define IN            0x3111
#define RANGE         0x3112
#define WHILE         0x3113
#define CONTINUE      0x3114
#define BREAK         0x3115
#define IS            0x3116
#define ASYNC         0x3117
#define FUTURE        0x3118
#define UNSAFE        0x3119
#define DEFER         0x311A

typedef struct {
    int start;
    int offset;
    int type;
} TL_LexOut;

TL_LexOut tl_lexer_read_token(char* input, int offset, int length);

#endif
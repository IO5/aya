#pragma once

#include "Types.hpp"

namespace aya {

enum class Token : int {
    EOS = -1,

    NL = 1 << (8*sizeof(char)),

    IDENT,

    DO, END,
    IF, THEN, ELSE, ELIF, WHILE, FOR, IN, REPEAT, UNTIL,
    RETURN, BREAK, NEXT,
    LOCAL, DEF, CLASS,

    NIL, TRUE, FALSE,
    INT, REAL, STRING,

    OR, AND, NOT,
    LT, LE, GT, GE, EQ, NE,

    PLUS = '+', MINUS = '-', MUL = '*', DIV = '/', MOD = '%', EXP = '^', BAR = '|',

    ASSIGN = '=', COMMA = ',', DOT = '.',

    SQBR_L = '[', SQBR_R = ']', CRBR_L = '{', CRBR_R = '}', PAREN_L = '(', PAREN_R = ')',
};

typedef Token TK;

constexpr Token operator ""_tk(char ch) {
    return static_cast<Token>(ch);
}

#ifdef _DEBUG
string_view toString(Token token);
#endif

}


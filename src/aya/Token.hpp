#pragma once

#include "Types.hpp"

#include <type_traits>
#include <variant>
#include <cassert>

namespace aya {

/// used only on lexer / parser boundary
using uint_t = std::make_unsigned_t<int_t>;

class Token {
public:
    enum Type : int {
        EOS = -1,

        NL = 1 << 7, // above ASCII, so there is no collisions with single char tokens

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

    using SemInfo = std::variant<Nil, uint_t, real_t, string_view>;

    // TODO rethink constexpr
    Token(Type t, const SemInfo& info, int line_, int column_) : type(t), semInfo(info), line(line_), column(column_) {
        assert(std::holds_alternative<Nil>(semInfo) ||
            (std::holds_alternative<uint_t>(semInfo) && type == INT) || 
            (std::holds_alternative<real_t>(semInfo) && type == REAL) || 
            (std::holds_alternative<string_view>(semInfo) && (type == IDENT || type == STRING))
        );
    }

    uint_t getInt() const {
        return std::get<uint_t>(semInfo);
    }
    real_t getReal() const {
        return std::get<real_t>(semInfo);
    }
    const string_view& getString() const {
		return std::get<string_view>(semInfo);
    }
    int getLine() const { return line; }
    int getColumn() const { return column; }

    friend bool operator==(const Token& token, Type type) { return token.type == type; }
    friend bool operator==(Type type, const Token& token) { return token == type; }
    friend bool operator!=(const Token& token, Type type) { return ! (token == type); }
    friend bool operator!=(Type type, const Token& token) { return ! (token == type); }
	operator Type() { return type; }

private:
    Type type;
    SemInfo semInfo;
    int line, column;
};

using TK = Token;

constexpr Token::Type operator ""_tk(char ch) {
    return static_cast<Token::Type>(ch);
}

#ifdef _DEBUG
string_view toString(Token::Type token);
#endif

}

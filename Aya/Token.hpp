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

        NL = 1 << 7, // above ASCII

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

    Token(Type t, const SemInfo& semInfo, int line_, int column_) : type(t), line(line_), column(column_) {
        std::visit([this, &semInfo](auto val) { // TODO change to constexpr if
            using T = std::decay_t<decltype(val)>;
            assert((std::is_same_v<T, Nil>) ||
                (std::is_same_v<T, uint_t> && type == INT) || 
                (std::is_same_v<T, real_t> && type == REAL) || 
                (std::is_same_v<T, string_view> && (type == IDENT || type == STRING))
            );
            this->storageAs<T>() = val;
        }, semInfo);
    }
    Token(const Token& other) {
        *this = other;
    }
    Token& operator=(const Token& other) {
        type = other.type;
        switch (type) {
        case INT:
            storageAs<uint_t>() = other.storageAs<uint_t>();
            break;
        case REAL:
            storageAs<real_t>() = other.storageAs<real_t>();
            break;
        case IDENT:
        case STRING:
            storageAs<string_view>() = other.storageAs<string_view>();
            break;
        }
        return *this;
    }

    string_view getText() const {
        assert(type == IDENT || type == STRING);
        return storageAs<string_view>();
    }
    uint_t getInt() const {
        assert(type == INT);
        return storageAs<uint_t>();
    }
    real_t getReal() const {
        assert(type == REAL);
        return storageAs<real_t>();
    }

    int getLine() const { return line; }
    int getColumn() const { return column; }

    bool operator==(Type t) { return type == t; }

private:
    template <typename T> T& storageAs() { return *reinterpret_cast<T*>(&storage); }
    template <typename T> const T& storageAs() const { return *reinterpret_cast<const T*>(&storage); }

    Type type;
    std::aligned_union_t<0, uint_t, real_t, string_view> storage;
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

#pragma once

#include <string>
#include <gsl>
#include <variant>
#include <boost/container/small_vector.hpp>

#include "Types.hpp"
#include "Token.hpp"

namespace aya {

class Lexer {
public:
    /// Constructor
    /// @param inputBuffer  buffer containing whole source code (null terminated)
    /// @param inputName    name of the input source, usually either stdin (default) or filename
    Lexer(gsl::span<const char_t> inputBuffer, string_view inputName = "stdin");
    Lexer(const Lexer&) = delete;
    Lexer& operator=(const Lexer&) = delete;

    Token nextToken() {
        return Token(lex(), semInfo, line, column);
    }
    string_view getContext(const char* ptr);
    

private:
    Token::Type lex();

    char_t current();
    char_t next(int count = 1);
    char_t peekNext(int count = 1);
    bool eat(char_t ch);
    [[noreturn]] void error(string_view msg);

    void readNewLine();
    Token::Type readNumeral();
    template<int base = 10>
    uint_t readInteger();
    real_t readReal(const char_t* start);
    void unescape();
    void readString();
    template<class ClosingPred>
    bool readStringNoEscapeSeq(ClosingPred pred);
    template<class ClosingPred>
    void readStringWithEscapeSeq(const char_t* start, ClosingPred pred);
    Token::Type readIdentOrKeyword();

    gsl::span<const char_t> inputBuffer;
    const char_t* currentPtr;

    string_t inputName;
    int line, column;

    Token::SemInfo semInfo;
    boost::container::small_vector<char_t, 256> tmpBuffer;
};

}
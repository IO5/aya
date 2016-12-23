#pragma once

#include <string>
#include <type_traits>
#include <gsl>
#include <variant>
#include <boost/container/small_vector.hpp>

#include "Types.hpp"
#include "Token.hpp"

namespace aya {

/// used only on lexer / parser boundary
using uint_t = std::make_unsigned_t<int_t>;

class Lexer {
public:
    /// Constructor
    /// @param inputBuffer  buffer containing whole source code (null terminated)
    /// @param inputName    name of the input source, usually either stdin (default) or filename
    Lexer(gsl::span<const char_t> inputBuffer, string_view inputName = "stdin");
    Lexer(const Lexer&) = delete;
    Lexer& operator= (const Lexer&) = delete;

    Token lex();
    string_view getContext(const char* ptr);
    
    struct SemInfo {
        string_view getText() { return std::get<string_view>(val); }
        uint_t      getInt()  { return std::get<uint_t>(val); }
        real_t      getReal() { return std::get<real_t>(val); }
    private:
        std::variant<std::monostate, string_view, uint_t, real_t> val;
        friend Lexer;
    } semInfo;
    /// @returns semantic information for the last parsed token (that had said info)
    SemInfo& getSemInfo() { return semInfo; }

private:
    gsl::span<const char_t> inputBuffer;
    const char_t* currentPtr;

    string_t inputName;
    int line, column;

    boost::container::small_vector<char_t, 256> tmpBuffer;

    char_t current();
    char_t next(int count = 1);
    char_t peekNext(int count = 1);
    bool eat(char_t ch);
    [[noreturn]] void error(string_view msg);

    void readNewLine();
    Token readNumeral();
    template<int base = 10>
    uint_t readInteger();
    real_t readReal(const char_t* start);
    void unescape();
    void readString();
    template<class ClosingPred>
    bool readStringNoEscapeSeq(ClosingPred pred);
    template<class ClosingPred>
    void readStringWithEscapeSeq(const char_t* start, ClosingPred pred);
    Token readIdentOrKeyword();
};


}
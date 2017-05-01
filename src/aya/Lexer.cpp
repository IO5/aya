#include "Lexer.hpp"
#include <cassert>
#include <cctype>
#include <algorithm>
#include <limits>

#include <boost/lexical_cast.hpp>

#include "Exception.hpp"

using namespace std::string_literals;

namespace aya {

constexpr char_t operator ""_(char ch) {
    return static_cast<char_t>(ch);
}

Lexer::Lexer(gsl::span<const char_t> inputBuffer_, string_view inputName_) :
    inputBuffer(inputBuffer_),
    currentPtr(inputBuffer_.data()), 
    inputName(inputName_),
    line(1),
    column(1) {
    if (inputBuffer.size() == 0)
        throw Exception("input buffer is empty");
    if (inputBuffer.data()[inputBuffer.size() - 1] != 0)
        throw Exception("input buffer is not null terminated");
}

inline char_t Lexer::current() {
    return *currentPtr;
}

inline char_t Lexer::next(int count) {
    column += count;
    currentPtr += count;
    return *currentPtr;
}

inline char_t Lexer::peekNext(int count) {
    return currentPtr[count];
}

inline bool Lexer::eat(char_t ch) {
    if (current() == ch) {
        next();
        return true;
    }
    return false;
}

//string_view Lexer::getContext(const char_t* ptr) {
//    assert(ptr >= inputBuffer.data() && ptr < inputBuffer.data() + inputBuffer.size());
//    const char_t* lineStart = ptr;
//    const char_t* lineEnd = ptr;
//    while (*lineStart != '\n' && *lineStart != '\r' && lineStart > inputBuffer.data())
//        --lineStart;
//    while (*lineEnd != '\r' && *lineEnd != '\n' && *lineEnd != '\0')
//        ++lineEnd;
//
//    constexpr auto MAX_LEN = 80;
//    if (lineEnd - lineStart > MAX_LEN) {
//        return nullptr;
//    }
//    return {lineStart, lineEnd};
//}

inline void Lexer::error(string_view msg) {
    //throw SyntaxError(msg, inputName, line, column, getContext(currentPtr));
    throw SyntaxError(msg, inputName, line, column);
}

inline bool isNewLine(char_t ch) {
    return ch == '\n' || ch == '\r';
}

inline bool isSpace(char_t ch) {
    return ch == ' '  || ch == '\t' ||
           ch == '\f' || ch == '\v';
}

template<int base = 10>
inline bool isDigit(char_t ch) {
    static_assert(base == 8 || base == 10 || base == 16, "incorrect base");
    if (base == 16) {
        return isxdigit(ch) != 0;
    } else {
        return ch >= '0' && ch < '0' + base;
    }
}

inline bool isAlphaOr_(char_t ch) {
    return (ch >= 'A' && ch <= 'Z') ||
           (ch >= 'a' && ch <= 'z') ||
            ch == '_';
}

inline bool isAlphaNumOr_(char_t ch) {
    return isAlphaOr_(ch) || isDigit(ch);
}

Token::Type Lexer::lex() {
    // spaces are common inbetween tokens
    while (current() == ' ')
        next();

    for (;;) {
        switch (auto ch = current()) {
        case 0:
            return TK::EOS;

        // ignore whitespaces
        case ' ': case '\t': case '\f': case '\v':
            while (isSpace(next()))
                ;
            continue;

        case '\n': case '\r':
            readNewLine();
            return TK::NL;

        case ';':
            next();
            return TK::NL;

         case '\"': case '\'':
            readString();
            return TK::STRING;

        case '!':
            if (peekNext() == '=') {
                next(2);
                return TK::NE;
            } else {
                error("unknown token \'!\'");
            } 

        case '=':
            next();
            if (eat('='))
                return TK::EQ;
            else
                return TK::ASSIGN;

        case '<':
            next();
            if (eat('='))
                return TK::LE;
            else if (eat('>'))
                return TK::NE;
            else
                return TK::LT;

        case '>':
            next();
            if (eat('='))
                return TK::GE;
            else
                return TK::GT;

        // ignore comments until EOL
        case '#':
            do {
                next();
                if (eat('\0'))
                    return TK::EOS;
            } while (!isNewLine(current()));
            readNewLine();
            return TK::NL;

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            return readNumeral();

        case '.':
            if (!isDigit(peekNext())) {
                next();
                return TK::DOT;
            } else {
                readReal(currentPtr);
                return TK::REAL;
            }

        case '+': case '-': case '*': case '/':
        case '%': case '^': case '|': case '(':
        case ')': case '{': case '}': case '[':
        case ']': case ',':
            next();
            return static_cast<Token::Type>(ch);

        default:
            if (isAlphaOr_(ch)) {
                return readIdentOrKeyword();
            } else {
                error("unknown token: \'"s + ch + '\'');
            }
        }
    }
}

void Lexer::readNewLine() {
    assert(isNewLine(current()));

    char prev = current();
    next();
    if (prev == '\r') {
        eat('\n');
    }
    ++line;
    column = 1;
}

inline bool isExponent(char_t ch) {
    return ch == 'e' || ch == 'E';
}

Token::Type Lexer::readNumeral() {
    assert(isDigit(current()));

    const char* start = currentPtr;
    if (eat('0')) {
        // "0."
        if (current() == '.') {
            if (isDigit(peekNext())) {
                semInfo = readReal(start);
                return TK::REAL;
            } else {
                semInfo = uint_t(0);
                return TK::INT;
            }
        // 0e" "0E"
        } else if (isExponent(current())) {
            semInfo = readReal(start);
            return TK::REAL;
        // octal literal
        } else if (eat('x') || eat('X')) {
            if (!isDigit<16>(current()))
                error("malformed hexadecimal literal");
            semInfo = readInteger<16>();
            return TK::INT;
        // hexadecimal literal
        } else {
            semInfo = readInteger<8>();
            return TK::INT;
        }
    //decimal
    } else {
        semInfo = readInteger<10>();

        if (current() == '.' || isExponent(current())) {
            semInfo = readReal(start);
            return TK::REAL;
        } else {
            return TK::INT;
        }
    }
}

template<int base=10>
uint_t charDigitToInt(char_t ch) {
    static_assert(base == 8 || base == 10 || base == 16, "incorrect base");
    if (base == 16) {
        return (ch > '9') ? (ch & ~0x20) - 'A' + 10 : ch - '0';
    } else {
        return ch - '0';
    }
};

template<int base>
uint_t Lexer::readInteger() {
    static_assert(base == 8 || base == 10 || base == 16, "incorrect base");

    uint_t res = 0;
    while (isDigit<base>(current())) {
        uint_t oldRes = res;
        res = base * res + charDigitToInt<base>(current());
		constexpr auto Max_Int = static_cast<uint_t>(std::numeric_limits<int_t>::max());
        // wrap around or number bigger than max_int + 1 (it can be actually min_int if is's preceded by unary minus)
        if (oldRes != res / base || res - 1 > Max_Int)
            error("integer constant too big");
        next();
    }

    if (base == 8 && (isDigit<16>(current()) || current() == '.')) {
        error("malformed octal literal");
    }
    return res;
}

real_t Lexer::readReal(const char_t* start) {
    assert(current() == '.' || isExponent(current()));
    assert(*start != '.' || !isDigit(start[1]));

    if (current() == '.')
        next();

    while (!isExponent(current()) && isDigit(current()))
        next();

    if (isExponent(current())) {
        next();
        if (current() == '-' || current() == '+')
            next();
        if (!isDigit(current()))
            error("malformed real literal");
    }

    while (isDigit(current()))
        next();

    const char* end = currentPtr;

    return boost::lexical_cast<real_t>(start, end - start);
}

Token::Type Lexer::readIdentOrKeyword() {
    assert(isAlphaOr_(current()));

    const char* start = currentPtr;

    int64_t intRepr = 0;
    do {
        intRepr <<= 8;
        intRepr |= current();
    } while (isAlphaOr_(next()));

    // TODO keyword check

    // '!' and '?' are allowed as the last character in identifier
    if (current() == '!' || current() == '?')
        next();

    size_t len = currentPtr - start;
    semInfo = string_view{start, len};

    return TK::IDENT; //TODO
}

void Lexer::unescape() {
    assert(current() == '\\');

    next();

    switch (current()) {
    case '\0': error("unterminated string literal");
    case 'a':  tmpBuffer.push_back('\a'); break;
    case 'b':  tmpBuffer.push_back('\b'); break;
    case 'f':  tmpBuffer.push_back('\f'); break;
    case 'n':  tmpBuffer.push_back('\n'); break;
    case 'r':  tmpBuffer.push_back('\r'); break;
    case 't':  tmpBuffer.push_back('\t'); break;
    case 'v':  tmpBuffer.push_back('\v'); break;
    case '?':  tmpBuffer.push_back('\?'); break;
    case '\\': tmpBuffer.push_back('\\'); break;
    case '\'': tmpBuffer.push_back('\''); break;
    case '\"': tmpBuffer.push_back('\"'); break;
    case '\n': readNewLine(); break;
    // 2 hex digits after x
    case 'x': {
        next();
        if (!isDigit<16>(current()) || !isDigit<16>(peekNext()))
            error("invalid escape sequence");
        uint_t val = 16u * charDigitToInt<16>(current()) + charDigitToInt<16>(next());
        tmpBuffer.push_back(static_cast<char_t>(val));
        break;
    }
    // 1-3 octal digits
    case '0': case '1': case '2': case '3':
    case '4': case '5': case '6': case '7': {
        uint_t val = charDigitToInt<8>(current());
        if (isDigit<8>(peekNext())) {
            val = 8u * val + charDigitToInt<8>(next());
            if (isDigit<8>(peekNext()))
                val += 8u * val + charDigitToInt<8>(next());
        }
        if (val > std::numeric_limits<std::make_unsigned_t<char_t>>::max())
            error("invalid escape sequence");
        tmpBuffer.push_back(static_cast<char_t>(val));
        break;
    }
    default:
        error("invalid escape sequence");
    }
    next();
}

void Lexer::readString() {
    assert(current() == '\"' || current() == '\'');

    const char_t closingChar = current();
    bool longString = false;
    next();
    if (current() == closingChar) {
        next();
        // empty string
        if (current() != closingChar) {
            semInfo = string_view{};
            return;
        }
        // long string
        next();
    }

    const char_t* start = currentPtr;
    auto normalStringClosingPred = [this, closingChar]() {
        return current() != closingChar;
    };
    auto longStringClosingPred = [this, closingChar]() { 
        if (current() == closingChar &&
            peekNext() == closingChar &&
            peekNext(2) == closingChar) {
            next(2);
            return true;
        }
        return false;
    };

    bool noEscapeSeq;
    if (longString)
        noEscapeSeq = readStringNoEscapeSeq(longStringClosingPred);
    else
        noEscapeSeq = readStringNoEscapeSeq(normalStringClosingPred);

    // we can just point to the input buffer
    if (noEscapeSeq) {
        semInfo = string_view{start, size_t(currentPtr - start)};
    // there is stuff to escape, DAMN YOU USER
    } else {
        if (longString)
            readStringWithEscapeSeq(start, longStringClosingPred);
        else
            readStringWithEscapeSeq(start, normalStringClosingPred);
    }
    next();
}

template<class ClosingPred>
inline bool Lexer::readStringNoEscapeSeq(ClosingPred pred) {
    while (pred()) {
        if (current() == '\0' || isNewLine(current()))
            error("unterminated string literal");
        if (current() == '\\')
            return false;
        next();
    }
    return true;
}

template<class ClosingPred>
void Lexer::readStringWithEscapeSeq(const char_t* start, ClosingPred pred) {
    tmpBuffer.clear();
    tmpBuffer.reserve(currentPtr - start + 1);
    tmpBuffer.insert(tmpBuffer.end(), start, currentPtr);
    unescape();
    start = currentPtr;

    while (pred()) {
        if (current() == '\0' || isNewLine(current()))
            error("unterminated string literal");
        if (current() == '\\') {
            tmpBuffer.insert(tmpBuffer.end(), start, currentPtr);
            unescape();
            start = currentPtr;
        }
    }
    tmpBuffer.insert(tmpBuffer.end(), start, currentPtr);

    semInfo = string_view{tmpBuffer.data(), tmpBuffer.size()};
}

}

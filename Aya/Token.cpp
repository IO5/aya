#ifdef _DEBUG
#include "Token.hpp"

namespace aya {

string_view toString(Token::Type token) {
    static char res[2] = {};
    switch (token) {
    case TK::EOS:    return "EOS";
    case TK::NL:     return "NEWLINE";
    case TK::IDENT:  return "IDENT";
    case TK::DO:     return "DO";
    case TK::END:    return "END";
    case TK::IF:     return "IF";
    case TK::THEN:   return "THEN";
    case TK::ELSE:   return "ELSE";
    case TK::ELIF:   return "ELIF";
    case TK::WHILE:  return "WHILE";
    case TK::FOR:    return "FOR";
    case TK::IN:     return "IN";
    case TK::REPEAT: return "REPEAT";
    case TK::UNTIL:  return "UNTIL";
    case TK::RETURN: return "RETURN";
    case TK::BREAK:  return "BREAK";
    case TK::NEXT:   return "NEXT";
    case TK::LOCAL:  return "LOCAL";
    case TK::DEF:    return "DEF";
    case TK::CLASS:  return "CLASS";
    case TK::NIL:    return "NIL";
    case TK::TRUE:   return "TRUE";
    case TK::FALSE:  return "FALSE";
    case TK::INT:    return "INT";
    case TK::REAL:   return "REAL";
    case TK::STRING: return "STRING";
    case TK::OR:     return "OR";
    case TK::AND:    return "AND";
    case TK::NOT:    return "NOT";
    case TK::LT:     return "<";
    case TK::LE:     return "<=";
    case TK::GT:     return ">";
    case TK::GE:     return ">=";
    case TK::EQ:     return "==";
    case TK::NE:     return "!=";

    case '+'_tk: case '-'_tk: case '*'_tk: case '/'_tk: case '%'_tk: case '^'_tk: case '|'_tk:
    case '='_tk: case ','_tk: case '.'_tk:
    case '['_tk: case ']'_tk: case '{'_tk: case '}'_tk: case '('_tk: case ')'_tk:
        res[0] = static_cast<char>(token);
        return res;

    default:
        return "UNKNOWN";
    }
}

}
#endif
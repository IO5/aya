#include "Parser.hpp"

namespace aya {

Parser::Parser() {}


Parser::~Parser() {}

AstNode* parse() {
    /*
    start -> block EOF

    block -> e | stat (NL stat)*

    stat -> expr
    stat -> if expr then block end
    stat -> var (, var)* = expr (, expr)*
    expr -> simpleExpr
    simpleExpr -> literal | var | functionCall 
    functionCall -> prefixExpr '(' {expr , } ')'
    */
    return nullptr;
}
/*
bool block() {
    while (current == TK::NL)
        next();

    if (!statement())
        return true;

    do {
        if (current != TK::NL)
            break;
        while (current == TK::NL)
            next();
    } while (statement());
    return true;
}

bool statement() {
    switch (current) {
    case TK::IF: return ifStat();
    case TK::WHILE: return whileStat();
    case TK::REPEAT: return repeatStat();
    case TK::FOR: return forStat();
    case TK::BREAK: return breakStat();
        ...
    default: return assignmentOrExpr();
    }
}

bool terminal(Token tk) {
    //if (current == tk) {
    //    next();
    //    return true;
    //}
    //return false;
}
*/
}
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
    expr -> var (, var)* = expr (, expr)*
    expr -> simpleExpr
    simpleExpr -> literal | var | functionCall 
    functionCall -> prefixExpr '(' {expr , } ')'
    */
    return nullptr;
}

}
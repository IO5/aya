#include "Parser.hpp"

namespace aya {

// TODO
Parser::Parser() : current(TK::EOS, Nil(), 0, 0) {}

Parser::~Parser() {}

template <class T, typename... Args>
inline unique_ptr<T> Parser::make(Args... args) {
    // TODO custom alloc
	return nullptr;//std::make_unique<T>(args...);
}

Token& Parser::next() {
    // TODO
    return current;
}

unique_ptr<AstNode> Parser::parse() {
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

unique_ptr<BlockNode> Parser::block()
{
    while (current == TK::NL)
        next();

	auto firstStat = statement();
    if (!firstStat)
        return nullptr;

	// TODO custom alloc
	BlockNode::StatementList statements;
	statements.emplace_back(std::move(firstStat));

    while (true) {
        if (current != TK::NL)
            break;
        while (current == TK::NL)
            next();
		if (auto stat = statement()) {
			statements.emplace_back(std::move(stat));
		} else {
			break;
		}
    }
    return make<BlockNode>(std::move(statements));
}

unique_ptr<StatementNode> Parser::statement() {
    switch (current) {
    //case TK::IF: return ifStat();
    //case TK::WHILE: return whileStat();
    //case TK::REPEAT: return repeatStat();
    //case TK::FOR: return forStat();
    //case TK::BREAK: return breakStat();
    //    ...
    //default: return assignmentOrExpr();
    }
	return nullptr;
}
/*
unique_ptr<IntNode> Parser::intLiteral() {
    if (current == TK::INT) {
        next();
        // TODO assert val < int_max
        return make<IntNode>(current.getInt());
    }
    return nullptr;
}

unique_ptr<RealNode> Parser::realLiteral() {
    if (current == TK::REAL) {
        next();
        return make<RealNode>(current.getReal());
    }
    return nullptr;
}

unique_ptr<StringNode> Parser::stringLiteral() {
    if (current == TK::STRING) {
        next();
		return make<StringNode>(current.getString());
    }
    return nullptr;
}
*/
}
#pragma once

#include "Token.hpp"
#include "Utils.hpp"
#include "ast/AstNode.hpp"

#include <algorithm>
#include <memory>
#include <optional>

namespace aya {

class AstNode;

class Parser {
public:
    Parser();
    ~Parser();

    unique_ptr<AstNode> parse();
private:
    template <class T, typename... Args>
    unique_ptr<T> make(Args... args);

    Token& next();
    
    unique_ptr<BlockNode> block();
    unique_ptr<StatementNode> statement();

    Token current;
};

}
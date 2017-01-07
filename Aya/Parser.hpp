#pragma once

#include "Token.hpp"

#include <algorithm>

namespace aya {
class AstNode;

class Parser {
public:
    Parser();
    ~Parser();

    AstNode* parse();
private:
    typedef std::pair<bool, AstNode*> ParseRes;
};

}
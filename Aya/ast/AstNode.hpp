#pragma once

#include "../Utils.hpp"
#include "../Exception.hpp"

#include <vector>

namespace aya {

class AstNode {
public:
    ~AstNode() {}
    virtual void genCode() = 0;
};

class StatementNode : public AstNode {};

class BlockNode : public StatementNode {
public:
    using StatementList = std::vector<unique_ptr<StatementNode>>;
    
    BlockNode(StatementList&& statements_) : statements(std::move(statements_)) {}

private:
    std::vector<unique_ptr<StatementNode>> statements;
};

class ExprNode : public AstNode {
protected:
    virtual bool isConst() = 0;
};

class IntNode : public ExprNode {
public:
    IntNode(int_t val_) : val(val_) {}
    void genCode() override {}

private:
    bool isConst() override { return true; }

    int_t val;
};

class StringNode : public ExprNode {
public:
    StringNode(string_view sv) : view(sv) {}
    StringNode(string_t&& s) : copy(s), view(copy) {}
    void genCode() override {}

private:
    bool isConst() override { return true; }

    string_view view;
    string_t copy;
};


class RealNode : public ExprNode {
public:
    RealNode(real_t val_) : val(val_) {}
    void genCode() override {}

private:
    bool isConst() override { return true; }

    real_t val;
};

template<class Op>
class BinOpNode : public ExprNode {
public:
    BinOpNode(nn_unique_ptr<ExprNode> left_, nn_unique_ptr<ExprNode> right_) :
        left(left_), right(right_) {}
private:
    bool isConst() override {
        return left->isConst() && right->isConst();
    }

    nn_unique_ptr<ExprNode> left;
    nn_unique_ptr<ExprNode> right;
};

}
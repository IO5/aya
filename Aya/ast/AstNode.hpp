#pragma once
#include <cassert>
#include <memory>

class AstNode {
public:
    ~AstNode() {}
    virtual void genCode() =0;
};

class ExprNode : public AstNode {
protected:
    virtual bool isConst() =0;
};

template<class T>
class LiteralNode : public ExprNode {
public:
    LiteralNode(const T& val_) : val(val_) {}
private:
    override bool isConst() { return true; }

    T val;
};

template<class Op>
class BinOpNode : public ExprNode {
public:
    BinOpNode(Op op, std::unique_ptr<ExprNode> left_, std::unique_ptr<ExprNode> right_) : 
    left(left_), right(right_) {}
private:
    override bool isConst() {
        return left->isConst() && right->isConst();
    }

    Op op;
    std::unique_ptr<ExprNode> left;
    std::unique_ptr<ExprNode> right;
};
#pragma once

#include <gsl>
#include <string>
#include <exception>

#include "Types.hpp"

namespace aya {

class Exception : public std::exception {
public:
    explicit Exception(gsl::cstring_span<> msg_)
        : msg(gsl::to_string(msg_)) {}

    char const* what() const override {
        return msg.c_str();
    }

protected:
    const std::string msg;
};

class SyntaxError : public Exception {
public:
    const string_t inputName;
    const size_t line, column;
    const string_t context;

    SyntaxError(gsl::cstring_span<> msg_, string_view inputName_, size_t line_, size_t column_, string_view context_ = nullptr) :
        Exception(msg_),
        inputName(gsl::to_string(inputName_)),
        line(line_), 
        column(column_),
        context(gsl::to_string(context_)) {}
    SyntaxError(SyntaxError&& other) = default;
    SyntaxError(const SyntaxError& other) :
        Exception(other.msg),
        inputName(other.inputName),
        line(other.line), 
        column(other.column),
        context(other.context) {}

private:
};

}
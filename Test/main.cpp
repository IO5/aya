#include <limits>
#include <iostream>
#include <cmath>

//#include "Lexer.hpp"
//#include "Exception.hpp"

using namespace std;
//using namespace aya;
/*
#include "gtest/gtest.h"


TEST(inputBufferSanity, lexerTest) {
    EXPECT_THROW(Lexer lexer(nullptr), Exception);
    EXPECT_THROW(Lexer lexer({}), Exception);
    EXPECT_THROW(Lexer lexer(gsl::span<const char_t>{"", 0}), Exception);
    EXPECT_THROW(Lexer lexer(gsl::span<const char_t>{"abc", 3}), Exception);

    EXPECT_NO_THROW(Lexer lexer(""));
    EXPECT_NO_THROW(Lexer lexer("abc"));
}

TEST(emptyAndIgnoredInput, lexerTest) {
    EXPECT_EQ(Lexer("").lex(), TK::EOS);
    EXPECT_EQ(Lexer("  \v  \t   \v\t ").lex(), TK::EOS);
    EXPECT_EQ(Lexer("# comment only ").lex(), TK::EOS);
}

TEST(basicIntegers, lexerTest) {
    const char input[] = "0 1 12 23456 "
                   "0 00 01 0012 023456 0000 "
                   "0x0 0X1 0xa9 0xabcdef 0X0fAb001";
    Lexer lexer(input);
    stringstream ss(input);
    ss << setbase(0);
    int_t ssInt;
    while (ss >> ssInt) {
        EXPECT_EQ(lexer.lex(), TK::INT);
        EXPECT_EQ(lexer.getSemInfo().getInt(), ssInt);
    }
    ASSERT_EQ(lexer.lex(), TK::EOS);
}

TEST(invalidIntegers, lexerTest) {
    EXPECT_THROW(Lexer("99999999999999999").lex(), SyntaxError);
    EXPECT_THROW(Lexer("08").lex(), SyntaxError);
    EXPECT_THROW(Lexer("0a").lex(), SyntaxError);
    EXPECT_THROW(Lexer("00709").lex(), SyntaxError);
    EXPECT_THROW(Lexer("0x").lex(), SyntaxError);
    EXPECT_THROW(Lexer("0X").lex(), SyntaxError);
    EXPECT_THROW(Lexer("0xg").lex(), SyntaxError);
}

TEST(basicReals, lexerTest) {
    const char input[] = "0.0 1.0 0.5 0.0625 2.5 1024.03125 ";
                         "0e0 0e10 1e1 5E0 13e-0 1e10 23e4 7E-3 666e+1 "
                         "0.0e-0 0.0E+0 2.5e-1 23.57e11 42.88E-7 "
                         "0.e0 0.E3 3.E0 3.e3";
    Lexer lexer(input);
    stringstream ss(input);
    ss << setbase(0);
    real_t ssReal;
    while (ss >> ssReal) {
        EXPECT_EQ(lexer.lex(), TK::REAL);
        EXPECT_EQ(lexer.getSemInfo().getReal(), ssReal);
    }
    ASSERT_EQ(lexer.lex(), TK::EOS);
}

TEST(nanReal, lexerTest) {
    Lexer lexer("nan NaN NAN");
    for (int i = 0; i < 3; ++i) {
        EXPECT_EQ(lexer.lex(), TK::REAL);
        EXPECT_TRUE(isnan(lexer.getSemInfo().getReal()));
    }
    ASSERT_EQ(lexer.lex(), TK::EOS);
}

TEST(infReal, lexerTest) {
    Lexer lexer("inf Inf INF");
    for (int i = 0; i < 3; ++i) {
        EXPECT_EQ(lexer.lex(), TK::REAL);
        EXPECT_EQ(lexer.getSemInfo().getReal(), numeric_limits<real_t>::infinity());
    }
    ASSERT_EQ(lexer.lex(), TK::EOS);
}

TEST(invalidReals, lexerTest) {
    EXPECT_THROW(Lexer("0.E+").lex(), SyntaxError);
    EXPECT_THROW(Lexer("1.e-").lex(), SyntaxError);
    EXPECT_THROW(Lexer("2.e+X").lex(), SyntaxError);
    EXPECT_THROW(Lexer("3.E+-").lex(), SyntaxError);
    EXPECT_THROW(Lexer("3e+").lex(), SyntaxError);
    EXPECT_THROW(Lexer("0.5E-abc").lex(), SyntaxError);
}

TEST(instSeparators, lexerTest) {
    Lexer lexer("\n ; \n\r \r\n");
    for (int i = 0; i < 4; ++i)
        EXPECT_EQ(lexer.lex(), TK::NL);
    ASSERT_EQ(lexer.lex(), TK::EOS);
}

TEST(basicTokens, lexerTest) {
    const char input[] = "+-/*%^|=.,(){}[]"
                         "== != <> <= < > >=";
    Lexer lexer(input);
    Token tokens[] = {
        TK::PLUS, TK::MINUS, TK::DIV, TK::MUL, TK::MOD, TK::EXP, TK::BAR,
        TK::ASSIGN, TK::DOT, TK::COMMA,
        TK::PAREN_L, TK::PAREN_R, TK::CRBR_L, TK::CRBR_R, TK::SQBR_L, TK::SQBR_R,
        TK::EQ, TK::NE, TK::NE, TK::LE, TK::LT, TK::GT, TK::GE,
        TK::EOS
    };
    for (auto token : tokens) {
        EXPECT_EQ(lexer.lex(), token);
    }
}

TEST(badTokens, lexerTest) {
    EXPECT_THROW(Lexer("!").lex(), SyntaxError);
    EXPECT_THROW(Lexer("!a").lex(), SyntaxError);
    EXPECT_THROW(Lexer("~").lex(), SyntaxError);
    EXPECT_THROW(Lexer("@").lex(), SyntaxError);
    EXPECT_THROW(Lexer("$").lex(), SyntaxError);
    EXPECT_THROW(Lexer("&").lex(), SyntaxError);
    EXPECT_THROW(Lexer("\\").lex(), SyntaxError);
    EXPECT_THROW(Lexer(":").lex(), SyntaxError);
    EXPECT_THROW(Lexer("\xff").lex(), SyntaxError);
}
*/
#include "Value.hpp"

#include <functional>
#include <vector>
#include <tuple>
//#include <boost/hana/functional/overload.hpp>

namespace aya {

template <typename Op>
struct BinOpImpl {
    template <typename L, typename R>
    auto operator()(const L& lhs, const R& rhs) ->
        std::enable_if_t<std::is_arithmetic_v<L> && std::is_arithmetic_v<R>, Value> {

        return op(lhs, rhs);
    }
    template <typename L, typename R>
    auto operator()(const L& lhs, const R& rhs) ->
        std::enable_if_t<!std::is_arithmetic_v<L> || !std::is_arithmetic_v<R>, Value> {

        throw 1;
    }
    BinOpImpl(const Op& op) : op(op) {}
    const Op& op;
};

//template <typename Op, typename... SelectedTypes>
//Value binOp(Value lhs, Value rhs, const Op& op) {
//    std::visit([](auto&& arg) {
//        throw 1;
//    }, lhs.v);
//}

template <typename... Types>
struct UnpackStackImpl;

template <typename Head, typename... Tail>
struct UnpackStackImpl<Head, Tail...> {
    static auto unpackStack(const gsl::span<Value>& stack) {
        if (Head* val = stack[0].getIf<Head>()) {
            return std::tuple_cat(
                std::tuple<Head>(*val),
                UnpackStackImpl<Tail...>::unpackStack(stack.subspan(1, stack.size() - 1))
            );
        } else {
            throw 1;
        }
    }
};

template <>
struct UnpackStackImpl<> {
    static auto unpackStack(const gsl::span<Value>&) {
        return std::tuple<>();
    }
};

template <typename... Types>
auto unpackStack(const gsl::span<Value>& stack) {
    return UnpackStackImpl<Types...>::unpackStack(stack);
}

//std::function<int(std::vector<aya::Value>&)>
template <typename Ret, typename... Args, typename Func>
NotNull<CFunction*> makeBind(Func&& func) {
    return new CFunction([f = std::forward<Func>(func)](gsl::span<Value>& stack) {
        assert(stack.size() == sizeof...(Args));
        auto args = unpackStack<Args...>(stack);
        //stack[0].get
        try {
            std::apply(f, args);
        } catch (...) {

        }
        return 1;
    });
}

}

void foo(aya::int_t, double, bool) {
    std::cout << "hi\n";
}

int main(int argc, char** argv) 
{
    (void)argc; (void)argv;
    using namespace aya;
    //testing::InitGoogleTest(&argc, argv); 
    //return RUN_ALL_TESTS();
    const aya::Value a = 2, b = 0.5;
    const Value c = aya::makeBind<void, aya::int_t, double, bool>(&foo);
    auto* dupa = c.getIf<CFunction>();
    std::array<Value, 3> stack{2,0.0,true};
    (*dupa)(gsl::span<Value>{stack});
    //auto dupa = a.isOneOf<Nil>();
    //aya::Value c = aya::binOp([](auto&& lhs, auto&& rhs) { return rhs + lhs; }, a, b);
    //aya::Value d = aya::Nil();
    //std::cout << toString(d) << '\n';
    //auto sub = c.getSubvariant<aya::int_t, aya::real_t>();
    //std::cout << aya::typeToString(b) << '\n';
    //std::visit([](auto&& val) { std::cout << "a" << '\n'; }, v.v);
    
    //aya::CFunction func = aya::makeBind<void, aya::int_t, double, bool, aya::Nil>(foo);
    //std::vector<aya::Value> stack = { 23, 5.0, true, aya::Nil() };
    //func(gsl::span<aya::Value>{stack.data(), (int)stack.size()});
}

/*
int main2(int argc, const char* argv[]) {
    if (argc != 2)
        return -1;

    ifstream inputFile(argv[1]);
    if (!inputFile.good())
        return -1;

    vector<char> source;
    copy(istreambuf_iterator<char>(inputFile), istreambuf_iterator<char>(), back_inserter(source));
    source.push_back('\0');

    aya::Lexer lexer(source);

    try {
        aya::Token current = lexer.lex();
        while (current != aya::TK::EOS) {
            cout << aya::toString(current);
            if (current == aya::TK::INT) {
                cout << '(' << lexer.getSemInfo().getInt() << ')';
            } else if (current == aya::TK::REAL) {
                cout << '(' << lexer.getSemInfo().getReal() << ')';
            }
            cout << ' ';
            current = lexer.lex();
        }
    } catch (aya::SyntaxError& err) {
        cout << '\n';
        cerr << err.what() << " in " << err.inputName << ':' << err.line << '\n';
        cerr << err.context <<'\n';
    }

    cout << '\n';
    return 0;
}
*/
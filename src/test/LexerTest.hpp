#pragma once

#include "Lexer.hpp"
#include "Exception.hpp"

#include <gtest/gtest.h>
#include <sstream>
#include <cmath>
#include <limits>

using namespace aya;

TEST(lexerTest, inputBufferSanity) {
    EXPECT_THROW(Lexer lexer(nullptr), Exception);
    EXPECT_THROW(Lexer lexer({}), Exception);
    EXPECT_THROW(Lexer lexer({"abc", int(0)}), Exception);
    EXPECT_THROW(Lexer lexer({"abc", 3}), Exception);

    EXPECT_NO_THROW(Lexer lexer(""));
    EXPECT_NO_THROW(Lexer lexer("abc"));
}

TEST(lexerTest, emptyAndIgnoredInput) {
    EXPECT_EQ(Lexer("").nextToken(), TK::EOS);
    EXPECT_EQ(Lexer("  \v  \t   \v\t ").nextToken(), TK::EOS);
    EXPECT_EQ(Lexer("# comment only ").nextToken(), TK::EOS);
}

TEST(lexerTest, basicIntegers) {
    const char input[] = "0 1 12 23456 "
                   "0 00 01 0012 023456 0000 "
                   "0x0 0X1 0xa9 0xabcdef 0X0fAb001";
    Lexer lexer(input);
    std::stringstream ss(input);
    ss << std::setbase(0);
    int_t ssInt;
    int i = 1;
    while (ss >> ssInt) {
        auto token = lexer.nextToken();
        ASSERT_EQ(token, TK::INT) << "failed on token no. " << i;
        EXPECT_EQ(token.getInt(), ssInt);
        ++i;
    }
    ASSERT_EQ(lexer.nextToken(), TK::EOS);
}

TEST(lexerTest, invalidIntegers) {
    EXPECT_THROW(Lexer("99999999999999999").nextToken(), SyntaxError);
    EXPECT_THROW(Lexer("08").nextToken(),    SyntaxError);
    EXPECT_THROW(Lexer("0a").nextToken(),    SyntaxError);
    EXPECT_THROW(Lexer("00709").nextToken(), SyntaxError);
    EXPECT_THROW(Lexer("0x").nextToken(),    SyntaxError);
    EXPECT_THROW(Lexer("0X").nextToken(),    SyntaxError);
    EXPECT_THROW(Lexer("0xg").nextToken(),   SyntaxError);
    EXPECT_THROW(Lexer("0x 23").nextToken(), SyntaxError);
}

TEST(lexerTest, basicReals) {
    const char input[] = "0.0 1.0 0.5 0.0625 2.5 1024.03125 "
                         "0e0 0e10 1e1 5E0 13e-0 1e10 23e4 7E-3 666e+1 "
                         "0.0e-0 0.0E+0 2.5e-1 23.57e11 42.88E-7";
                        // "0.e0 0.E3 3.E0 3.e3";
    Lexer lexer(input);
    std::stringstream ss(input);
    ss << std::setbase(0);
    real_t ssReal;
    int i = 1;
    while (ss >> ssReal) {
        auto token = lexer.nextToken();
        ASSERT_EQ(token, TK::REAL) << "failed on token no. " << i;
        EXPECT_EQ(token.getReal(), ssReal);
        ++i;
    }
    ASSERT_EQ(lexer.nextToken(), TK::EOS);
}

// stringstream won't parse those
TEST(lexerTest, problematicReals) {
    const char input[] = "0.e0 0.E3 3.E0 3.e3";
    Lexer lexer(input);

    auto token = lexer.nextToken();
    ASSERT_EQ(token, TK::REAL);
    EXPECT_EQ(token.getReal(), 0.0);
    token = lexer.nextToken();
    ASSERT_EQ(token, TK::REAL);
    EXPECT_EQ(token.getReal(), 0.0);
    token = lexer.nextToken();
    ASSERT_EQ(token, TK::REAL);
    EXPECT_EQ(token.getReal(), 3.0);
    token = lexer.nextToken();
    ASSERT_EQ(token, TK::REAL);
    EXPECT_EQ(token.getReal(), 3000.0);

    ASSERT_EQ(lexer.nextToken(), TK::EOS);
}

TEST(lexerTest, nanReal) {
    Lexer lexer("nan NaN NAN");
    for (int i = 0; i < 3; ++i) {
        auto token = lexer.nextToken();
        ASSERT_EQ(token, TK::REAL);
        EXPECT_TRUE(std::isnan(token.getReal()));
    }
    ASSERT_EQ(lexer.nextToken(), TK::EOS);
}

TEST(lexerTest, infReal) {
    Lexer lexer("inf Inf INF");
    for (int i = 0; i < 3; ++i) {
        auto token = lexer.nextToken();
        ASSERT_EQ(token, TK::REAL);
        EXPECT_EQ(token.getReal(), std::numeric_limits<real_t>::infinity());
    }
    ASSERT_EQ(lexer.nextToken(), TK::EOS);
}

TEST(lexerTest, invalidReals) {
    EXPECT_THROW(Lexer("0.E+").nextToken(),     SyntaxError);
    EXPECT_THROW(Lexer("1.e-").nextToken(),     SyntaxError);
    EXPECT_THROW(Lexer("2.e+X").nextToken(),    SyntaxError);
    EXPECT_THROW(Lexer("3.E+-").nextToken(),    SyntaxError);
    EXPECT_THROW(Lexer("3e+").nextToken(),      SyntaxError);
    EXPECT_THROW(Lexer("0.5E-abc").nextToken(), SyntaxError);
}

TEST(lexerTest, instSeparators) {
    Lexer lexer("\n ; \n\r \r\n");
    for (int i = 0; i < 4; ++i)
        EXPECT_EQ(lexer.nextToken(), TK::NL);

    ASSERT_EQ(lexer.nextToken(), TK::EOS);
}

TEST(lexerTest, basicTokens) {
    const char input[] = "+ - / * % ^ |"
                         "= . ,"
                         "() {} []"
                         "== != <> <= < > >=";
    Lexer lexer(input);
    Token::Type tokens[] = {
        TK::PLUS, TK::MINUS, TK::DIV, TK::MUL, TK::MOD, TK::EXP, TK::BAR,
        TK::ASSIGN, TK::DOT, TK::COMMA,
        TK::PAREN_L, TK::PAREN_R, TK::CRBR_L, TK::CRBR_R, TK::SQBR_L, TK::SQBR_R,
        TK::EQ, TK::NE, TK::NE, TK::LE, TK::LT, TK::GT, TK::GE,
        TK::EOS
    };
    for (auto token : tokens) {
        EXPECT_EQ(lexer.nextToken(), token);
    }
}

TEST(lexerTest, badTokens) {
    EXPECT_THROW(Lexer("!").nextToken(),    SyntaxError);
    EXPECT_THROW(Lexer("!a").nextToken(),   SyntaxError);
    EXPECT_THROW(Lexer("~").nextToken(),    SyntaxError);
    EXPECT_THROW(Lexer("@").nextToken(),    SyntaxError);
    EXPECT_THROW(Lexer("$").nextToken(),    SyntaxError);
    EXPECT_THROW(Lexer("&").nextToken(),    SyntaxError);
    EXPECT_THROW(Lexer("\\").nextToken(),   SyntaxError);
    EXPECT_THROW(Lexer(":").nextToken(),    SyntaxError);
    EXPECT_THROW(Lexer("\xff").nextToken(), SyntaxError);
}


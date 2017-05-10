#pragma once

#include "Lexer.hpp"
#include "Exception.hpp"

#include <gtest/gtest.h>
#include <sstream>

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
    while (ss >> ssInt) {
        auto token = lexer.nextToken();
        ASSERT_EQ(token, TK::INT);
        EXPECT_EQ(token.getInt(), ssInt);
    }
    ASSERT_EQ(lexer.nextToken(), TK::EOS);
}

//TEST(invalidIntegers, lexerTest) {
    //EXPECT_THROW(Lexer("99999999999999999").nextToken(), SyntaxError);
    //EXPECT_THROW(Lexer("08").nextToken(), SyntaxError);
    //EXPECT_THROW(Lexer("0a").nextToken(), SyntaxError);
    //EXPECT_THROW(Lexer("00709").nextToken(), SyntaxError);
    //EXPECT_THROW(Lexer("0x").nextToken(), SyntaxError);
    //EXPECT_THROW(Lexer("0X").nextToken(), SyntaxError);
    //EXPECT_THROW(Lexer("0xg").nextToken(), SyntaxError);
//}

//TEST(basicReals, lexerTest) {
    //const char input[] = "0.0 1.0 0.5 0.0625 2.5 1024.03125 ";
                         //"0e0 0e10 1e1 5E0 13e-0 1e10 23e4 7E-3 666e+1 "
                         //"0.0e-0 0.0E+0 2.5e-1 23.57e11 42.88E-7 "
                         //"0.e0 0.E3 3.E0 3.e3";
    //Lexer lexer(input);
    //stringstream ss(input);
    //ss << setbase(0);
    //real_t ssReal;
    //while (ss >> ssReal) {
        //EXPECT_EQ(lexer.nextToken(), TK::REAL);
        //EXPECT_EQ(lexer.getSemInfo().getReal(), ssReal);
    //}
    //ASSERT_EQ(lexer.nextToken(), TK::EOS);
//}

//TEST(nanReal, lexerTest) {
    //Lexer lexer("nan NaN NAN");
    //for (int i = 0; i < 3; ++i) {
        //EXPECT_EQ(lexer.nextToken(), TK::REAL);
        //EXPECT_TRUE(isnan(lexer.getSemInfo().getReal()));
    //}
    //ASSERT_EQ(lexer.nextToken(), TK::EOS);
//}

//TEST(infReal, lexerTest) {
    //Lexer lexer("inf Inf INF");
    //for (int i = 0; i < 3; ++i) {
        //EXPECT_EQ(lexer.nextToken(), TK::REAL);
        //EXPECT_EQ(lexer.getSemInfo().getReal(), numeric_limits<real_t>::infinity());
    //}
    //ASSERT_EQ(lexer.nextToken(), TK::EOS);
//}

//TEST(invalidReals, lexerTest) {
    //EXPECT_THROW(Lexer("0.E+").nextToken(), SyntaxError);
    //EXPECT_THROW(Lexer("1.e-").nextToken(), SyntaxError);
    //EXPECT_THROW(Lexer("2.e+X").nextToken(), SyntaxError);
    //EXPECT_THROW(Lexer("3.E+-").nextToken(), SyntaxError);
    //EXPECT_THROW(Lexer("3e+").nextToken(), SyntaxError);
    //EXPECT_THROW(Lexer("0.5E-abc").nextToken(), SyntaxError);
//}

//TEST(instSeparators, lexerTest) {
    //Lexer lexer("\n ; \n\r \r\n");
    //for (int i = 0; i < 4; ++i)
        //EXPECT_EQ(lexer.nextToken(), TK::NL);
    //ASSERT_EQ(lexer.nextToken(), TK::EOS);
//}

//TEST(basicTokens, lexerTest) {
    //const char input[] = "+-/*%^|=.,(){}[]"
                         //"== != <> <= < > >=";
    //Lexer lexer(input);
    //Token tokens[] = {
        //TK::PLUS, TK::MINUS, TK::DIV, TK::MUL, TK::MOD, TK::EXP, TK::BAR,
        //TK::ASSIGN, TK::DOT, TK::COMMA,
        //TK::PAREN_L, TK::PAREN_R, TK::CRBR_L, TK::CRBR_R, TK::SQBR_L, TK::SQBR_R,
        //TK::EQ, TK::NE, TK::NE, TK::LE, TK::LT, TK::GT, TK::GE,
        //TK::EOS
    //};
    //for (auto token : tokens) {
        //EXPECT_EQ(lexer.nextToken(), token);
    //}
//}

//TEST(badTokens, lexerTest) {
    //EXPECT_THROW(Lexer("!").nextToken(),    SyntaxError);
    //EXPECT_THROW(Lexer("!a").nextToken(),   SyntaxError);
    //EXPECT_THROW(Lexer("~").nextToken(),    SyntaxError);
    //EXPECT_THROW(Lexer("@").nextToken(),    SyntaxError);
    //EXPECT_THROW(Lexer("$").nextToken(),    SyntaxError);
    //EXPECT_THROW(Lexer("&").nextToken(),    SyntaxError);
    //EXPECT_THROW(Lexer("\\").nextToken(),   SyntaxError);
    //EXPECT_THROW(Lexer(":").nextToken(),    SyntaxError);
    //EXPECT_THROW(Lexer("\xff").nextToken(), SyntaxError);
//}


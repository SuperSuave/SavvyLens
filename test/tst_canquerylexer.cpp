#include "tst_canquerylexer.h"
#include "src/query/canquerylexer.h"

// Qt headers
#include <QtTest>

using namespace CanQuery;

void TestCanQueryLexer::tokenizeSimpleIdFilter()
{
    Lexer lexer(QStringLiteral("id:0x123"));
    QVector<Token> tokens = lexer.tokenizeAll();

    QCOMPARE(tokens.size(), 4); // Identifier, Colon, Number, EndOfInput
    QCOMPARE(tokens[0].type, TokenType::Identifier);
    QCOMPARE(tokens[0].text, QStringLiteral("id"));

    QCOMPARE(tokens[1].type, TokenType::Colon);

    QCOMPARE(tokens[2].type, TokenType::Number);
    QCOMPARE(tokens[2].numericValue, 0x123ULL);

    QCOMPARE(tokens[3].type, TokenType::EndOfInput);
    QVERIFY(!lexer.hasError());
}

void TestCanQueryLexer::tokenizeIdRange()
{
    Lexer lexer(QStringLiteral("id:0x700..0x7FF"));
    QVector<Token> tokens = lexer.tokenizeAll();

    QCOMPARE(tokens.size(), 6);
    QCOMPARE(tokens[0].text, QStringLiteral("id"));
    QCOMPARE(tokens[1].type, TokenType::Colon);
    QCOMPARE(tokens[2].numericValue, 0x700ULL);
    QCOMPARE(tokens[3].type, TokenType::DotDot);
    QCOMPARE(tokens[4].numericValue, 0x7FFULL);
    QVERIFY(!lexer.hasError());
}

void TestCanQueryLexer::tokenizeByteComparison()
{
    Lexer lexer(QStringLiteral("byte[2] == 0x80"));
    QVector<Token> tokens = lexer.tokenizeAll();

    QCOMPARE(tokens.size(), 7);
    QCOMPARE(tokens[0].text, QStringLiteral("byte"));
    QCOMPARE(tokens[1].type, TokenType::LeftBracket);
    QCOMPARE(tokens[2].numericValue, 2ULL);
    QCOMPARE(tokens[3].type, TokenType::RightBracket);
    QCOMPARE(tokens[4].type, TokenType::EqualsEquals);
    QCOMPARE(tokens[5].numericValue, 0x80ULL);
    QVERIFY(!lexer.hasError());
}

void TestCanQueryLexer::tokenizeStringLiteral()
{
    Lexer lexer(QStringLiteral("label:\"unlock\""));
    QVector<Token> tokens = lexer.tokenizeAll();

    QCOMPARE(tokens.size(), 4);
    QCOMPARE(tokens[0].text, QStringLiteral("label"));
    QCOMPARE(tokens[1].type, TokenType::Colon);
    QCOMPARE(tokens[2].type, TokenType::StringLiteral);
    QCOMPARE(tokens[2].text, QStringLiteral("unlock"));
    QVERIFY(!lexer.hasError());
}

void TestCanQueryLexer::handleUnterminatedStringError()
{
    Lexer lexer(QStringLiteral("label:\"unlock"));
    QVector<Token> tokens = lexer.tokenizeAll();

    QVERIFY(lexer.hasError());
    QCOMPARE(tokens.last().type, TokenType::Unknown);
}

void TestCanQueryLexer::handleUnexpectedCharError()
{
    Lexer lexer(QStringLiteral("id @ 0x10"));
    QVector<Token> tokens = lexer.tokenizeAll();

    QVERIFY(lexer.hasError());
}

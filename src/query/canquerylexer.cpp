#include "canquerylexer.h"

// Qt headers
#include <QChar>

// C++ standard-library headers
#include <cctype>

namespace CanQuery {

Lexer::Lexer(const QString &input)
    : input_(input)
{
}

void Lexer::skipWhitespace() noexcept
{
    while (cursor_ < input_.length() && input_.at(cursor_).isSpace()) {
        ++cursor_;
    }
}

Token Lexer::nextToken()
{
    skipWhitespace();

    if (cursor_ >= input_.length()) {
        Token tok;
        tok.type = TokenType::EndOfInput;
        tok.position = cursor_;
        return tok;
    }

    const QChar ch = input_.at(cursor_);

    if (ch.isDigit()) {
        return lexNumberOrHex();
    }

    if (ch.isLetter() || ch == '_') {
        return lexIdentifier();
    }

    if (ch == '"' || ch == '\'') {
        return lexStringLiteral();
    }

    return lexOperatorOrSymbol();
}

QVector<Token> Lexer::tokenizeAll()
{
    QVector<Token> tokens;
    while (true) {
        Token tok = nextToken();
        tokens.append(tok);
        if (tok.type == TokenType::EndOfInput || tok.type == TokenType::Unknown) {
            break;
        }
    }
    return tokens;
}

Token Lexer::lexNumberOrHex()
{
    const int startPos = cursor_;
    Token tok;
    tok.position = startPos;

    if (cursor_ + 1 < input_.length() && input_.at(cursor_) == '0' &&
        (input_.at(cursor_ + 1) == 'x' || input_.at(cursor_ + 1) == 'X')) {
        cursor_ += 2;
        const int hexStart = cursor_;
        while (cursor_ < input_.length() &&
               std::isxdigit(static_cast<unsigned char>(input_.at(cursor_).toLatin1()))) {
            ++cursor_;
        }
        if (cursor_ == hexStart) {
            hasError_ = true;
            errorMessage_ = QStringLiteral("Malformed hex number at position %1").arg(startPos);
            tok.type = TokenType::Unknown;
            return tok;
        }
        tok.text = input_.mid(startPos, cursor_ - startPos);
        bool ok = false;
        tok.numericValue = input_.mid(hexStart, cursor_ - hexStart).toULongLong(&ok, 16);
        tok.type = ok ? TokenType::Number : TokenType::Unknown;
        if (!ok) {
            hasError_ = true;
            errorMessage_ = QStringLiteral("Hex number overflow at position %1").arg(startPos);
        }
        return tok;
    }

    while (cursor_ < input_.length() && input_.at(cursor_).isDigit()) {
        ++cursor_;
    }
    tok.text = input_.mid(startPos, cursor_ - startPos);
    bool ok = false;
    tok.numericValue = tok.text.toULongLong(&ok, 10);
    tok.type = ok ? TokenType::Number : TokenType::Unknown;
    if (!ok) {
        hasError_ = true;
        errorMessage_ = QStringLiteral("Number overflow at position %1").arg(startPos);
    }
    return tok;
}

Token Lexer::lexIdentifier()
{
    const int startPos = cursor_;
    while (cursor_ < input_.length() &&
           (input_.at(cursor_).isLetterOrNumber() || input_.at(cursor_) == '_')) {
        ++cursor_;
    }

    Token tok;
    tok.position = startPos;
    tok.text = input_.mid(startPos, cursor_ - startPos);
    tok.type = TokenType::Identifier;
    return tok;
}

Token Lexer::lexStringLiteral()
{
    const int startPos = cursor_;
    const QChar quoteChar = input_.at(cursor_++);
    const int contentStart = cursor_;

    while (cursor_ < input_.length() && input_.at(cursor_) != quoteChar) {
        ++cursor_;
    }

    Token tok;
    tok.position = startPos;

    if (cursor_ >= input_.length()) {
        hasError_ = true;
        errorMessage_ = QStringLiteral("Unterminated string literal at position %1").arg(startPos);
        tok.type = TokenType::Unknown;
        return tok;
    }

    tok.text = input_.mid(contentStart, cursor_ - contentStart);
    tok.type = TokenType::StringLiteral;
    ++cursor_; // consume trailing quote
    return tok;
}

Token Lexer::lexOperatorOrSymbol()
{
    const int startPos = cursor_;
    const QChar ch = input_.at(cursor_++);

    Token tok;
    tok.position = startPos;

    switch (ch.unicode()) {
    case ':':
        tok.type = TokenType::Colon;
        tok.text = QStringLiteral(":");
        break;
    case '[':
        tok.type = TokenType::LeftBracket;
        tok.text = QStringLiteral("[");
        break;
    case ']':
        tok.type = TokenType::RightBracket;
        tok.text = QStringLiteral("]");
        break;
    case '.':
        if (cursor_ < input_.length() && input_.at(cursor_) == '.') {
            ++cursor_;
            tok.type = TokenType::DotDot;
            tok.text = QStringLiteral("..");
        } else {
            hasError_ = true;
            errorMessage_ = QStringLiteral("Unexpected character '.' at position %1").arg(startPos);
            tok.type = TokenType::Unknown;
        }
        break;
    case '=':
        if (cursor_ < input_.length() && input_.at(cursor_) == '=') {
            ++cursor_;
            tok.type = TokenType::EqualsEquals;
            tok.text = QStringLiteral("==");
        } else {
            hasError_ = true;
            errorMessage_ = QStringLiteral("Unexpected single '=' at position %1").arg(startPos);
            tok.type = TokenType::Unknown;
        }
        break;
    case '!':
        if (cursor_ < input_.length() && input_.at(cursor_) == '=') {
            ++cursor_;
            tok.type = TokenType::NotEquals;
            tok.text = QStringLiteral("!=");
        } else {
            hasError_ = true;
            errorMessage_ = QStringLiteral("Unexpected '!' without '=' at position %1").arg(startPos);
            tok.type = TokenType::Unknown;
        }
        break;
    case '<':
        if (cursor_ < input_.length() && input_.at(cursor_) == '=') {
            ++cursor_;
            tok.type = TokenType::LessEquals;
            tok.text = QStringLiteral("<=");
        } else {
            tok.type = TokenType::LessThan;
            tok.text = QStringLiteral("<");
        }
        break;
    case '>':
        if (cursor_ < input_.length() && input_.at(cursor_) == '=') {
            ++cursor_;
            tok.type = TokenType::GreaterEquals;
            tok.text = QStringLiteral(">=");
        } else {
            tok.type = TokenType::GreaterThan;
            tok.text = QStringLiteral(">");
        }
        break;
    default:
        hasError_ = true;
        errorMessage_ = QStringLiteral("Unexpected character '%1' at position %2").arg(ch).arg(startPos);
        tok.type = TokenType::Unknown;
        break;
    }

    return tok;
}

} // namespace CanQuery

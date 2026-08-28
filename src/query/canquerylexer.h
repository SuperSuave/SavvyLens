#ifndef CANQUERYLEXER_H
#define CANQUERYLEXER_H

// SavvyLens headers
#include "canquerytoken.h"

// Qt headers
#include <QString>
#include <QVector>

namespace CanQuery {

class Lexer
{
public:
    explicit Lexer(const QString &input);

    Token nextToken();
    QVector<Token> tokenizeAll();

    bool hasError() const noexcept { return hasError_; }
    QString errorMessage() const { return errorMessage_; }

private:
    void skipWhitespace() noexcept;
    Token lexNumberOrHex();
    Token lexIdentifier();
    Token lexStringLiteral();
    Token lexOperatorOrSymbol();

    QString input_;
    int cursor_ = 0;
    bool hasError_ = false;
    QString errorMessage_;
};

} // namespace CanQuery

#endif // CANQUERYLEXER_H

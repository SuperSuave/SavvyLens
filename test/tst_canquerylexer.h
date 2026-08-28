#ifndef TST_CANQUERYLEXER_H
#define TST_CANQUERYLEXER_H

// Qt headers
#include <QObject>

class TestCanQueryLexer : public QObject
{
    Q_OBJECT

private slots:
    void tokenizeSimpleIdFilter();
    void tokenizeIdRange();
    void tokenizeByteComparison();
    void tokenizeStringLiteral();
    void handleUnterminatedStringError();
    void handleUnexpectedCharError();
};

#endif // TST_CANQUERYLEXER_H

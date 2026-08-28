#ifndef CANQUERYTOKEN_H
#define CANQUERYTOKEN_H

// Qt headers
#include <QString>

// C++ standard-library headers
#include <cstdint>

namespace CanQuery {

enum class TokenType {
    Identifier,    // e.g. "id", "bus", "byte", "changed", "rate"
    Number,        // e.g. "123", "0x123"
    Colon,         // :
    DotDot,        // ..
    LeftBracket,   // [
    RightBracket,  // ]
    EqualsEquals,  // ==
    NotEquals,     // !=
    LessThan,      // <
    LessEquals,    // <=
    GreaterThan,   // >
    GreaterEquals, // >=
    StringLiteral, // e.g. "unlock"
    EndOfInput,    // End of input stream
    Unknown        // Syntax/lexical error
};

struct Token {
    TokenType type = TokenType::Unknown;
    QString text;
    quint64 numericValue = 0;
    int position = 0;

    constexpr bool isValid() const noexcept
    {
        return type != TokenType::Unknown;
    }
};

} // namespace CanQuery

#endif // CANQUERYTOKEN_H

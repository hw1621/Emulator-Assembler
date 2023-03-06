#ifndef TOKENIZER_H_
#define TOKENIZER_H_

typedef enum {
  TOK_BLANK,      // [ \f\t\v\r\n]+                   - see comment below
  TOK_COMMENT,    // [;][<all except \0, \r and \n>]* - see comment below
  TOK_IDENTIFIER, // [_ A-Z a-z] [_ 0-9 A-Z a-z]*
  TOK_STRING,     // ["][<all except ">]*["]
  TOK_NUMBER,     // [0-9]+ | [0][xX][0-9 A-F a-f]+
  TOK_COMMA,      // [,]
  TOK_COLON,      // [:]
  TOK_HASHSIGN,   // [#]
  TOK_EQUALS,     // [=]
  TOK_PLUS,       // [+]
  TOK_MINUS,      // [-]
  TOK_BANG,       // [!]
  TOK_LBRACKET,   // [[]
  TOK_RBRACKET,   // []]
  TOK_LBRACE,     // [{]
  TOK_RBRACE,     // [}]
  TOK_EOF,        // [\0]                             - see comment below
  TOK_UNKNOWN     // Represents an error
} TokenKind;

// Record type for a token, lexeme is the string [start, end)
typedef struct {
  TokenKind kind;
  const char* start, * end;
} Token;

// Returns the next token (skipping all `TOK_BLANK` and `TOK_COMMENT`), advancing pointer `*p`.
// On reaching the end of the string ('\0'), `*p` will not be advanced (so that repeated call of
// `nextToken` at the end of a string will give a sequence of `TOK_EOF`).
// Pre: `*p` points to a memory block whose end position is marked by a '\0'.
Token nextToken(const char** p);

// Returns the name of a token ID
const char* tokenName(TokenKind id);

#endif // TOKENIZER_H_

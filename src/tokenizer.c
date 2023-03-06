#include "tokenizer.h"
#include <stdbool.h>

bool between(char c, char l, char u) {
  return c >= l && c <= u;
}

Token nextToken(const char** p) {
  const char* start;
restart:
  start = *p;
  switch (**p) {
    case ' ': case '\f': case '\t': case '\v': case '\r': case '\n':
      (*p)++;
      while (true) {
        switch (**p) {
          case ' ': case '\f': case '\t': case '\v': case '\r': case '\n': (*p)++; break;
          default:                                                         goto restart;
        }
      }
    case ';':
      (*p)++;
      while (true) {
        switch (**p) {
          case '\0': case '\r': case '\n': goto restart;
          default:                         (*p)++; break;
        }
      }
    /*
    case '\r':
      (*p)++;
      switch (**p) {
        case '\n': (*p)++; break;
        default:   break;
      }
      return (Token) { TOK_LINE_BREAK, start, *p };
    case '\n': (*p)++; return (Token) { TOK_LINE_BREAK, start, *p };
    */
    case '"':
      (*p)++;
      while (true) {
        switch (**p) {
          case '"': (*p)++; return (Token) { TOK_STRING, start, *p };
          default:  (*p)++; break;
        }
      }
    case ',':  (*p)++; return (Token) { TOK_COMMA, start, *p };
    case ':':  (*p)++; return (Token) { TOK_COLON, start, *p };
    case '#':  (*p)++; return (Token) { TOK_HASHSIGN, start, *p };
    case '=':  (*p)++; return (Token) { TOK_EQUALS, start, *p };
    case '+':  (*p)++; return (Token) { TOK_PLUS, start, *p };
    case '-':  (*p)++; return (Token) { TOK_MINUS, start, *p };
    case '!':  (*p)++; return (Token) { TOK_BANG, start, *p };
    case '[':  (*p)++; return (Token) { TOK_LBRACKET, start, *p };
    case ']':  (*p)++; return (Token) { TOK_RBRACKET, start, *p };
    case '{':  (*p)++; return (Token) { TOK_LBRACE, start, *p };
    case '}':  (*p)++; return (Token) { TOK_RBRACE, start, *p };
    case '\0':         return (Token) { TOK_EOF, start, *p };
    default:
      if (between(**p, '0', '9')) {
        if (**p == '0') {
          (*p)++;
          if (**p == 'x' || **p == 'X') {
            (*p)++;
            while (between(**p, '0', '9') || between(**p, 'A', 'F') || between(**p, 'a', 'f')) (*p)++;
            return (Token) { TOK_NUMBER, start, *p };
          }
        }
        while (between(**p, '0', '9')) (*p)++;
        return (Token) { TOK_NUMBER, start, *p };
      }
      if (**p == '_' || between(**p, 'A', 'Z') || between(**p, 'a', 'z')) {
        (*p)++;
        while (**p == '_' || between(**p, '0', '9') || between(**p, 'A', 'Z') || between(**p, 'a', 'z')) (*p)++;
        return (Token) { TOK_IDENTIFIER, start, *p };
      }
      (*p)++;
      return (Token) { TOK_UNKNOWN, start, *p };
  }
}

const char* tokenName(TokenKind kind) {
  switch (kind) {
    case TOK_BLANK:      return "blank";
    case TOK_COMMENT:    return "comment";
    // case TOK_LINE_BREAK: return "line break";
    case TOK_IDENTIFIER: return "identifier";
    case TOK_STRING:     return "string";
    case TOK_NUMBER:     return "number";
    case TOK_COMMA:      return "comma (,)";
    case TOK_COLON:      return "colon (:)";
    case TOK_HASHSIGN:   return "hash sign (#)";
    case TOK_EQUALS:     return "equals sign (=)";
    case TOK_PLUS:       return "plus sign (+)";
    case TOK_MINUS:      return "minus sign (-)";
    case TOK_BANG:       return "exclamation sign (!)";
    case TOK_LBRACKET:   return "left bracket ([)";
    case TOK_RBRACKET:   return "right bracket (])";
    case TOK_LBRACE:     return "left brace ({)";
    case TOK_RBRACE:     return "right brace (})";
    case TOK_EOF:        return "end-of-file";
    case TOK_UNKNOWN:    return "unknown token";
    default:             return "unknown token";
  }
}

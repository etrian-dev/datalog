#ifndef LEXER_HH_INCLUDED
#define LEXER_HH_INCLUDED

#include <fstream>
#include <ostream>
#include <vector>

class FilePos {
private:
  size_t line;
  size_t col;
  std::streamoff offset;

public:
  FilePos(const size_t &line, const size_t &column,
          const std::streamoff &offset)
    : line(line), col(column), offset(offset) {
  }
  friend std::ostream &operator<<(std::ostream &stream, const FilePos &fpos);
  std::string to_string(void) const;
};

enum State { GOOD, ERROR };

// TODO: implement comment tokens (and discard them)

enum TokenType {
  END_OF_FILE,
  LITERAL,
  LPAREN = '(',
  RPAREN = ')',
  DOT = '.',
  COLON = ':',
  MINUS = '-',
  COMMA = ','
};

class Token {
private:
  TokenType type;
  std::string lexeme;
  FilePos file_pos;

public:
  Token(const TokenType tt, const std::string &lexeme, const FilePos &pos);
  friend std::ostream &operator<<(std::ostream &stream, const Token &tok);
  TokenType get_type(void) const;
  std::string get_lexeme(void) const;
  FilePos get_pos(void) const;
  std::string to_string(void) const;
};

class Lexer {
private:
  std::ifstream istream;
  State state;
  std::vector<Token> tokens;
  std::vector<Token> runLexer(std::istream &stream);

public:
  Lexer(std::string &ifile);
  ~Lexer();
  std::vector<Token> run(void);
  std::vector<Token> run(std::string &query);
};

void print_tokens(std::ostream &stream, const std::vector<Token> tokens);

#endif

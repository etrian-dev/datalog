
#include "lexer.hh"

#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>

std::string
FilePos::to_string(void) const {
  return "[" + std::to_string(line) + ":" + std::to_string(col) + " ("
         + std::to_string(offset) + ")]";
}

std::ostream &
operator<<(std::ostream &stream, const FilePos &fpos) {
  stream << "[" << fpos.line << ":" << fpos.col << "]";
  return stream;
}

Token::
Token(const TokenType tt, const std::string &lexeme, const FilePos &pos)
  : type(tt), lexeme(lexeme), file_pos(pos) {
}

std::ostream &
operator<<(std::ostream &stream, const Token &tok) {
  std::string ttype = "";
  ttype = static_cast<char>(tok.type);
  if (tok.type == TokenType::LITERAL) {
    ttype = "LITERAL";
  }
  if (tok.type == TokenType::END_OF_FILE) {
    ttype = "EOF";
  }
  stream << tok.file_pos << " Token(" << ttype << ","
         << "\"" << tok.lexeme << "\")";
  return stream;
}

std::string
Token::to_string(void) const {
  std::string s = "Token(";
  switch (type) {
    case TokenType::LITERAL:
      s += "LITERAL";
      break;
    case TokenType::END_OF_FILE:
      s += "EOF";
      break;
    default:
      s += static_cast<char>(type);
  }
  return s + ", \"" + lexeme + "\")";
}

TokenType
Token::get_type(void) const {
  return this->type;
}

std::string
Token::get_lexeme(void) const {
  return this->lexeme;
}

FilePos
Token::get_pos(void) const {
  return this->file_pos;
}

Lexer::
Lexer(std::string &ifile)
  : istream(ifile), state(State::GOOD), tokens() {
  if (!istream.is_open()) {
    throw ifile;
  }
}

Lexer::~
Lexer() {
  if (istream.is_open()) {
    istream.close();
  }
}

void
Lexer::set_stream(std::ifstream &new_stream) {
  reset();
  istream = std::move(new_stream);
}

void
Lexer::reset(void) {
  if (istream.is_open()) {
    istream.close();
  }
  istream.clear();
  state = State::GOOD;
  tokens.clear();
}

State
add_literal(std::vector<Token> &tokens, std::string &lexeme, FilePos &fpos) {
  if (lexeme.empty()) {
    return State::ERROR;
  }
  Token literal(TokenType::LITERAL, lexeme, fpos);
  tokens.push_back(literal);
  lexeme.clear();
  return State::GOOD;
}

std::vector<Token>
Lexer::run(std::string &query) {
  std::istringstream stream(query);
  return runLexer(stream);
}

std::vector<Token>
Lexer::run(void) {
  this->tokens = runLexer(istream);
  return tokens;
}

std::vector<Token>
Lexer::runLexer(std::istream &stream) {
  std::vector<Token> lexer_tokens;
  char c;
  size_t line = 1;
  size_t column = 0;
  std::string buffer("");
  stream.get(c);
  while (stream.good() && !stream.eof()) {
    FilePos fpos(line, column, stream.tellg());
    switch (c) {
      case TokenType::LPAREN:
      case TokenType::RPAREN:
      case TokenType::COMMA:
      case TokenType::MINUS:
      case TokenType::COLON: {
        FilePos bufpos(line, column - buffer.length() + 1, stream.tellg());
        if (buffer.length() > 0
            && add_literal(lexer_tokens, buffer, bufpos) == State::ERROR) {
          std::cerr << "[" << line << ":" << column - buffer.length()
                    << "] Expected literal, found \"\"\n";
          return lexer_tokens;
        }
        buffer = "";
        Token tok(static_cast<TokenType>(c), buffer, fpos);
        lexer_tokens.push_back(tok);
        ++column;
        break;
      }
      case TokenType::DOT: {
        if (buffer.length() > 0) {
          Token fact(TokenType::LITERAL, buffer, fpos);
          lexer_tokens.push_back(fact);
        }
        buffer = "";
        Token tok(static_cast<TokenType>(c), ".", fpos);
        lexer_tokens.push_back(tok);
        ++column;
        break;
      }
      case '\n': {
        if (buffer.length() > 0) {
          add_literal(lexer_tokens, buffer, fpos);
        }
        ++line;
        column = 0;
        break;
      }
      case ' ':
      case '\r': {
        ++column;
        break;
      }
      default: {
        // update the buffer
        buffer += c;
        ++column;
      }
    }
    stream.get(c);
  }
  FilePos endPos(line, column, stream.tellg());
  std::string lexeme("");
  Token eof(TokenType::END_OF_FILE, lexeme, endPos);
  lexer_tokens.push_back(eof);
  if (lexer_tokens.size() == 0 || state == State::ERROR) {
    std::cerr << "Lexing terminated with error\n";
  }
  else {
    std::cout << "Lexing terminated successfully (" << lexer_tokens.size()
              << " tokens read)\n";
  }
  return lexer_tokens;
}

void
print_tokens(std::ostream &stream, std::vector<Token> tokens) {
  for (auto token : tokens) {
    stream << token << "\n";
  }
}

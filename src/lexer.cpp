
#include "lexer.hh"

#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>

std::string FilePos::to_string(void) const {
  return "[" + std::to_string(line) + ":" + std::to_string(col) + " (" +
         std::to_string(offset) + ")]";
}

std::ostream &operator<<(std::ostream &stream, const FilePos &fpos) {
  stream << "[" << fpos.line << ":" << fpos.col << "]";
  return stream;
}

Token::Token(const TokenType tt, const std::string &lexeme, const FilePos &pos)
    : type(tt), lexeme(lexeme), file_pos(pos) {
  ;
}

std::ostream &operator<<(std::ostream &stream, const Token &tok) {
  std::string ttype = "";
  ttype = static_cast<char>(tok.type);
  if (tok.type == TokenType::LITERAL) {
    ttype = "LITERAL";
  }
  stream << tok.file_pos << " Token(" << ttype << ","
         << "\"" << tok.lexeme << "\")";
  return stream;
}

std::string Token::to_string(void) const {
  std::string s = "Token(";
  switch (type) {
  case TokenType::LITERAL:
    s += "LITERAL";
    break;
  case TokenType::END_OF_FILE:
    s += "EOF";
  default:
    s += static_cast<char>(type);
  }
  return s + ", \"" + lexeme + "\")";
}

TokenType Token::get_type(void) const { return this->type; }

std::string Token::get_lexeme(void) const { return this->lexeme; }

FilePos Token::get_pos(void) const { return this->file_pos; }

Lexer::Lexer(std::string &ifile)
    : istream(ifile), state(State::GOOD), tokens() {
  if (!istream.is_open()) {
    throw ifile;
  }
}

Lexer::~Lexer() {
  if (istream.is_open()) {
    istream.close();
  }
}

State add_literal(std::vector<Token> &tokens, std::string &lexeme,
                  FilePos &fpos) {
  if (lexeme.empty()) {
    return State::ERROR;
  }
  Token literal(TokenType::LITERAL, lexeme, fpos);
  tokens.push_back(literal);
  lexeme.clear();
  return State::GOOD;
}

std::vector<Token> Lexer::run(std::string &query) {
  std::istringstream stream(query);
  return runLexer(stream);
}

std::vector<Token> Lexer::run(void) {
  this->tokens = runLexer(istream);
  return tokens;
}

std::vector<Token> Lexer::runLexer(std::istream &stream) {
  std::vector<Token> lexer_tokens;
  char c;
  size_t line = 1;
  size_t column = 0;
  std::string buffer("");
  while (stream.good()) {
    FilePos fpos(line, column, stream.tellg());
    stream.get(c);
    switch (c) {
    case TokenType::LPAREN:
    case TokenType::RPAREN:
    case TokenType::DOT:
    case TokenType::COMMA:
    case TokenType::MINUS:
    case TokenType::COLON: {
      if (add_literal(lexer_tokens, buffer, fpos) == State::ERROR) {
        std::cerr << "[" << line << ":" << stream.tellg()
                  << "] Expected literal, found \"\"\n";
      }
      buffer = "";
      Token tok(static_cast<TokenType>(c), buffer, fpos);
      lexer_tokens.push_back(tok);
      ++column;
      break;
    }
    case '\n': {
      add_literal(lexer_tokens, buffer, fpos);
      ++line;
      column = 0;
      break;
    }
    case ' ':
    case '\r': {
      add_literal(lexer_tokens, buffer, fpos);
      ++column;
      break;
    }
    default: {
      // update the buffer
      buffer += c;
      ++column;
    }
    }
  }
  FilePos endPos(line, column, stream.tellg());
  std::string lexeme("");
  Token eof(TokenType::END_OF_FILE, lexeme, endPos);
  lexer_tokens.push_back(eof);
  if (state == State::ERROR) {
    std::cerr << "Lexing terminated with error\n";
  } else {
    std::cout << "Lexing terminated successfully (" << lexer_tokens.size()
              << " tokens read)\n";
  }
  return lexer_tokens;
}

void print_tokens(std::ostream &stream, std::vector<Token> tokens) {
  for (auto token : tokens) {
    stream << token << "\n";
  }
}
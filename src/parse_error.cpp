#include "parser.hh"
#include "lexer.hh"
#include "interpreter.hh"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

ParseError::
ParseError(const std::string &cause, const Token &current)
  : std::runtime_error(cause), cause(cause), token(current) {
  std::cout << current.get_pos().to_string() << cause << "\t"
            << current.to_string() << "\n";
}

ParseError::
ParseError(const std::string &cause)
  : std::runtime_error(cause), cause(cause),
    token(TokenType::END_OF_FILE, "", FilePos(0, 0, 0)) {
  std::cout << cause << "\n";
}

const char *
ParseError::what(void) {
  std::string text
    = token.get_pos().to_string() + cause + "\ttok = " + token.to_string();
  return text.c_str();
}

#include "parser.hh"
#include "lexer.hh"
#include "interpreter.hh"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

Term::
Term(std::string &pred, TermType ttype)
  : name(pred), term_type(ttype) {
}

std::string
Term::get_name(void) {
  return std::string(name);
}
TermType
Term::get_term_type(void) {
  return term_type;
}

bool
Term::operator==(Term &other) {
  return this->name == other.get_name()
         && this->term_type == other.get_term_type();
}

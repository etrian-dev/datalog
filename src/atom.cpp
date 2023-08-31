#include "parser.hh"
#include "lexer.hh"
#include "interpreter.hh"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

Atom::
Atom(std::string &pred, std::vector<Term> &terms)
  : predicate(pred), terms(terms) {
}

Atom::
Atom(std::string &pred)
  : predicate(pred) {
  terms = std::vector<Term>();
}

std::string
Atom::get_predicate(void) {
  return std::string(predicate);
}
std::vector<Term>
Atom::get_terms(void) {
  return terms;
}

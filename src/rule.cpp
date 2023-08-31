#include "parser.hh"
#include "lexer.hh"
#include "interpreter.hh"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

Rule::
Rule(Atom &head, std::vector<Atom> &goals)
  : head(head), goals(goals) {
}

Rule::
Rule(Atom &head)
  : head(head), goals(std::vector<Atom>()) {
}

Atom
Rule::get_head(void) {
  std::string pred = head.get_predicate();
  std::vector<Term> terms = head.get_terms();
  return Atom(pred, terms);
}

std::vector<Atom>
Rule::get_goals(void) {
  return goals;
}

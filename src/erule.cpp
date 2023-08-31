#include "parser.hh"
#include "lexer.hh"
#include "interpreter.hh"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

EvaluatedRule::
EvaluatedRule(EvaluatedAtom &head, std::vector<EvaluatedAtom> &goals)
  : ehead(head), egoals(goals) {
}

EvaluatedAtom
EvaluatedRule::get_ehead(void) {
  return EvaluatedAtom(ehead);
}

std::vector<EvaluatedAtom>
EvaluatedRule::get_egoals(void) {
  return std::vector<EvaluatedAtom>(egoals);
}

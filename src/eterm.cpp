#include "interpreter.hh"
#include "ast.hh"
#include "parser.hh"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

EvaluatedTerm::
EvaluatedTerm(std::string pred, TermType tt)
  : Term(pred, tt) {
}

bool
EvaluatedTerm::set_binding(std::string &name) {
  if (bound_value.empty()) {
    bound_value = std::string(name);
    return true;
  }
  return false;
}

void
EvaluatedTerm::reset_binding(void) {
  bound_value.clear();
}

std::string
EvaluatedTerm::get_bound(void) {
  if (!bound_value.empty()) {
    return std::string(bound_value);
  }
  throw std::runtime_error("No value is bound!");
}

bool
EvaluatedTerm::is_bound(void) {
  return !bound_value.empty();
}

bool
EvaluatedTerm::operator==(EvaluatedTerm &other) {
  if (Term::operator==(other)) {
    return ((!bound_value.empty() && other.is_bound())
            || (!bound_value.empty() && !other.is_bound()));
  }
  return false;
}

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
  : Term(pred, tt), bound_value(nullptr) {
}

bool
EvaluatedTerm::set_binding(EvaluatedTerm *other) {
  if (bound_value == nullptr) {
    bound_value = other;
    return true;
  }
  else if (bound_value->get_name() == other->get_name()) {
    return true;
  }
  return false;
}

void
EvaluatedTerm::reset_binding(void) {
  bound_value = nullptr;
}

EvaluatedTerm *
EvaluatedTerm::get_bound(void) {
  if (bound_value) {
    return bound_value;
  }
  return nullptr;
}

bool
EvaluatedTerm::is_bound(void) {
  return (bound_value != nullptr);
}

bool
EvaluatedTerm::operator==(EvaluatedTerm &other) {
  if (Term::operator==(other)) {
    if (!bound_value && !other.is_bound()) {
      return true;
    }
    if (bound_value && other.is_bound()) {
      return *bound_value == *other.get_bound();
    }
  }
  return false;
}

bool
EvaluatedTerm::operator!=(EvaluatedTerm &other) {
  return !(*this == other);
}

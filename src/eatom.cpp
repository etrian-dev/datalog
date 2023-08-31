#include "interpreter.hh"
#include "ast.hh"
#include "parser.hh"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

EvaluatedAtom::
EvaluatedAtom(void) {
}

EvaluatedAtom::
EvaluatedAtom(std::string &pred, std::vector<EvaluatedTerm> &terms)
  : predicate(pred), terms(terms) {
}
EvaluatedAtom::
EvaluatedAtom(Atom &inner)
  : predicate(inner.get_predicate()) {
  for (Term t : inner.get_terms()) {
    EvaluatedTerm et = EvaluatedTerm(t.get_name(), t.get_term_type());
    terms.push_back(et);
  }
}
EvaluatedAtom::
EvaluatedAtom(std::string &pred)
  : predicate(pred) {
  terms = std::vector<EvaluatedTerm>();
}
bool
EvaluatedAtom::is_ground(void) {
  for (EvaluatedTerm term : terms) {
    if (!term.is_bound()) {
      return false;
    }
  }
  return true;
}
void
EvaluatedAtom::update_term(EvaluatedTerm &t) {
  std::replace_if(
    terms.begin(), terms.end(),
    [&t](EvaluatedTerm &term) { return term.operator==(t); }, t);
}

void
EvaluatedAtom::reset(void) {
  for (EvaluatedTerm eterm : terms) {
    eterm.reset_binding();
  }
}

EvaluatedTerm &
EvaluatedAtom::get_term(size_t idx) {
  if (idx > terms.size()) {
    throw std::runtime_error("Idx too big");
  }
  return terms[idx];
}

void
EvaluatedAtom::remove_term(EvaluatedTerm &t) {
  std::remove_if(terms.begin(), terms.end(),
                 [&t](EvaluatedTerm &term) { return term == t; });
}

std::vector<EvaluatedTerm>
EvaluatedAtom::get_eterms(void) {
  return terms;
}

std::string
EvaluatedAtom::get_predicate(void) {
  return std::string(predicate);
}

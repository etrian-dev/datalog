#include "interpreter.hh"
#include "ast.hh"
#include "parser.hh"

#include <stdexcept>
#include <string>
#include <vector>

EvaluatedTerm::EvaluatedTerm(Term &inner) {
  bound_value = std::make_shared<Term>(&inner);
}
bool EvaluatedTerm::set_binding(Term *t) {
  if (!bound_value) {
    bound_value.reset(t);
    return true;
  }
  return false;
}
void EvaluatedTerm::reset_binding(void) { bound_value.reset(); }
Term *EvaluatedTerm::get_bound(void) {
  if (bound_value) {
    return bound_value.get();
  }
  throw std::runtime_error("No value is bound!");
}

bool EvaluatedTerm::is_bound(void) {
  if (bound_value) {
    return true;
  }
  return false;
}

bool can_unify(Term &r_term, Term &q_term) {
  if (r_term.get_name() == q_term.get_name()) {
    return true;
  }
  if (r_term.get_term_type() == TermType::CONSTANT &&
      q_term.get_term_type() == TermType::CONSTANT) {
    return false;
  }
  return true;
}

void unify(EvaluatedTerm &var_term, EvaluatedTerm &const_term) {
  Term *var_t = var_term.get_bound();
  Term *const_t = const_term.get_bound();
  if (var_t->get_term_type() == TermType::CONSTANT) {
    unify(const_term, var_term);
  } else {
    var_term.set_binding(const_t);
  }
}

void Interpreter::interpret(Program &program, Program &query) {
  Rule q_rule = query.get_rules()[0];
  Atom q_head = q_rule.get_head();
  std::vector<Term> q_terms = q_head.get_terms();
  std::vector<Rule> rules = program.get_rules();

  for (auto rule : rules) {
    Atom head = rule.get_head();
    // try to match the head of the rule with the query's one
    std::vector<Term> h_terms = head.get_terms();
    if (head.get_predicate() == q_head.get_predicate() &&
        h_terms.size() == q_terms.size()) {
      bool hasMismatch = false;
      size_t i = 0;
      while (!hasMismatch && i < q_terms.size()) {
        if (can_unify(h_terms[i], q_terms[i])) {
          // unify
          EvaluatedTerm h_eterm(h_terms[i]);
          EvaluatedTerm q_eterm(q_terms[i]);
          unify(h_eterm, q_eterm);
        } else {
          hasMismatch = true;
        }
        ++i;
      }
    }
  }
}
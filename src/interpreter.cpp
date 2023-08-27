#include "interpreter.hh"
#include "ast.hh"
#include "parser.hh"

#include <algorithm>
#include <iostream>
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

bool EvaluatedTerm::is_bound(void) { return (bool)this->bound_value; }

EvaluatedAtom::EvaluatedAtom(std::string &pred,
                             std::vector<EvaluatedTerm> &terms)
    : predicate(pred), terms(terms) {
  ;
}
EvaluatedAtom::EvaluatedAtom(Atom &inner) : predicate(inner.get_predicate()) {
  for (Term t : inner.get_terms()) {
    terms.push_back(EvaluatedTerm(t));
  }
}
EvaluatedAtom::EvaluatedAtom(void) : predicate("") {
  terms = std::vector<EvaluatedTerm>();
}
bool EvaluatedAtom::is_ground(void) {
  for (EvaluatedTerm term : terms) {
    if (!term.is_bound() ||
        term.get_bound()->get_term_type() == TermType::VARIABLE) {
      return false;
    }
  }
  return true;
}
void EvaluatedAtom::add_term(EvaluatedTerm &t) { terms.push_back(t); }

void EvaluatedAtom::remove_term(EvaluatedTerm &t) {
  std::remove_if(terms.begin(), terms.end(),
                 [&t](EvaluatedTerm &term) { return term == t; });
}

std::vector<EvaluatedTerm> &EvaluatedAtom::get_eterms(void) { return terms; }

constexpr std::string_view EvaluatedAtom::get_predicate(void) {
  return predicate;
}

bool EvaluatedTerm::operator==(EvaluatedTerm &other) {
  if (this->bound_value && other.is_bound()) {
    Term t1 = *bound_value;
    Term t2 = other.get_bound();
    return t1.get_term_type() == t2.get_term_type() &&
           t1.get_name() == t2.get_name();
  }
  return false;
}

/* *Real* intepreter */

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

bool unify(EvaluatedTerm &var_term, EvaluatedTerm &const_term) {
  Term *var_t = var_term.get_bound();
  Term *const_t = const_term.get_bound();
  // Always have the possibly variable term as the first arg
  if (var_t->get_term_type() == TermType::CONSTANT &&
      const_t->get_term_type() == TermType::VARIABLE) {
    return unify(const_term, var_term);
  } else {
    return var_term.set_binding(const_t);
  }
}

bool unify_atom(EvaluatedAtom &goal, EvaluatedAtom &query) {
  std::vector<EvaluatedTerm> h_terms = goal.get_eterms();
  std::vector<EvaluatedTerm> q_terms = query.get_eterms();
  if (goal.get_predicate() == goal.get_predicate() &&
      h_terms.size() == q_terms.size()) {
    bool hasMismatch = false;
    size_t i = 0;
    while (!hasMismatch && i < q_terms.size()) {
      Term h_t = h_terms[i].get_bound();
      Term q_t = q_terms[i].get_bound();
      if (can_unify(h_t, q_t)) {
        // unify
        EvaluatedTerm h_eterm(h_terms[i]);
        EvaluatedTerm q_eterm(q_terms[i]);
        // a term cannot be unified -> fail
        if (unify(h_eterm, q_eterm)) {
          goal.add_term(h_eterm);
        } else {
          hasMismatch = true;
          goal.remove_term(h_eterm);
        }
      } else {
        hasMismatch = true;
      }
      ++i;
    }
    return !hasMismatch;
  }
  return false;
}

void Interpreter::interpret(Program &program, Program &query) {
  Rule q_rule = query.get_rules()[0];
  std::vector<Rule> rules = program.get_rules();
  AstPrinter printer;

  for (auto rule : rules) {
    // try to match the head of the rule with the query's one
    Atom head = rule.get_head();
    Atom q_heda = q_rule.get_head();
    EvaluatedAtom ehead = EvaluatedAtom();
    EvaluatedAtom q_ehead = EvaluatedAtom();
    if (unify_atom(ehead, q_ehead)) {
      std::cout << "Unified heads:\n"
                << "Head: " << printer.visit(rule)
                << "Query: " << printer.visit(query);
      // then unify the goals
      for (auto goal : rule.get_goals()) {
      }
    }
  }
}

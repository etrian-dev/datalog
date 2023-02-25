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
Term EvaluatedTerm::get_bound(void) {
  if (bound_value) {
    return *bound_value.get();
  }
  throw std::runtime_error("No value is bound!");
}

void Interpreter::interpret(Program &program, Program &query) {
  Rule q_rule = query.get_rules()[0];
  Atom q_head = q_rule.get_head();
  std::vector<Term> q_terms = q_head.get_terms();
  std::vector<Rule> rules = program.get_rules();
  for (auto rule : rules) {
    Atom head = rule.get_head();
    // try to match the head of the rule with the query's one
    std::vector<Term> terms = head.get_terms();
    if (head.get_predicate() == q_head.get_predicate() &&
        terms.size() == q_terms.size()) {
      bool hasMismatch = false;
      size_t i = 0;
      while (!hasMismatch && i < q_terms.size()) {
        TermType q_ttype = q_terms[i].get_term_type();
        std::string q_term_name = q_terms[i].get_name();
        TermType ttype = terms[i].get_term_type();
        std::string term_name = terms[i].get_name();
        switch (ttype) {
        case TermType::CONSTANT:
          if (q_ttype == TermType::CONSTANT && term_name != q_term_name) {
            hasMismatch = true;
          }
          break;
        case TermType::VARIABLE:
          if (q_ttype == TermType::CONSTANT) {
            // bind the rule term to the constant and continue
            EvaluatedTerm eterm(terms[i]);
            AstPrinter visitor;
            std::cout << "Bound term "
                      << "\n"; // << visitor.visit(q_terms[i]) << " to "
                               //<< visitor.visit(terms[i]) << "\n";
          }
          break;
        default:
          break;
        }
        ++i;
      }
    }
  }
}
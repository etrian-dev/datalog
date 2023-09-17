#include "parser.hh"
#include "interpreter.hh"

#include <string>
#include <vector>

/*
 * <program> ::= <fact> <program> | <rule> <program> | ɛ
 * <fact> ::=  <relation> "(" <constant-list> "). | halt."
 * <rule> ::= <atom> ":-" <atom-list> "."
 * <atom> ::= <relation> "(" <term-list> ")"
 * <atom-list> ::= <atom> | <atom> "," <atom-list>
 * <term> ::= <constant> | <variable>
 * <term-list> ::= <term> | <term> "," <term-list>
 * <constant-list> ::= <constant> | <constant> "," <constant-list>
 */

template <>
std::string
AstPrinter::visit(Term &term) {
  std::string term_type("const");
  if (term.get_term_type() == TermType::VARIABLE) {
    term_type = "var";
  }
  return term.get_name() + ":" + term_type;
}

template <>
std::string
AstPrinter::visit(EvaluatedTerm &eterm) {
  if (eterm.is_bound()) {
    return "[" + eterm.get_bound()->get_name() + "/" + eterm.get_name()
           + "]:var";
  }
  else {
    return eterm.get_name() + ":"
           + (eterm.get_term_type() == TermType::CONSTANT ? "const" : "var");
  }
}

template <>
std::string
AstPrinter::visit(Atom &atom) {
  std::string s = atom.get_predicate();
  std::vector<Term> terms = atom.get_terms();
  if (terms.empty()) {
    return s;
  }
  else {
    s += "(";
  }
  for (size_t i = 0; i < terms.size() - 1; ++i) {
    s += terms[i].accept(*this) + ", ";
  }
  return s + terms[terms.size() - 1].accept(*this) + ")";
}

template <>
std::string
AstPrinter::visit(EvaluatedAtom &atom) {
  std::string s = std::string(atom.get_predicate());
  std::vector<EvaluatedTerm> eterms = atom.get_eterms();
  if (eterms.empty()) {
    return s;
  }
  else {
    s += "(";
  }
  for (size_t i = 0; i < eterms.size() - 1; ++i) {
    s += eterms[i].accept(*this) + ", ";
  }
  return s + eterms[eterms.size() - 1].accept(*this) + ")";
}

template <>
std::string
AstPrinter::visit(Rule &rule) {
  Atom head = rule.get_head();
  std::string s = head.accept(*this);
  std::vector<Atom> goals = rule.get_goals();
  if (goals.size() == 0) {
    return s + "\n";
  }
  s += ":-\n";
  size_t i = 0;
  for (; i < goals.size(); ++i) {
    s += "\t" + goals[i].accept(*this) + ",\n";
  }
  return s + "\t" + goals[goals.size() - 1].accept(*this) + ".\n";
}

template <>
std::string
AstPrinter::visit(Program &program) {
  std::string s;
  std::vector<Rule> facts_rules = program.get_rules();
  for (size_t i = 0; i < facts_rules.size(); ++i) {
    s += facts_rules[i].accept(*this);
  }
  return s;
}

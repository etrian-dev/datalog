#include "interpreter.hh"
#include "ast.hh"
#include "parser.hh"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

/* *Real* intepreter */

bool
can_unify(EvaluatedTerm &r_term, EvaluatedTerm &q_term) {
  if (r_term.get_name() == q_term.get_name()) {
    return true;
  }
  if (r_term.get_term_type() == TermType::CONSTANT
      && q_term.get_term_type() == TermType::CONSTANT) {
    return false;
  }
  return true;
}

bool
unify(EvaluatedTerm &t1, EvaluatedTerm &t2) {
  // Always have the possibly variable term as the first arg
  if (t1.get_term_type() == TermType::CONSTANT
      && t2.get_term_type() == TermType::VARIABLE) {
    return unify(t2, t1);
  }
  else if (t1.get_term_type() == TermType::CONSTANT) {
    // both are constant
    return t1.get_name() == t2.get_name();
  }
  else {
    // bind the var with the term in t2
    Term *const_t = t2.get_bound();
    return t1.set_binding(const_t);
  }
}

bool
unify_atom(EvaluatedAtom &goal, EvaluatedAtom &query) {
  std::vector<EvaluatedTerm> h_terms = goal.get_eterms();
  std::vector<EvaluatedTerm> q_terms = query.get_eterms();
  if (goal.get_predicate() == goal.get_predicate()
      && h_terms.size() == q_terms.size()) {
    bool hasMismatch = false;
    size_t i = 0;
    while (!hasMismatch && i < q_terms.size()) {
      if (can_unify(h_terms[i], q_terms[i])) {
        // unify
        EvaluatedTerm h_eterm(h_terms[i]);
        EvaluatedTerm q_eterm(q_terms[i]);
        // a term cannot be unified -> fail
        if (unify(h_eterm, q_eterm)) {
          query.add_term(h_eterm);
        }
        else {
          hasMismatch = true;
          query.reset();
        }
      }
      else {
        query.reset();
        hasMismatch = true;
      }
      ++i;
    }
    return !hasMismatch;
  }
  return false;
}

bool
unify_rule(Program &program, EvaluatedRule &q_rule) {
  AstPrinter printer;
  std::vector<Rule> rules = program.get_rules();
  for (auto rule : rules) {
    // try to match the head of the rule with the query's one
    Atom head = rule.get_head();
    EvaluatedAtom ehead = EvaluatedAtom(head);
    EvaluatedAtom q_ehead = q_rule.get_ehead();
    if (!unify_atom(ehead, q_ehead)) {
      std::cout << "Failed to unify heads"
                << "\nHead: " << printer.visit(ehead)
                << "\nQuery: " << printer.visit(q_ehead);
      continue;
    }
    std::cout << "Heads can be unified:"
              << "\nHead: " << printer.visit(ehead)
              << "\nQuery: " << printer.visit(q_ehead);
    // then unify the goals
    for (auto goal : rule.get_goals()) {
      // Non-unified goal
      EvaluatedAtom egoal = EvaluatedAtom(goal);
      std::vector<EvaluatedTerm> eterms = ehead.get_eterms();
      // All bound terms of the head are bound in the goal, if possible
      for (auto goal_eterm : egoal.get_eterms()) {
        // Find a common term that is bound in the head
        auto eterm_opt
          = std::find_if(eterms.begin(), eterms.end(),
                         [&goal_eterm](EvaluatedTerm key) {
                           return key.get_name() == goal_eterm.get_name()
                                  && key.is_bound();
                         });
        if (eterm_opt != std::end(eterms)) {
          // Copy the bindind from the head to the goal
          Term *t = eterm_opt->get_bound();
          goal_eterm.set_binding(t);
        }
      }
      std::cout << "(Partially) bound goal:" << printer.visit(egoal) << "\n";
      std::vector<EvaluatedAtom> dummy = std::vector<EvaluatedAtom>();
      EvaluatedRule erule = EvaluatedRule(egoal, dummy);
      if (!unify_rule(program, erule)) {
        std::cout << "Failed to unify rule\n"
                  << printer.visit(rule) << "on goal:\n"
                  << printer.visit(egoal);
        return false;
      }
    }
  }
  return true;
}

bool
Interpreter::interpret(Program &program, Program &query) {

  Rule q_rule = query.get_rules()[0];
  Atom q_head = q_rule.get_head();
  std::vector<Atom> q_goals = q_rule.get_goals();
  EvaluatedAtom q_ehead = EvaluatedAtom(q_head);
  std::vector<EvaluatedAtom> q_egoals;
  for (auto q_goal : q_goals) {
    EvaluatedAtom q_egoal = EvaluatedAtom(q_goal);
    q_egoals.push_back(q_goal);
  }
  EvaluatedRule q_erule = EvaluatedRule(q_ehead, q_egoals);
  return unify_rule(program, q_erule);
}

bool
Interpreter::do_halt(Program &query) {
  std::vector<Rule> rules = query.get_rules();
  return !rules.empty() > 0
         && rules[0].get_head().get_predicate() == HALT_COMMAND;
}

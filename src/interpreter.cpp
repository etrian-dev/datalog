/**
 * @file interpreter.cpp
 *
 * Implementation of the datalog interpreter
 */

#include "interpreter.hh"
#include "ast.hh"
#include "parser.hh"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

/* Helpers */
template <typename T>
void
print_unification(std::ostream &stream, T &target, T &query, bool successful) {
  AstPrinter printer;
  stream << "Unification "
         << (successful ? "\033[32msucceeded\033[0m" : "\033[31mfailed\033[0m")
         << ": " << printer.visit(target) << " and " << printer.visit(query)
         << "\n";
}

void
print_binding_chain(std::ostream &stream, EvaluatedTerm &term) {
  AstPrinter printer;
  stream << "Binding chain for term:\n";
  EvaluatedTerm t = term;
  while (t.is_bound()) {
    stream << printer.visit(t) << " -> ";
    t = *t.get_bound();
  }
  stream << printer.visit(t) << "\n";
}

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

/**
 * @brief Unify term t1 with term t2
 * @param t1 (possibly) bound term to be unified
 * @param t2 unbound term to be unified
 * @returns true iff at least one unification can be found
 */
bool
unify_term(EvaluatedTerm &t1, EvaluatedTerm &t2) {
  // Always have the possibly variable term as the first arg
  if (t1.get_term_type() == TermType::CONSTANT
      && t2.get_term_type() == TermType::VARIABLE) {
    return unify_term(t2, t1);
  }
  else if (t1.get_term_type() == TermType::CONSTANT) {
    // both are constant
    return t1.get_name() == t2.get_name();
  }
  // check whether the value t1 is the same as t2
  else if (t1.is_bound()) {
    // for each bound term in t2's binding chains, search for that term in t1's
    // chain
    //
    // [P/t1] -> [C/P] -----
    //                     |
    //                     v
    // [A/t2] -> [B/A] -> [C/B] -> [D/C] -> [cnst/D]
    //
    bool found = false;
    EvaluatedTerm t2_chain = t2;
    // till the chain of bound terms for t2 bottoms out, try to find a matching
    // term in the chain for t1
    while (t2_chain.get_term_type() != TermType::CONSTANT) {
      EvaluatedTerm bound_t1 = t1;
      while (bound_t1.is_bound()) {
        bound_t1 = *bound_t1.get_bound();
        if (bound_t1 == t2_chain) {
          found = true;
        }
      }
      if (bound_t1 == t2_chain) {
        found = true;
      }
      // The chain for t2 ends in an unbounded variable
      if (!t2_chain.is_bound()
          && t2_chain.get_term_type() == TermType::VARIABLE) {
        break;
      }
      // otherwise advance t2's chain
      t2_chain = *t2_chain.get_bound();
    }
    return found;
  }
  else {
    // bind the var with the term in t2
    return t1.set_binding(&t2);
  }
}

/**
 * @brief Unify atom goal with atom query, by exaustively
 * finding all possible bindings, if any.
 * @param goal The goal to be unified
 * @param query The search query to be unified
 * @returns true iff at least one unification can be found
 */
bool
unify_atom(EvaluatedAtom &goal, EvaluatedAtom &query) {
  size_t q_nterms = query.get_eterms().size();
  if (goal.get_predicate() == query.get_predicate()
      && goal.get_eterms().size() == q_nterms) {
    bool hasMismatch = false;
    size_t i = 0;
    while (!hasMismatch && i < q_nterms) {
      // a term cannot be unified -> fail
      if (!unify_term(goal.get_term(i), query.get_term(i))) {
        print_unification(std::cerr, goal.get_term(i), query.get_term(i),
                          false);
        query.reset();
        hasMismatch = true;
      }
      else {
        print_unification(std::cerr, goal.get_term(i), query.get_term(i), true);
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
      print_unification(std::cerr, ehead, q_ehead, false);
      continue;
    }
    print_unification(std::cout, ehead, q_ehead, true);
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
          goal_eterm.set_binding(eterm_opt->get_bound());
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

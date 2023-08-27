#ifndef INTERPRETER_HH_INCLUDED
#define INTERPRETER_HH_INCLUDED

#include "ast.hh"
#include "parser.hh"

#include <iostream>
#include <memory>

class EvaluatedTerm {
private:
  std::shared_ptr<Term> bound_value;

public:
  EvaluatedTerm(Term &inner);
  bool set_binding(Term *t);
  void reset_binding(void);
  Term *get_bound(void);
  bool is_bound(void);
  bool operator==(EvaluatedTerm &other);
};

class EvaluatedAtom {
private:
  const std::string_view predicate;
  std::vector<EvaluatedTerm> terms;

public:
  EvaluatedAtom(std::string &pred, std::vector<EvaluatedTerm> &terms);
  EvaluatedAtom(Atom &atom);
  EvaluatedAtom(void);
  bool is_ground(void);
  void add_term(EvaluatedTerm &t);
  void remove_term(EvaluatedTerm &t);
  std::vector<EvaluatedTerm> &get_eterms(void);
  constexpr std::string_view get_predicate(void);
};

class Interpreter {
public:
  void interpret(Program &program, Program &query);
};

#endif

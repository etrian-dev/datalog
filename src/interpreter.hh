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
};

class EvaluatedAtom {
private:
  std::vector<EvaluatedTerm> terms;

public:
  EvaluatedAtom(std::vector<EvaluatedTerm> &terms);
  EvaluatedAtom(void);
  bool is_ground(void);
  void add_term(EvaluatedTerm &t);
};

class Interpreter {
public:
  void interpret(Program &program, Program &query);
};

#endif
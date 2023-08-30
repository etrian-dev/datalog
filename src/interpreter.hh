#ifndef INTERPRETER_HH_INCLUDED
#define INTERPRETER_HH_INCLUDED

#include "ast.hh"
#include "parser.hh"

#include <iostream>
#include <memory>

const std::string_view HALT_COMMAND = "halt";

class EvaluatedTerm : public Term {
private:
  std::shared_ptr<Term> bound_value;

public:
  EvaluatedTerm(Term &t);
  bool set_binding(Term *t);
  void reset_binding(void);
  Term *get_bound(void);
  bool is_bound(void);
  bool operator==(EvaluatedTerm &other);

  template <class T>
  T accept(AstVisitor<T> &visitor) {
    return visitor.visit(*this);
  }
};

class EvaluatedAtom : AstNode {
private:
  std::string predicate;
  std::vector<EvaluatedTerm> terms;

public:
  EvaluatedAtom(void);
  EvaluatedAtom(std::string &pred, std::vector<EvaluatedTerm> &terms);
  EvaluatedAtom(Atom &atom);
  EvaluatedAtom(std::string &pred);
  bool is_ground(void);
  void add_term(EvaluatedTerm &t);
  void remove_term(EvaluatedTerm &t);
  void reset(void);
  std::vector<EvaluatedTerm> get_eterms(void);
  std::string get_predicate(void);

  template <class T>
  T accept(AstVisitor<T> &visitor) {
    return visitor.visit(*this);
  }
};

class EvaluatedRule : AstNode {
private:
  EvaluatedAtom ehead;
  std::vector<EvaluatedAtom> egoals;

public:
  EvaluatedRule(Rule &rule);
  EvaluatedRule(EvaluatedAtom &head, std::vector<EvaluatedAtom> &goals);
  EvaluatedAtom get_ehead(void);
  std::vector<EvaluatedAtom> get_egoals(void);
};

class Interpreter {
public:
  bool interpret(Program &program, Program &query);
  bool do_halt(Program &query);
};

#endif

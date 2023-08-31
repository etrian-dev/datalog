#ifndef DATALOG_HH_INCLUDED
#define DATALOG_HH_INCLUDED

#include "lexer.hh"
#include "parser.hh"
#include "interpreter.hh"

bool can_unify(EvaluatedTerm &r_term, EvaluatedTerm &q_term);
bool unify_term(EvaluatedTerm &t1, EvaluatedTerm &t2);
bool unify_atom(EvaluatedAtom &goal, EvaluatedAtom &query);
bool unify_rule(Program &program, EvaluatedRule &q_rule);

#endif

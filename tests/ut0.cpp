#include <snitch/snitch_all.hpp>

#include "datalog.hh"

TEST_CASE("unify_term", "[unify][term]") {
  EvaluatedTerm t1 = EvaluatedTerm("pred", TermType::CONSTANT);
  EvaluatedTerm t2 = EvaluatedTerm("X", TermType::VARIABLE);
  REQUIRE(unify_term(t1, t2));
  REQUIRE(t2.get_bound()->get_name() == t1.get_name());
}

TEST_CASE("unify_term_vars", "[unify][term]") {
  EvaluatedTerm t1 = EvaluatedTerm("X", TermType::VARIABLE);
  EvaluatedTerm t2 = EvaluatedTerm("Y", TermType::CONSTANT);
  // unification successful X -> [Y/X], Y -> Y
  REQUIRE(unify_term(t1, t2));
  REQUIRE(t1.get_bound()->get_name() == t2.get_name());
  REQUIRE_FALSE(t2.is_bound());
}

TEST_CASE("unify_const", "[unify][term]") {
  EvaluatedTerm t1 = EvaluatedTerm("casa", TermType::CONSTANT);
  EvaluatedTerm t2 = EvaluatedTerm("caso", TermType::CONSTANT);
  EvaluatedTerm t3 = EvaluatedTerm("casa", TermType::CONSTANT);
  REQUIRE_FALSE(unify_term(t1, t2));
  REQUIRE(unify_term(t1, t3));
  REQUIRE_FALSE(t1.is_bound());
}

TEST_CASE("unify_const", "[unify][term]") {
  // If t1 unified with t2, it can't unify with t3
  EvaluatedTerm t1 = EvaluatedTerm("X", TermType::VARIABLE);
  EvaluatedTerm t2 = EvaluatedTerm("p", TermType::CONSTANT);
  EvaluatedTerm t3 = EvaluatedTerm("q", TermType::CONSTANT);
  REQUIRE(unify_term(t1, t2));
  REQUIRE_FALSE(unify_term(t1, t3));
  REQUIRE(t1.get_bound()->get_name() == t2.get_name());
}

TEST_CASE("unify_deduce", "[unify][term]") {
  // If t1 unified with t2 and t2 is an unbound variable,
  // then t2 unifies with t2 and t1 with t3
  // 1. X -> [Y/X], Y -> Y
  // 2. Y -> [pp/Y], pp -> pp
  // 3. []
  EvaluatedTerm t1 = EvaluatedTerm("X", TermType::VARIABLE);
  EvaluatedTerm t2 = EvaluatedTerm("Y", TermType::VARIABLE);
  EvaluatedTerm t3 = EvaluatedTerm("Z", TermType::VARIABLE);
  EvaluatedTerm t4 = EvaluatedTerm("pp", TermType::CONSTANT);
  REQUIRE(unify_term(t1, t2));
  REQUIRE(unify_term(t2, t4));
  REQUIRE(unify_term(t3, t2));
  REQUIRE(unify_term(t1, t3));
}

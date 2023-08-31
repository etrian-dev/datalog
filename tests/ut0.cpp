#include <snitch/snitch_all.hpp>

#include "datalog.hh"

TEST_CASE("unify_term", "[term]") {
  EvaluatedTerm t1 = EvaluatedTerm("pred", TermType::CONSTANT);
  EvaluatedTerm t2 = EvaluatedTerm("X", TermType::VARIABLE);
  REQUIRE(unify_term(t1, t2));
  REQUIRE(t2.get_bound() == t1.get_name());
}

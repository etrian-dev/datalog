#include "parser.hh"
#include "lexer.hh"
#include "interpreter.hh"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

Program::
Program(std::vector<Rule> &rules)
  : rules(rules) {
}
Program::
Program(void) {
  rules = std::vector<Rule>();
}

std::vector<Rule>
Program::get_rules(void) {
  return rules;
}

void
Program::add_rule(Rule &rule) {
  rules.push_back(rule);
}

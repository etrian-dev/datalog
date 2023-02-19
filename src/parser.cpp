#include "parser.hh"
#include "lexer.hh"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

ParseError::ParseError(const std::string &cause, const Token &current)
    : std::runtime_error(cause), cause(cause), token(current) {
  std::cout << current.get_pos().to_string() << cause << "\t"
            << current.to_string() << "\n";
}

ParseError::ParseError(const std::string &cause)
    : std::runtime_error(cause), cause(cause),
      token(TokenType::END_OF_FILE, "", FilePos(0, 0, 0)) {
  std::cout << cause << "\n";
}

const char *ParseError::what(void) {
  std::string text =
      token.get_pos().to_string() + cause + "\ttok = " + token.to_string();
  return text.c_str();
}

/*<program> ::= <fact> <program> | <rule> <program> | ɛ
<fact> ::=  <relation> "(" <constant-list> ")."
<rule> ::= <atom> ":-" <atom-list> "."
<atom> ::= <relation> "(" <term-list> ")"
<atom-list> ::= <atom> | <atom> "," <atom-list>
<term> ::= <constant> | <variable>
<term-list> ::= <term> | <term> "," <term-list>
<constant-list> ::= <constant> | <constant> "," <constant-list>
*/

std::string AstPrinter::visit(Term &term) {
  std::string term_type("const");
  if (term.get_term_type() == TermType::VARIABLE) {
    term_type = "var";
  }
  return term.get_name() + ":" + term_type;
}

std::string AstPrinter::visit(Atom &atom) {
  std::string s = atom.get_predicate() + "(";
  std::vector<Term> terms = atom.get_terms();
  for (size_t i = 0; i < terms.size() - 1; ++i) {
    s += terms[i].accept(*this) + ", ";
  }
  return s + terms[terms.size() - 1].accept(*this) + ")";
}

std::string AstPrinter::visit(Rule &rule) {
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

std::string AstPrinter::visit(Program &program) {
  std::string s;
  std::vector<Rule> facts_rules = program.get_rules();
  for (size_t i = 0; i < facts_rules.size(); ++i) {
    s += facts_rules[i].accept(*this);
  }
  return s;
}

Term::Term(std::string &name, TermType type) : name(name), term_type(type) {}

std::string Term::get_name(void) { return std::string(name); }
TermType Term::get_term_type(void) { return term_type; }

Atom::Atom(std::string &pred, std::vector<Term> &terms)
    : predicate(pred), terms(terms) {}

Atom::Atom(std::string &pred) : predicate(pred) { terms = std::vector<Term>(); }

std::string Atom::get_predicate(void) { return std::string(predicate); }
std::vector<Term> Atom::get_terms(void) { return std::vector<Term>(terms); }

Rule::Rule(Atom &head, std::vector<Atom> &goals) : head(head), goals(goals) {}

Rule::Rule(Atom &head) : head(head), goals(std::vector<Atom>()) {}

Atom Rule::get_head(void) {
  std::string pred = head.get_predicate();
  std::vector<Term> terms = head.get_terms();
  return Atom(pred, terms);
}

std::vector<Atom> Rule::get_goals(void) { return std::vector<Atom>(goals); }

Program::Program(std::vector<Rule> &rules) : rules(rules) {}
Program::Program(void) { rules = std::vector<Rule>(); }

std::vector<Rule> Program::get_rules(void) { return std::vector<Rule>(rules); }

Parser::Parser(std::vector<Token> &token_list) : current(0) {
  tokens = token_list;
}

Parser::~Parser() {}

Token &Parser::peek(void) { return tokens[current]; }

Token &Parser::advance(void) { return tokens[current++]; }

Token &Parser::previous(void) {
  if (current == 0UL) {
    throw ParseError("Cannot roll back the start of the token sequence");
  }
  return tokens[current - 1UL];
}

bool Parser::is_eof(Token &tok) {
  return tok.get_type() == TokenType::END_OF_FILE;
}

Program Parser::parse(void) {
  try {
    return parse_program();
  } catch (ParseError &error) {
    std::cout /*<< "\033[31m"*/ << error.what() /*<< "\033[0m"*/ << std::endl;
  }
  return Program();
}

Program Parser::parse_program(void) {
  Program prog; // the empty program
  std::vector<Rule> rules;
  while (!is_eof(peek())) {
    Rule rule = parse_rule();
    rules.push_back(rule);
  }
  prog = Program(rules);
  return prog;
}

Rule Parser::parse_rule(void) {
  Atom head = parse_atom();
  Token next = advance();
  if (next.get_type() == TokenType::DOT) {
    // then we're parsing a fact => we are done
    std::vector<Term> terms = head.get_terms();
    return Rule(head);
  } else if (next.get_type() == TokenType::COLON) {
    if (peek().get_type() == TokenType::MINUS) {
      // then we're parsing a rule: more atoms to follow
      std::vector<Atom> goals;
      advance();
      while (peek().get_type() != TokenType::DOT) {
        Atom goal = parse_atom();
        goals.push_back(goal);
        Token comma_or_dot = advance();
        if (comma_or_dot.get_type() == TokenType::DOT) {
          break;
        }
        if (comma_or_dot.get_type() != TokenType::COMMA) {
          throw ParseError("Expected comma at ", previous());
        }
      }
      advance(); // discard the final dot
      return Rule(head, goals);
    }
  }
  throw ParseError("Expected dot or :- at ", next);
}

Atom Parser::parse_atom(void) {
  Token relation = advance();
  if (relation.get_type() == TokenType::LITERAL) {
    std::string lexeme = relation.get_lexeme();
    Token next = advance();
    if (next.get_type() == TokenType::LPAREN) {
      // parsing a term list
      Term term = parse_term();
      std::vector<Term> terms{term};
      while (peek().get_type() != TokenType::RPAREN) {
        advance();
        Term term = parse_term();
        Token comma_or_paren = peek();
        terms.push_back(term);
        if (comma_or_paren.get_type() == TokenType::RPAREN) {
          break;
        }
        if (comma_or_paren.get_type() != TokenType::COMMA) {
          throw ParseError("Expected comma at ", comma_or_paren);
        }
      }
      advance(); // discard the RPAREN
      return Atom(lexeme, terms);
    } else if (next.get_type() == TokenType::DOT) {
      // Just asserts a fact like "Prolog."
      return Atom(lexeme);
    }
    throw ParseError("Expected a term list or a dot at ", previous());
  }
  throw ParseError("Expected a predicate at ", peek());
}

bool is_var(std::string &lexeme) {
  return (!lexeme.empty() &&
          (lexeme[0] == '_' || (lexeme[0] >= 'A' && lexeme[0] <= 'Z')));
}

Term Parser::parse_term(void) {
  Token tok = advance();
  if (tok.get_type() == TokenType::LITERAL) {
    std::string lexeme = tok.get_lexeme();
    // FIXME: we actually need to check if it's valid identifier,
    // unless we do that at the lexer level
    if (is_var(lexeme)) {
      // then it's a variable
      return Term(lexeme, TermType::VARIABLE);
    }
    return Term(lexeme, TermType::CONSTANT);
  }
  throw ParseError("Expected a variable or constant at ", previous());
}

void print_ast(std::ostream &stream, Program &ast) {
  AstPrinter visitor;
  stream << visitor.visit(ast);
}
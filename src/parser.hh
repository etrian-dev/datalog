#ifndef PARSER_HH_INCLUDED
#define PARSER_HH_INCLUDED

#include "ast.hh"
#include "lexer.hh"

#include <stdexcept>
#include <string>
#include <vector>

/* Parser grammar
<program> ::= <fact> <program> | <rule> <program> | ɛ
<fact> ::=  <relation> "(" <constant-list> "). | halt."
<rule> ::= <atom> ":-" <atom-list> "."
<atom> ::= <relation> "(" <term-list> ")"
<atom-list> ::= <atom> | <atom> "," <atom-list>
<term> ::= <constant> | <variable>
<term-list> ::= <term> | <term> "," <term-list>
<constant-list> ::= <constant> | <constant> "," <constant-list>
*/

class ParseError : std::runtime_error {
private:
  std::string cause;
  Token token;

public:
  ParseError(const std::string &cause);
  ParseError(const std::string &cause, const Token &current);
  const char *what(void);
};

enum class TermType { CONSTANT, VARIABLE };

// ABC for AST grammar nodes
class AstNode {
  template <class T>
  T accept(AstVisitor<T> &visitor);
};

class Term : AstNode {
private:
  std::string name;
  TermType term_type;

public:
  Term(std::string &name, TermType type);
  std::string get_name(void);
  TermType get_term_type(void);
  bool operator==(Term &other);

  template <class T>
  T accept(AstVisitor<T> &visitor) {
    return visitor.visit(*this);
  }
};

class Atom : AstNode {
private:
  std::string predicate;
  std::vector<Term> terms;

public:
  Atom(std::string &pred, std::vector<Term> &terms);
  Atom(std::string &pred);
  std::string get_predicate(void);
  std::vector<Term> get_terms(void);

  template <class T>
  T accept(AstVisitor<T> &visitor) {
    return visitor.visit(*this);
  }
};

class Rule {
private:
  Atom head;
  std::vector<Atom> goals;

public:
  Rule(Atom &head);
  Rule(Atom &head, std::vector<Atom> &goals);
  Atom get_head(void);
  std::vector<Atom> get_goals(void);

  template <class T>
  T accept(AstVisitor<T> &visitor) {
    return visitor.visit(*this);
  }
};

class Program : AstNode {
private:
  std::vector<Rule> rules;
  //	Program& operator=(const Program &prog);
public:
  Program(void);
  Program(std::vector<Rule> &rules);
  std::vector<Rule> get_rules(void);
  void add_rule(Rule &rule);

  template <class T>
  T accept(AstVisitor<T> &visitor) {
    return visitor.visit(*this);
  }
};

class Parser {
private:
  size_t current;
  std::vector<Token> tokens;

  Program parse_program(void);
  Rule parse_rule(void);
  Atom parse_atom(void);
  Term parse_term(void);

  // void synchronize(void);
  Token &peek(void);
  Token &advance(void);
  Token &previous(void);
  bool is_eof(Token &tok);
  void reset(std::vector<Token> &tokens, size_t pos);

public:
  Parser(std::vector<Token> &token_list);
  ~Parser();
  Program parse(void);
  Program parse(std::vector<Token> &tokens);
};

void print_ast(std::ostream &stream, Program &ast);

#endif

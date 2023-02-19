#ifndef PARSER_HH_INCLUDED
#define PARSER_HH_INCLUDED

#include "lexer.hh"

#include <stdexcept>
#include <string>
#include <vector>

/* Parser grammar
<program> ::= <fact> <program> | <rule> <program> | ɛ
<fact> ::=  <relation> "(" <constant-list> ")."
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

class Program;
class Rule;
class Atom;
class Term;

class AstPrinter {
public:
  std::string visit(Program &program);
  std::string visit(Rule &rule);
  std::string visit(Atom &atom);
  std::string visit(Term &term);
};

// ABC for AST grammar nodes
class AstNode {
  // template <class T>
  virtual std::string accept(AstPrinter &visitor) = 0;
};

class Term : AstNode {
private:
  std::string name;
  TermType term_type;

public:
  Term(std::string &name, TermType type);
  std::string get_name(void);
  TermType get_term_type(void);

  std::string accept(AstPrinter &visitor) { return visitor.visit(*this); }
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

  // template <class T>
  std::string accept(AstPrinter &visitor) { return visitor.visit(*this); }
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

  // template <class T>
  std::string accept(AstPrinter &visitor) { return visitor.visit(*this); }
};

class Program : AstNode {
private:
  std::vector<Rule> rules;
  //	Program& operator=(const Program &prog);
public:
  Program(void);
  Program(std::vector<Rule> &rules);
  std::vector<Rule> get_rules(void);

  // template <class T>
  std::string accept(AstPrinter &visitor) { return visitor.visit(*this); }
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

public:
  Parser(std::vector<Token> &token_list);
  ~Parser();
  Program parse(void);
};

void print_ast(std::ostream &stream, Program &ast);

/*template <typename T>
class AstVisitor<T> {
        T visitProgram(const &Program) = 0;
        T visitFact(const &Fact) = 0;
        T visitRule(const &Program) = 0;
        T visitAtom(const &Atom) = 0;
        T visitTerm(const &Term) = 0;
};

class AstPrinter : AstVisitor<std::string> {
        std::string visitProgram(const Program &program);
        std::string visitFact(const Fact &fact);
        std::string visitRule(const Rule &rule);
        std::string visitAtom(const Atom &atom);
        std::string visitTerm(const Term &term);
};*/

#endif
#ifndef PARSER_HH_INCLUDED
#define PARSER_HH_INCLUDED

#include "lexer.hh"

#include <stdexcept>
#include <vector>
#include <string>

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
	FilePos error_loc;
public:
	ParseError(const std::string &cause);
	ParseError(const std::string &cause, const FilePos &loc);
	const char* what(void);
};

enum class TermType {
	CONSTANT,
	VARIABLE
};

//template <typename T>
//class AstVisitor<T>;

// ABC for AST grammar nodes
class AstNode {
	//template <class T>
	//T accept(AstVisitor<T> &visitor);
};

class Term : AstNode {
private:
	std::string name;
	TermType term_type;
public:
	Term(std::string &name, TermType type);
	std::string get_name(void);
	TermType get_term_type(void);

	//template <class T>
	//inline T accept(AstVisitor<T> &visitor) {visitor.visit(this);}
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
	
	//template <class T>
	//inline T accept(AstVisitor<T> &visitor) {visitor.visit(this);}
};

class Fact : AstNode {
protected:
	Atom head;
public:
	Fact(Atom &head);
	Atom get_head(void);
	
	//template <class T>
	//inline T accept(AstVisitor<T> &visitor) {visitor.visit(this);}
};

// NOTE: public inheritance implies that the Liskov's
// substitution principle is respected
class Rule : public Fact {
private:
	std::vector<Atom> goals;
public:
	Rule(Atom &head, std::vector<Atom> &goals);
	Atom get_head(void);
	std::vector<Atom> get_goals(void);
	
	//template <class T>
	//inline T accept(AstVisitor<T> &visitor) {visitor.visit(this);}
};

class Program : AstNode {
private:
	std::vector<Fact> facts;
//	Program& operator=(const Program &prog);
public:
	Program(void);
	Program(std::vector<Fact> &facts);
	std::vector<Fact> get_facts_rules(void);
	
	//template <class T>
	//inline T accept(AstVisitor<T> &visitor) {visitor.visit(this);}
};

class Parser {
private:
	size_t current;
	std::vector<Token> tokens;

	Program parse_program(void);
	Fact parse_fact_rule(void);
	Atom parse_atom(void);
	Term parse_term(void);

	void synchronize(void);
	Token& peek(void);
	Token& advance(void);
	Token& previous(void);
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
#include "parser.hh"
#include "lexer.hh"

#include <vector>
#include <string>
#include <stdexcept>

ParseError::ParseError(const std::string &cause, const FilePos &loc) :
	std::runtime_error(cause.c_str()), cause(cause), error_loc(loc) {
}

ParseError::ParseError(const std::string &cause) :
	std::runtime_error(cause.c_str()), cause(cause), error_loc(0, 0, 0) {
}

const char* ParseError::what(void) {
	std::string text = error_loc.to_string() + cause;
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

std::string visitTerm(Term &term) {
	std::string term_type("const");
	if(term.get_term_type() == TermType::VARIABLE) {
		term_type = "var";
	}
	return term.get_name() + ":" + term_type;
}

std::string visitAtom(Atom &atom) {
	std::string s = atom.get_predicate() + "(";
	std::vector<Term> terms = atom.get_terms();
	for(size_t i = 0; i < terms.size() - 1; ++i) {
		s += visitTerm(terms[i]) + ", ";
	}
	return s + visitTerm(terms[terms.size() - 1]) + ").\n";
}

std::string visitFact(Fact &fact) {
	Atom head = fact.get_head();
	return visitAtom(head) + ".\n";
}
std::string visitFact(Rule &rule) {
	Atom head = rule.get_head();
	std::string s = visitAtom(head) + ":-\n";
	std::vector<Atom> goals = rule.get_goals();
	size_t i = 0;
	for(; i < goals.size(); ++i) {
		s += "\t" + visitAtom(goals[i]) + ",\n";
	}
	return s + visitAtom(goals[goals.size() - 1]) + ".\n";
}

std::string visitProgram(Program &program) {
	std::string s;
	std::vector<Fact> facts_rules = program.get_facts_rules();
	for (size_t i = 0; i < facts_rules.size(); ++i) {
		s += visitFact(facts_rules[i]);
	}
	return s;
}


Term::Term(std::string &name, TermType type) :
	name(name), term_type(type) {}

std::string Term::get_name(void) {
	return std::string(name);
}
TermType Term::get_term_type(void) {
	return term_type;
}

Atom::Atom(std::string &pred, std::vector<Term> &terms) :
	predicate(pred), terms(terms) {}

Atom::Atom(std::string &pred) : predicate(pred) {
	terms = std::vector<Term>();
}

std::string Atom::get_predicate(void) {
	return std::string(predicate);
}
std::vector<Term> Atom::get_terms(void) {
	return std::vector<Term>(terms);
}

Fact::Fact(Atom &head) :
	head(head) {}

Atom Fact::get_head(void) {
	std::string pred = head.get_predicate();
	std::vector<Term> terms = head.get_terms();
	return Atom(pred, terms);
}

Rule::Rule(Atom &head, std::vector<Atom> &goals) :
	Fact(head), goals(goals) {}

Atom Rule::get_head(void) {
	std::string pred = head.get_predicate();
	std::vector<Term> terms = head.get_terms();
	return Atom(pred, terms);
}

std::vector<Atom> Rule::get_goals(void) {
	return std::vector<Atom>(goals);
}

Program::Program(std::vector<Fact> &facts) :
	facts(facts) {}
Program::Program(void) {
	facts = std::vector<Fact>();
}

std::vector<Fact> Program::get_facts_rules(void) {
	return std::vector<Fact>(facts);
}

Parser::Parser(std::vector<Token> &token_list) : 
	current(0) {
	tokens = token_list;
}

Parser::~Parser() {}

Token& Parser::peek(void) {
	return tokens[current];
}

Token& Parser::advance(void) {
	return tokens[current++];
}

Token& Parser::previous(void) {
	if(current == 0UL) {
		throw ParseError("Cannot roll back the start of the token sequence");
	}
	return tokens[current - 1UL];
}

bool Parser::is_eof(Token &tok) {
	return tok.get_type() == TokenType::END_OF_FILE;
}


Program Parser::parse(void) {
	return parse_program();
}

Program Parser::parse_program(void) {
	Program prog; // the empty program
	std::vector<Fact> facts;
	while(!is_eof(peek())) {
		Fact fact = parse_fact_rule();
		facts.push_back(fact);
	}
	prog = Program(facts);
	return prog;
}

Fact Parser::parse_fact_rule(void) {
	Atom head = parse_atom();
	if(advance().get_type() == TokenType::DOT) {
		// then we're parsing a fact => we are done
		return Fact(head);
	}
	else if(peek().get_type() == TokenType::COLON
			&& advance().get_type() == TokenType::MINUS) {
		// then we're parsing a rule: more atoms to follow
		std::vector<Atom> atoms {head};
		while(advance().get_type() != TokenType::DOT) {
			Atom goal = parse_atom();
			if(advance().get_type() != TokenType::COMMA) {
				throw ParseError("Expected comma at");
			}
			atoms.push_back(goal);
		}
		return Rule(head, atoms);
	}
	throw ParseError("Expected dot or :- at");
}

Atom Parser::parse_atom(void) {
	Token relation = peek();
	if(relation.get_type() == TokenType::LITERAL) {
		std::string lexeme = relation.get_lexeme();
		Token tok = advance();
		if(tok.get_type() == TokenType::LPAREN) {
			// parsing a term list
			Term term = parse_term();
			std::vector<Term> terms {term};
			while(advance().get_type() != TokenType::RPAREN) {
				Term term = parse_term();
				if(advance().get_type() != TokenType::COMMA) {
					throw ParseError("Expected comma at");
				}
				terms.push_back(term);
			}
			// expect a ")"
			if(advance().get_type() != TokenType::RPAREN) {
				throw ParseError("Expected a right paren at");
			}
			return Atom(lexeme, terms);
		}
		else if(tok.get_type() == TokenType::DOT) {
			// Just asserts a fact like "Prolog."
			return Atom(lexeme);
		}
		throw ParseError("Expected a term list or a dot at");
	}
	throw ParseError("Expected a predicate at");
}

bool is_var(std::string &lexeme) {
	return (!lexeme.empty() && (lexeme[0] == '_' || (lexeme[0] >= 'A' && lexeme[0] <= 'Z')));
}

Term Parser::parse_term(void) {
	Token tok = advance();
	if(tok.get_type() == TokenType::LITERAL) {
		std::string lexeme = tok.get_lexeme();
		// FIXME: we actually need to check if it's valid identifier, 
		// unless we do that at the lexer level
		if(is_var(lexeme)) {
			// then it's a variable
			return Term(lexeme, TermType::VARIABLE);
		}
		return Term(lexeme, TermType::CONSTANT);
	}
	throw ParseError("Expected a variable or constant at");
}

void print_ast(std::ostream &stream, Program &ast) {
	//AstPrinter visitor;
	stream << visitProgram(ast);
}
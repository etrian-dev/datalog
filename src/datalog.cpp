/*
<program> ::= <fact> <program> | <rule> <program> | ɛ
<fact> ::=  <relation> "(" <constant-list> ")." 
<rule> ::= <atom> ":-" <atom-list> "."
<atom> ::= <relation> "(" <term-list> ")"
<atom-list> ::= <atom> | <atom> "," <atom-list>
<term> ::= <constant> | <variable>
<term-list> ::= <term> | <term> "," <term-list>
<constant-list> ::= <constant> | <constant> "," <constant-list>
*/
#include "datalog.hh"

#include <fstream>
#include <ostream>
#include <iostream>

Token::Token(TokenType tt, std::string &lexeme, FilePos &pos) :
		type(tt), lexeme(lexeme), file_pos(pos) {
	;
		}

Lexer::Lexer(std::string &ifile) :
	istream(ifile), 
	state(State::GOOD),
	tokens() {
	if(!istream.is_open()) {
		throw ifile;
	}
}

Lexer::~Lexer() {
	if(istream.is_open()) {
		istream.close();
	}
}

State add_literal(std::vector<Token> &tokens, std::string &lexeme, FilePos &fpos) {
	if(lexeme.empty()) {
		return State::ERROR;
	}	
	Token literal(TokenType::LITERAL, lexeme, fpos);
	tokens.push_back(literal);
	lexeme.clear();
	return State::GOOD;
}

std::vector<Token> Lexer::run(void) {
	char c;
	size_t line = 1;
	size_t column = 0;
	std::string buffer("");
	while(istream.good()) {
		FilePos fpos(line, column, istream.tellg());
		istream.get(c);
		switch(c) {
		case TokenType::LPAREN:
		case TokenType::RPAREN:
		case TokenType::DOT:
		case TokenType::COMMA:
		case TokenType::MINUS: {
			if(add_literal(tokens, buffer, fpos) == State::ERROR) {
				std::cerr 	<< "[" << line << ":" << istream.tellg() 
							<< "] Expected literal, found \"\"\n";
			}
			buffer = "";
			Token tok(static_cast<TokenType>(c), buffer, fpos);
			tokens.push_back(tok);
			++column;
			break;
		}
		case '\n': {
			add_literal(tokens, buffer, fpos);
			++line;
			column = 0;
			break;
		}
		case ' ':
		case '\r': {
			add_literal(tokens, buffer, fpos);
			++column;
			break;
		}
		default: {
			// update the buffer
			buffer += c;
			++column;
		}
		}
	}
	if(state == State::ERROR) {
		std::cerr 	<< "Lexing terminated with error\n";
	} else {
		std::cout	<< "Lexing terminated successfully (" 
					<< tokens.size() << " tokens read)\n";
	}
	return tokens;
}

void Lexer::print_tokens(std::ostream &stream) {
	for(auto token : tokens) {
		stream << token << "\n";
	}
}
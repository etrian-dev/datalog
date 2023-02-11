
#include "lexer.hh"

#include <fstream>
#include <ostream>
#include <iostream>

std::string FilePos::to_string(void) {
	return "[" + std::to_string(line) + ":" 
			+ std::to_string(col) + " (" 
			+ std::to_string(offset) + "]";
}

std::ostream& operator<<(std::ostream &stream, const FilePos &fpos) {
	stream << "[" << fpos.line << ":" << fpos.col << "]";
	return stream;
}

Token::Token(TokenType tt, std::string &lexeme, FilePos &pos) :
		type(tt), lexeme(lexeme), file_pos(pos) {
	;
		}

std::ostream& operator<<(std::ostream &stream, const Token &tok) {
	std::string ttype = "";
	ttype = static_cast<char>(tok.type);
	if(tok.type == TokenType::LITERAL) {
		ttype = "LITERAL";
	}
	stream 	<< tok.file_pos << " Token(" << ttype << "," 
			<< "\"" << tok.lexeme << "\")";
	return stream;
}

TokenType Token::get_type(void) {
	return this->type;
}

std::string Token::get_lexeme(void) {
	return this->lexeme;
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
	FilePos endPos(line, column, istream.tellg());
	std::string lexeme("");
	Token eof(TokenType::END_OF_FILE, lexeme, endPos);
	tokens.push_back(eof);
	if(state == State::ERROR) {
		std::cerr 	<< "Lexing terminated with error\n";
	} else {
		std::cout	<< "Lexing terminated successfully (" 
					<< tokens.size() << " tokens read)\n";
	}
	return tokens;
}

void print_tokens(std::ostream &stream, std::vector<Token> tokens) {
	for(auto token : tokens) {
		stream << token << "\n";
	}
}
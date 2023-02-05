#ifndef DATALOG_HH_INCLUDED
#define DATALOG_HH_INCLUDED

#include <fstream>
#include <vector>
#include <ostream>

class FilePos {
private:
	size_t line;
	size_t col;
	std::streamoff offset;
public:
	FilePos(const size_t &line, const size_t &column, const std::streamoff &offset) :
		line(line), col(column), offset(offset) {}
	friend std::ostream& operator<<(std::ostream &stream, const FilePos &fpos);
};

std::ostream& operator<<(std::ostream &stream, const FilePos &fpos) {
	stream << "[" << fpos.line << ":" << fpos.col << "]";
	return stream;
}

enum State
{
	GOOD,
	ERROR
};

enum TokenType
{
	LPAREN = '(',
	RPAREN = ')',
	DOT = '.',
	COLON = ':',
	MINUS = '-',
	COMMA = ',',
	LITERAL
};

class Token {
private:
	TokenType type;
	std::string lexeme;
	FilePos file_pos;
public:
	Token(TokenType tt, std::string &lexeme, FilePos &pos);
	friend std::ostream& operator<<(std::ostream &stream, const Token &tok);
};

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

class Lexer {
private:
	std::ifstream istream;
	State state;
	std::vector<Token> tokens;
public:
	Lexer(std::string &ifile);
	~Lexer();
	std::vector<Token> run(void);
	void print_tokens(std::ostream &stream);
};

#endif
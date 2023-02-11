#ifndef LEXER_HH_INCLUDED
#define LEXER_HH_INCLUDED

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
	std::string to_string(void);
};

enum State
{
	GOOD,
	ERROR
};

enum TokenType
{
	END_OF_FILE,
	LITERAL,
	LPAREN = '(',
	RPAREN = ')',
	DOT = '.',
	COLON = ':',
	MINUS = '-',
	COMMA = ','
};

class Token {
private:
	TokenType type;
	std::string lexeme;
	FilePos file_pos;
public:
	Token(TokenType tt, std::string &lexeme, FilePos &pos);
	friend std::ostream& operator<<(std::ostream &stream, const Token &tok);
	TokenType get_type(void);
	std::string get_lexeme(void);
};

class Lexer {
private:
	std::ifstream istream;
	State state;
	std::vector<Token> tokens;
public:
	Lexer(std::string &ifile);
	~Lexer();
	std::vector<Token> run(void);
};

void print_tokens(std::ostream &stream, const std::vector<Token> tokens);

#endif
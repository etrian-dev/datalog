#include "datalog.hh"

#include <fstream>
#include <iostream>
#include <vector>

void usage(char **argv) {
	std::cout << "Usage: " << argv[0] << " <FILE>\n";
}

int main(int argc, char** argv) {
	if(argc == 1) {
		usage(argv);
		return 0;
	}
	std::string ifile(argv[1]);
	Lexer lexer(ifile);
	std::vector<Token> tokens = lexer.run();
	print_tokens(std::cout, tokens);
	Parser parser(tokens);
	Program prog = parser.parse();
	print_ast(std::cout, prog);
	return 0;
}
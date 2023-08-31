#include "../src/datalog.hh"

#include <fstream>
#include <iostream>
#include <vector>

void
usage(char **argv) {
  std::cout << "Usage: " << argv[0] << "<KB> <QUERY>\n";
}

int
main(int argc, char **argv) {
  if (argc == 1) {
    usage(argv);
    return 0;
  }
  // read an input file
  std::string ifile(argv[1]);
  Lexer lexer = Lexer(ifile);
  std::vector<Token> tokens = lexer.run();
  Parser parser(tokens);
  Program prog = parser.parse();
  // print_ast(std::cout, prog);

  //  read the query file
  std::string iquery(argv[2]);
  std::ifstream ifs(iquery);
  lexer.set_stream(ifs);
  std::vector<Token> query_tokens = lexer.run();
  // print_tokens(std::cout, query_tokens);
  Program query = parser.parse(query_tokens);
  // print_ast(std::cout, query);

  Interpreter interpreter;
  if (interpreter.interpret(prog, query)) {
    return 0;
  }
  return 1;
}

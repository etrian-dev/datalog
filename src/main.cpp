#include "datalog.hh"

#include <fstream>
#include <iostream>
#include <vector>

void
usage(char **argv) {
  std::cout << "Usage: " << argv[0] << " <FILE>...\n";
}

int
main(int argc, char **argv) {
  if (argc == 1) {
    usage(argv);
    return 0;
  }
  // read input file(s)
  std::string ifile(argv[1]);
  Lexer lexer(ifile);
  std::vector<Token> tokens = lexer.run();
  print_tokens(std::cout, tokens);
  Parser parser(tokens);
  Program prog = parser.parse();
  // print_ast(std::cout, prog);
  //  make a query
  std::string buf;
  std::cout << "? ";
  Interpreter interpreter;
  while (std::cin.good()) {
    std::getline(std::cin, buf, '\n');
    std::vector<Token> query_tokens = lexer.run(buf);
    print_tokens(std::cout, query_tokens);
    Program query = parser.parse(query_tokens);
    // print_ast(std::cout, query);

    // Do we need to halt?
    if (interpreter.do_halt(query)) {
      std::cout << "\nHalted.\n";
      return 0;
    }

    if (!interpreter.interpret(prog, query)) {
      std::cout << "\nFalse\n";
    }
    std::cout << "? ";
  }
  return 0;
}

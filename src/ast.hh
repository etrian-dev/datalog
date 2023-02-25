#ifndef AST_HH_INCLUDED
#define AST_HH_INCLUDED

#include <string>

class AstNode;
class Program;
class Rule;
class Atom;
class Term;

template <class T> class AstVisitor {
public:
  T visit(Program &program);
  T visit(Rule &rule);
  T visit(Atom &atom);
  T visit(Term &term);
};

typedef AstVisitor<std::string> AstPrinter;
// typedef AstVisitor<AstNode> AstInterpreter;

#endif
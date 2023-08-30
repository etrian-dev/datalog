#ifndef AST_HH_INCLUDED
#define AST_HH_INCLUDED

#include <string>

class AstNode;
class Program;
class Rule;
class Atom;
class EvaluatedAtom;
class Term;
class EvaluatedTerm;

template <class T>
class AstVisitor {
public:
  T visit(Program &program);
  T visit(Rule &rule);
  T visit(EvaluatedAtom &eatom);
  T visit(Atom &atom);
  T visit(EvaluatedTerm &eterm);
  T visit(Term &term);
};

typedef AstVisitor<std::string> AstPrinter;
// typedef AstVisitor<AstNode> AstInterpreter;

#endif

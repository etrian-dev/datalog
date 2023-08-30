#include "parser.hh"
#include "lexer.hh"
#include "interpreter.hh"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>


Parser::
Parser(std::vector<Token> &token_list)
  : current(0) {
  tokens = token_list;
}

Parser::~
Parser() {
}

Token &
Parser::peek(void) {
  return tokens[current];
}

Token &
Parser::advance(void) {
  return tokens[current++];
}

Token &
Parser::previous(void) {
  if (current == 0UL) {
    throw ParseError("Cannot roll back the start of the token sequence");
  }
  return tokens[current - 1UL];
}

void
Parser::reset(std::vector<Token> &tokens, size_t pos) {
  this->tokens = tokens;
  this->current = pos;
}

bool
Parser::is_eof(Token &tok) {
  return tok.get_type() == TokenType::END_OF_FILE;
}

Program
Parser::parse(std::vector<Token> &tokens) {
  try {
    reset(tokens, 0);
    return parse_program();
  }
  catch (ParseError &error) {
    std::cout /*<< "\033[31m"*/ << error.what() /*<< "\033[0m"*/ << std::endl;
  }
  return Program();
}

Program
Parser::parse(void) {
  try {
    return parse_program();
  }
  catch (ParseError &error) {
    std::cout /*<< "\033[31m"*/ << error.what() /*<< "\033[0m"*/ << std::endl;
  }
  return Program();
}

Program
Parser::parse_program(void) {
  Program prog; // the empty program
  std::vector<Rule> rules;
  while (!is_eof(peek())) {
    Rule rule = parse_rule();
    // at the start of the new rule's token or EOF
    rules.push_back(rule);
  }
  prog = Program(rules);
  return prog;
}

Rule
Parser::parse_rule(void) {
  Atom head = parse_atom();
  // either at "." or at ":-"
  Token next = advance();
  if (next.get_type() == TokenType::DOT) {
    // then we're parsing a fact => we are done
    std::vector<Term> terms = head.get_terms();
    return Rule(head);
  }
  else if (next.get_type() == TokenType::COLON
           && peek().get_type() == TokenType::MINUS) {
    // then we're parsing a rule: more atoms to follow
    std::vector<Atom> goals;
    advance(); // skip the "-" or the ","
    do {
      Atom goal = parse_atom();
      goals.push_back(goal);
      next = advance();
    } while (next.get_type() == TokenType::COMMA);
    if (next.get_type() == TokenType::DOT) {
      return Rule(head, goals);
    }
    else {
      throw ParseError("Expected dot at ", next);
    }
  }
  throw ParseError("Expected dot or :- at ", next);
}

Atom
Parser::parse_atom(void) {
  Token relation = advance();
  if (relation.get_type() == TokenType::LITERAL) {
    std::string lexeme = relation.get_lexeme();

    // Just a fact (no parentheses)
    if (peek().get_type() == TokenType::DOT) {
      return Atom(lexeme);
    }

    Token next = advance(); // skip and save the lparen
    if (next.get_type() == TokenType::LPAREN) {
      // parsing a term list
      Term term = parse_term();
      std::vector<Term> terms{term};
      next = advance(); // skip and save the rparen or comma
      while (next.get_type() == TokenType::COMMA) {
        Term term = parse_term();
        terms.push_back(term);
        next = advance();
      }
      if (next.get_type() == TokenType::RPAREN) {
        return Atom(lexeme, terms);
      }
      else {
        throw ParseError("Expected ) at ", next);
      }
    }
    else {
      throw ParseError("Expected a term list or a dot at ", previous());
    }
  }
  throw ParseError("Expected a predicate at ", relation);
}

bool
is_var(std::string &lexeme) {
  return (!lexeme.empty()
          && (lexeme[0] == '_' || (lexeme[0] >= 'A' && lexeme[0] <= 'Z')));
}

Term
Parser::parse_term(void) {
  Token tok = advance();
  if (tok.get_type() == TokenType::LITERAL) {
    std::string lexeme = tok.get_lexeme();
    // FIXME: we actually need to check if it's valid identifier,
    // unless we do that at the lexer level
    if (is_var(lexeme)) {
      // then it's a variable
      return Term(lexeme, TermType::VARIABLE);
    }
    return Term(lexeme, TermType::CONSTANT);
  }
  throw ParseError("Expected a variable or constant at ", previous());
}

void
print_ast(std::ostream &stream, Program &ast) {
  AstPrinter visitor;
  stream << visitor.visit(ast);
}

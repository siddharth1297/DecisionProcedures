#ifndef __SCANNER_HPP__
#define __SCANNER_HPP__ 1

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "location.hh"
#include "parser.tab.hh"

namespace FE {

class Scanner : public yyFlexLexer {
public:
  Scanner(std::istream *in) : yyFlexLexer(in) {
    loc = new Parser::location_type();
  };

  // get rid of override virtual function warning
  using FlexLexer::yylex;

  virtual int yylex(Parser::semantic_type *const lval,
                    Parser::location_type *location);
  // YY_DECL defined in lexer.l
  // Method body created by flex in lexer.yy.cc

private:
  /* yyval ptr */
  Parser::semantic_type *yylval = nullptr;
  /* location ptr */
  Parser::location_type *loc = nullptr;
};

} // namespace FE

#endif /* END __SCANNER_HPP__ */

#ifndef __SCANNER_HPP__
#define __SCANNER_HPP__ 1

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "fe_parser.tab.hh"
#include "location.hh"

namespace FE {

class Scanner : public yyFlexLexer {
public:
  Scanner(std::istream *in) : yyFlexLexer(in){};
  virtual ~Scanner(){};

  // get rid of override virtual function warning
  using FlexLexer::yylex;

  virtual int yylex(Parser::semantic_type *const lval,
                    Parser::location_type *location);
  // YY_DECL defined in lexer.l
  // Method body created by flex in lexer.yy.cc

  std::string text;

private:
  /* yyval ptr */
  Parser::semantic_type *yylval = nullptr;
};

} // namespace FE

#endif /* END __SCANNER_HPP__ */

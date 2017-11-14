#include <ctype.h>

#include <iostream>

#include "parser.h"

using namespace std;

char Parser::nextChar() const
{
  if (pos <= last)
    {
      return str[pos];
    }
  return 0;
}

char Parser::getChar()
{
  if (pos > last)
    return 0;
  return str[pos++];
}

void Parser::skipChar()
{
  pos++;
}

void Parser::skipWhiteSpace()
{
  if (pos <= last && isblank(str[pos]))
    pos++;
}

void Parser::get_token()
{
  if (token.empty()) // token already supplied
    {
      skipWhiteSpace();
      char f = getChar();
      if (f != 0)
        {
          token.token += f;
          if (isdigit(f) || (f == '-'))
            {
              // token is number
              token.type = Token::integer; // assume "integer"
              // number
              f = nextChar();
              while (f == '.' || isdigit(f))
                {
                  if (f == '.')
                    token.type = Token::floatingpoint; // -> floating point
                  token.token += f;
                  skipChar();
                  f = nextChar();
                }
              if (token.token == "-")
                {
                  token.type = Token::singlecharacter;
                }
            }
          else if (f == '"' || f == '_')
            {
              // string literal starting with '"'
              // additionally '_' is accepted here for easier use on commandline
              // '_' is replaced with '"'
              char delimiter = f;
              token.token = "\"";
              token.type = Token::stringliteral;
              // string literal
              f = nextChar();
              while (f != delimiter)
                {
                  if (f == 0)
                    throw SdpException("Parsing", "string literal not complete");
                  token.token += f;
                  skipChar();
                  f = nextChar();
                }
              token.token += '"';
              skipChar();
            }
          else if (isalpha(f))
            {
              // identifier
              token.type = Token::identifier;
              f = nextChar();
              while (isalnum(f))
                {
                  token.token += f;
                  skipChar();
                  f = nextChar();
                }
            }
          else
            {
              token.type = Token::singlecharacter;
              // single character
            }
        }
      skipWhiteSpace();
    }
}

#ifndef SDP_PARSER_H
#define SDP_PARSER_H

#include <stdexcept>

#include <ctype.h>
#include <string>
#include "exceptions.h"

struct Token
{
  static const int nothing = 0;
  static const int integer = 1;
  static const int floatingpoint = 2;
  static const int identifier = 4;
  static const int stringliteral = 8;
  static const int singlecharacter = 16;

  std::string token;
  int type;

  int getInt()
  {
    return stoi(token);
  }

  double getFloat()
  {
    return stod(token);
  }
  std::string getString()
  {
    return token.substr(1, token.length() - 2);
  }

  bool empty() const
  {
    return type == nothing;
  }

  void clear()
  {
    type = nothing;
    token.clear();
  }
};

class Parser
{
public:

  Parser(const std::string& s, int start = 0):
    str(s), token(), pos(0), last(s.size() - 1)
  {
    while (last >= 0 && isblank(str[last]))
      --last;
  }

  virtual bool empty() const
  {
    return token.empty() && pos > last;
  }

  virtual Token nextToken()
  {
    get_token();
    return token;
  }

  virtual Token getToken()
  {
    get_token();
    Token result = token;
    token.clear();
    return result;
  }

  virtual Token getToken(int type)
  {
    get_token();
    if ((token.type & type) != token.type)
      throw SdpException("Parsing", "Expected token of type " + std::to_string(type) + " got " + token.token);
    return getToken();
  }

  virtual void expect(const std::string& tok)
  {
    get_token();
    if (token.token != tok)
      throw SdpException("Parsing", "Expected " + tok);
    token.clear();
  }

  virtual void expect(char tok)
  {
    get_token();
    if (token.type != Token::singlecharacter || token.token[0] != tok)
      throw SdpException("Parsing", std::string("Expected ") + tok);
    token.clear();
  }

  virtual void skipToken()
  {
    if (token.empty())
      get_token();
    token.clear();
  }

  virtual std::string String() const
  {
    return str;
  }

private:
  virtual char nextChar() const;
  virtual char getChar();
  virtual void skipChar();
  virtual void skipWhiteSpace();

  virtual void get_token();

  std::string str;
  Token token;

  int pos;
  int last;
};

#endif

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include <ctype.h>

#include <dirfunc.h>
#include <strtool.h>
#include <IceException.h>

using namespace std;
using namespace ice;

class Lexer
{
public:
  static const int nothing = 0;
  static const int integer = 1;
  static const int floatingpoint = 2;
  static const int identifier = 4;
  static const int stringliteral = 8;
  static const int singlecharacter = 16;
  static const int comment = 32;
  Lexer(const std::string& s, bool insideComment = false):
    str(s), pos(0), insideComment(insideComment)
  {
    nextToken();
  }

  // all handled
  virtual bool empty() const
  {
    return token.empty() && pos >= str.size();
  }

  // get next token
  virtual void nextToken();

  virtual std::string getAll()
  {
    std::string res = str.substr(pos, str.size() - pos);
    pos = str.size();
    return res;
  }

  virtual long int getInt();
  virtual double getDouble();
  virtual std::string getString();
  virtual std::string getWord();

  // expect and remove specific token (string)
  virtual void expect(const std::string& tok)
  {
    if (token != tok)
      throw IceException("Parsing", "Expected " + tok);
    nextToken();
  }

  // expect and remove specific token (singlecharacter)
  virtual void expect(char tok)
  {
    if (type != singlecharacter || token[0] != tok)
      throw IceException("Parsing", std::string("Expected ") + tok);
    nextToken();
  }

  virtual std::string getString() const
  {
    return str;
  }

  std::string token;
  int type;

private:
  virtual char nextChar() const;
  virtual char getChar();
  virtual void skipChar();
  virtual void skipWhiteSpace();

  //  virtual void get_token();

  std::string str;
  unsigned int pos;
public:
  bool insideComment;
};

char Lexer::nextChar() const
{
  if (pos < str.size())
    return str[pos];
  return 0;
}

char Lexer::getChar()
{
  if (pos < str.size())
    return str[pos++];
  return 0;
}

void Lexer::skipChar()
{
  pos++;
}

void Lexer::skipWhiteSpace()
{
  if (pos < str.size() && isblank(str[pos]))
    pos++;
}

void Lexer::nextToken()
{
  token.clear();
  type = nothing;
  skipWhiteSpace();
  char f = getChar();
  if (f != 0)
    {
      token += f;
      if (insideComment)
        {
          // f = nextChar();
          // skipChar();
          token = ' ';
          while (f != 0 && (f != '*' || nextChar() != '/'))
            {
              token += f;
              f = nextChar();
              skipChar();
            }
          if (f == '*')
            {
              token += "*/";
              skipChar();
              insideComment = false;
            }
          else
            insideComment = true;
        }
      else if (isdigit(f))
        {
          // token is number
          type = integer; // assume "integer"
          // number
          f = nextChar();
          while (isdigit(f) || f == '.')
            {
              if (f == '.')
                {
                  type = floatingpoint;  // -> floating point
                }
              token += f;
              skipChar();
              f = nextChar();
            }
        }
      else if (f == '"')
        {
          // string literal starting with '"'
          char delimiter = f;
          token = "\"";
          type = stringliteral;

          f = nextChar();
          while (f != delimiter)
            {
              if (f == 0)
                throw IceException("Parsing", "string literal not complete");
              if (f == '\\') // escape
                {
                  skipChar();
                  f = nextChar();
                  token += '\\';
                }
              token += f;
              skipChar();
              f = nextChar();
            }
          token += '"';
          skipChar();
        }
      else if (f == '/' && nextChar() == '/')
        {
          // one line comment
          skipChar();
          while (f != 0)
            {
              token += f;
              f = nextChar();
              skipChar();
            }
          type = comment;
        }
      else if (f == '/' && nextChar() == '*')
        {
          skipChar();
          type = comment;
          token = "/*";
          f = nextChar();
          skipChar();
          while (f != 0 && (f != '*' || nextChar() != '/'))
            {
              token += f;
              f = nextChar();
              skipChar();
            }
          if (f == '*')
            {
              token += "*/";
              skipChar();
            }
          else
            insideComment = true;
        }
      else if (isalpha(f) || f == '_')
        {
          //    cout << "id"<<endl;
          // identifier
          type = identifier;
          f = nextChar();
          //    cout << (int)f << " " << f << endl;
          while (isalnum(f) || f == '_')
            {
              token += f;
              skipChar();
              f = nextChar();
            }
        }
      else
        {
          type = singlecharacter;
          // single character
        }
    }
  skipWhiteSpace();
}

long int Lexer::getInt()
{
  // cout << "type: " << type << " token:" << token << endl;
  if (type != integer)
    throw IceException("getInt", "integer value expected");
  int res = stol(token);
  nextToken();
  return res;
}

double Lexer::getDouble()
{
  if (type != floatingpoint)
    throw IceException("getDouble", "floating point value expected");
  double res = stod(token);
  nextToken();
  return res;
}

std::string Lexer::getString()
{
  if (type != stringliteral)
    throw IceException("getString", "string expected");
  string res = token.substr(1, token.length() - 2);
  nextToken();
  return res;
}

std::string Lexer::getWord()
{
  if (type != identifier)
    throw IceException("getWord", "identifier or keyword expected");
  string res = token;
  nextToken();
  return res;
}

void read(const string& fn, vector<string>& buffer)
{
  ifstream is(fn);
  string line;
  while (getline(is, line))
    buffer.push_back(line);
}

void write(const string& fn, const vector<string>& buffer)
{
  ofstream os(fn);
  int nLines = buffer.size();
  int nEmptyLines = 0;
  for (int lineNr = 0; lineNr < nLines; lineNr++)
    {
      string line = buffer[lineNr];
      if (line != "////") // ignore these
        {
          if (line.empty())
            {
              nEmptyLines++;
              if (nEmptyLines < 2)
                os << endl;
            }
          else
            {
              nEmptyLines = 0;
              os << line << endl;
            }
        }
    }
}

bool leftMatch(const string& line, const string& pattern)
{
  int len = pattern.size();
  return line.substr(0, len) == pattern;
}

void getIceException(Lexer& lex, string& arg1, string& arg2)
{
  while (!lex.empty() && lex.type == Lexer::comment)
    lex.nextToken();
  lex.expect("throw");
  lex.expect("IceException");
  lex.expect('(');
  arg1 = lex.getWord();
  lex.expect(',');
  arg2 = lex.getWord();
  if (lex.token == ",")
    {
      lex.nextToken();
      lex.getWord();
    }
  lex.expect(')');
}

void convert(const string& fn)
{
  vector<string> buffer;
  read(fn, buffer);
  int nLines = buffer.size();
  vector<string> newBuffer(nLines);
  for (int lineNr = 0; lineNr < nLines; lineNr++)
    {
      newBuffer[lineNr] = buffer[lineNr];
      buffer[lineNr] = trim(buffer[lineNr]);
    }
  bool comment = false;
  bool mustWrite = false;
  for (int lineNr = 0; lineNr < nLines; lineNr++)
    {
      string hline = buffer[lineNr];
      Lexer lex(hline, comment);
      try
        {
          string arg1, arg2;
          getIceException(lex, arg1, arg2);
          /* we do not need this again
          if (lineNr>1 && lineNr<nLines-1)
            {
              if (buffer[lineNr-1]=="{" && buffer[lineNr+1]=="}")
                {
            newBuffer[lineNr-1]="////";
            newBuffer[lineNr+1]="////";
                }
            }
          */
          newBuffer[lineNr] = "throw IceException(" + arg1 + "," + arg2 + ");";
          cout << newBuffer[lineNr] << endl;
          mustWrite = true;
        }
      catch (IceException& ex)
        {
          // ignore
        }

      while (!lex.empty())
        lex.nextToken();
      comment = lex.insideComment;
    }
  if (mustWrite)
    {
      // store backup
      string backupFilename = fn + ".iceb";
      rename(fn.c_str(), backupFilename.c_str());
      // write new file
      write(fn, newBuffer);
    }
}

int main(int argc, char** argv)
{
  try
    {
      vector<string> files;
      Directory(files, "/home/noo/ice/src", "*.cpp", DIR_FILE | DIR_WITHPATH);
      Directory(files, "/home/noo/ice/src", "*.h", DIR_FILE | DIR_WITHPATH);
      Directory(files, "/home/noo/ice/src/visual", "*.cpp", DIR_FILE | DIR_WITHPATH);
      Directory(files, "/home/noo/ice/src/visual", "*.h", DIR_FILE | DIR_WITHPATH);
      Directory(files, "/home/noo/ice/src/ocr", "*.cpp", DIR_FILE | DIR_WITHPATH);
      Directory(files, "/home/noo/ice/src/ocr", "*.h", DIR_FILE | DIR_WITHPATH);
      Directory(files, "/home/noo/ice/src/devices", "*.cpp", DIR_FILE | DIR_WITHPATH);
      Directory(files, "/home/noo/ice/src/devices", "*.h", DIR_FILE | DIR_WITHPATH);
      for (auto file : files)
        {
          //      cout << file << endl;
          convert(file);
        }
    }
  catch (IceException& ex)
    {
      cerr << ex.what() <<  endl;
    }
}

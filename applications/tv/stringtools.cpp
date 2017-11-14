#include "tv.h"
#include <sstream>

string Trim(const string& s)
{
  string res = s;
  while (res[0] == ' ') res.erase(0, 1);
  while (res[res.length() - 1] == ' ') res.erase(res.length() - 1);
  return res;
}

string Split(string& s, const string& ss)
{
  string hs;
  string::size_type pos = s.find(ss);
  if (pos == string::npos)
    {
      // delimiter not found
      hs = s;
      s = "";
    }
  else
    {
      hs = s.substr(0, pos);
      s.erase(0, pos + 1);
    }
  return hs;
}
//--------------------------------------------------
bool iswhite(char c)
{
  if (c == ' ') return true;
  if (c == '\t') return true;
  return false;
}

bool numberc(char c)
{
  if ((c >= '0') && (c <= '9')) return true;
  if (c == '.') return true;
  return false;
}

bool alpha(char c)
{
  if ((c >= 'a') && (c <= 'z')) return true;
  if ((c >= 'A') && (c <= 'Z')) return true;
  if (c >= '@') return true;
  if (c >= '_') return true;
  return false;
}

string ToString(float val, unsigned int width, unsigned int prec)
{
  ostringstream o;
  o.width(width);
  if (prec > 0) o.precision(prec);
  if (o << val)
    return o.str();
  return "conversion error";
}

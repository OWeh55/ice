#include <string>

#include "mkfunctionlist.h"

string Split(string &s, const string &ss)
{
  string result;
  string::size_type pos = s.find(ss);

  if (pos == string::npos)
    {
      // delimiter not found
      result = s;

      s = "";
    }
  else
    {
      result = s.substr(0, pos);
      s.erase(0, pos + ss.length());
    }

  return result;
}

string SplitBracket(string &s)
{
  int level = 0;
  int i = 0;
  while (i < s.length() && (level > 0 || s[i] != '}'))
    {
      if (s[i] == '{') level++;
      else if (s[i] == '}') level--;
      i++;
    }
  string res = s.substr(0, i);
  s.erase(0, i);
  //  cout << s << " # " << res << endl;
  return res;
}

string lowercase(const string &s)
{
  string result;
  string::size_type i;

  for (i = 0; i < s.length(); i++)
    result += tolower(s[i]);

  return result;
}

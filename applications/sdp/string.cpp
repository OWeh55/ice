#include <ctype.h>

#include <iostream>

#include "string.h"

using namespace std;

bool stringMatch(const string& s1, const string& s2)
{
  if (s2.length() < s1.length())
    return false;
  bool match = true;
  for (unsigned int i = 0; i < s1.length() && match; ++i)
    {
      match = tolower(s1[i]) == tolower(s2[i]);
    }
  return match;
}

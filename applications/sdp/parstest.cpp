#include <iostream>
#include <string>

#include "parser.h"

using namespace std;

int main()
{
  string input;
  cout << "Text-Zeilen eingeben:" << endl;
  getline(cin, input);
  while (!input.empty())
    {
      Parser pp(input);
      while (!pp.empty())
        {
          Token part = pp.getToken();
          cout << part.token << " = " << part.type << endl;
        }
      getline(cin, input);
    }
}

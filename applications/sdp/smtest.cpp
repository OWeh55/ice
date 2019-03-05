#include <iostream>
#include <string>
#include "exceptions.h"
#include "stackmachine.h"

using namespace std;

int main()
{
  string input;
  while (true)
    {
      getline(cin, input);
      try
        {
	  vector<int> var(6);
          StackMachine sm(input);
          cout << sm.evaluate(var, 111, 222, 333) << endl;
        }
      catch (SdpException &ex)
        {
          cout << "Exception: " << ex.what() << endl;
        }
    }
}

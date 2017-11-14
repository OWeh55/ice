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
          StackMachine sm(input);
          cout << sm.evaluate(111, 222, 333, 444) << endl;
        }
      catch (SdpException ex)
        {
          cout << "Exception: " << ex.what() << endl;
        }
    }
}

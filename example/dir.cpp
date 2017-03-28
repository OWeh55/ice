#include <stdlib.h>
#include <image.h>

using namespace ice;

int main(int argc, char* argv[])
{
  vector<string> z;
  vector<string>::const_iterator p;
  Directory(z, "*");
//  Directory(z,"*.o");
  PutChar('a');

  for (p = z.begin(); p != z.end(); p++)
    cout << "-> " << *p << endl;

  return 1;
}

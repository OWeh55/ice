#include <string>
#include <iostream>

#include "ptu.h"
#include "serial.h"
#include "strtool.h"

using namespace std;

namespace ice
{
  int ptOpen(int port)
  {
    return srlOpen(port, 8, 0, 1, 9600, srlFLOWHARD, 10);
  }

  void ptClose(int port)
  {
    srlClose(port);
  }

  bool ptcommand(int port, const string& s)
  {
    int c;
    srlWriteString(port, (s + " ").c_str());

    do
      {
        c = srlRead(port);
//    cout << char(c) ;
      }
    while ((c != '*') && (c != '!'));
//    cout << endl;
    return (c == '*');
  }

  int ptMove(int port, int l, int b)
  {
    bool rc;
    rc = ptcommand(port, "pp" + NumberString(l));
    if (!rc)
      {
        return false;
      }
    return ptcommand(port, "tp" + NumberString(b));
  }

  int ptReset(int port)
  {
    int c;

    if (ptcommand(port, "R"))
      {
        return true;
      }

    // during reset a number of false negative acks have to be ignored
    do
      {
        c = srlRead(port);
//    cout << char(c) ;
      }
    while ((c != '*'));
//    cout << endl;
    return true;

  }

  int ptHome(int port)
  {
    return ptMove(port, 0, 0);
  }

  int ptWaitStop(int port)
  {
    return ptcommand(port, "a");
  }

}

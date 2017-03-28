#include <image.h>
// dies ist ein Test für den Laufzeitunterschied zwischen der Parameterübergabe
// eines Image per Referenz bzw. über Wert.

int wert(Image img, int x, int y)
{
  return GetVal(img, x, y);
}

int ref(const Image& img, int x, int y)
{
  return GetVal(img, x, y);
}

void start(double& t)
{
  t = TimeD();
}

double stop(double& t)
{
  return TimeD() - t;
}

void PrintT(double& t, const string& s)
{
  Print(s);
  Print(NumberString(stop(t)));
  Print("\n");
}

int main(int argc, char* argv[])
{
//  Alpha(ON);
  Image img;
  img.create(5120, 5120, 255);
  int x, y;

  wloop(img, x, y)
  {
    PutVal(img, x, y, 0);
  }

  double ta;
  Print("Teste..\n");
  start(ta);
  wloop(img, x, y) wert(img, x, y);
  PrintT(ta, "Wert: ");
  start(ta);
  wloop(img, x, y) ref(img, x, y);
  PrintT(ta, "Referenz: ");
//  GetChar();
  Print("Teste..\n");
  start(ta);
  wloop(img, x, y) wert(img, x, y);
  PrintT(ta, "Wert: ");
  start(ta);
  wloop(img, x, y) ref(img, x, y);
  PrintT(ta, "Referenz: ");
  GetChar();
  return OK;
}

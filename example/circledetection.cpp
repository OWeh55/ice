#include <image.h>

LineSeg InputLine(const Image& img, const string& p)
{
  LineSeg res;
  Print("Eingabe " + p + "\n");
  IPoint p1 = SelPoint(img);
  Marker(1, p1, 3, 5, img);
  IPoint p2 = SelPoint(img);
  Marker(1, p2, 3, 5, img);
  Print("G - Gerade\n");
  Print("A - Strahl\n");
  Print("I - Inverser Strahl\n");
  Print("S - Strecke\n");
  char c = GetChar();

  switch (c)
    {
    case 'G':
    case 'g':
      res = LineSeg(p1, p2, LineSeg::line);
      break;
    case 'A':
    case 'a':
      res = LineSeg(p1, p2, LineSeg::ray);
      break;
    case 'I':
    case 'i':
      res = LineSeg(p1, p2, LineSeg::inv_ray);
      break;
    case 'S':
    case 's':
    default:
      res = LineSeg(p1, p2, LineSeg::segment);
      break;
    }

  Marker(1, p1, 0, 5, img);
  Marker(1, p2, 0, 5, img);
  return res;
}

bool InputCircle(const Image& img, const string& p,Circle &c)
{
  Print("Eingabe " + p + "\n");
  Print("Eingabe Mittelpunkt \n");
  IPoint p1 = SelPoint(img);
  Marker(1, p1, 3, 5, img);
  Print("Eingabe Randpunkt \n");
  int rc;
  IPoint p2 = SelPoint(img,rc);
  if (rc>=0)
    {
      Marker(1, p1, 0, 5, img);
      c  = Circle(p1, (p2 - p1).r());
    }
  else
    c=Circle();
  return (rc>=0);
}

int main(int argc, char** argv)
{
  Image img;
  img.create(1024, 1024, 255);
  Image mrk;
  mrk.create(1024, 1024, 8);
  clearImg(img);
  clearImg(mrk);
  Show(OVERLAY, img, mrk);

  vector<Circle> circles;
  Circle c;
  while (InputCircle(mrk,"Kreis",c))
    {
      circles.push_back(c);
      draw(c,mrk,1);
    }

  while (GetChar() != 13) /* wait*/;
  
  return OK;
}

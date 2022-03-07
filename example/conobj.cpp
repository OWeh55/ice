#include <image.h>

// test der Konturfunktionen mit geometrischen Objekten

using namespace ice;

void Fill(const GeoObject& g, const Image& img, int value)
{
  for (int y = 0; y < img.ysize; y++)
    for (int x = 0; x < img.xsize; x++)
      if (g.Inside(x, y))
        img.setPixel(x, y, value);
}

Contur findContur(const GeoObject& g, IPoint start)
{
  Contur result;
  GeoObjectObjectFunctor gf(g);
  if (SearchStart(gf, start)) // startpunkt gefunden ?
    {
      result = CalcContur(gf, start); // Konturfolge
    }
  else
    cout << "Kein Startpunkt" << endl;
  return result;
}

void nDraw(const GeoObject& g, const Image& mark, int value)
{
  IPoint start;
  cout << g.getPos() ;
  if (g.getInsidePoint(start))
    {
      cout << " " << start ;
      Contur c = findContur(g, start);
      if (c.isValid())
        MarkContur(c, value, mark);
    }
  cout << endl;
}

/************************************************/
int main(int argc, char* argv[])
{
  Contur c;
  bool fill = false;
  double rad = 100.5;

  string fn = "";
  Point center(125, 125);

  int rc;

  while ((rc = getopt(argc, argv, "r:fx:y:")) >= 0)
    {
      switch (rc)
        {
        case 'r':
          rad = atof(optarg);
          break;
        case 'f':
          fill = true;
          break;
        case 'x':
          center.x = atof(optarg);
          break;
        case 'y':
          center.y = atof(optarg);
          break;
        }
    }

  if (argc - optind > 0)
    fn = argv[optind];

  Image img = NewImg(1111, 999, 255);
  setImg(img, 177);
  Image imgm = NewImg(img->xsize, img->ysize, 3);
  clearImg(imgm);

  Show(OVERLAY2, img, imgm);

  //  CircleSeg circle(center,rad,0.3,5.0);
  Circle circle(center, rad);

  Fill(circle, img, 127);
  nDraw(circle, imgm, 1);

  Point p1(600, 423);
  Point p2(500, 100);
  Point p3(200, 500);
  Triangle triangle(p1, p2, p3);

  Fill(triangle, img, 127);
  nDraw(triangle, imgm, 1);

  Point center2(800, 300);
  CircleSeg circleSeg(center2, 200, 0.3, 5.0);

  Fill(circleSeg, img, 127);
  nDraw(circleSeg, imgm, 1);

  Point center3(800, 700);
  Ellipse ellipse(center3, 300, 100, 2.3);

  Fill(ellipse, img, 100);
  nDraw(ellipse, imgm, 1);

  Point center4(300, 700);
  EllipseSeg ellipseSeg(center4, 300, 100, -0.73, 0.5, -0.2);

  Fill(ellipseSeg, img, 10);
  nDraw(ellipseSeg, imgm, 1);

  Point pp1(40, 400);
  Point pp2(50, 700);
  Point pp3(111, 400);
  Point pp4(333, 233);

  Polygon poly(pp1);
  poly.add(pp2);
  poly.add(pp3);
  poly.add(pp4);

  Fill(poly, img, 110);
  nDraw(poly, imgm, 1);

  GetChar();
  return OK;
}



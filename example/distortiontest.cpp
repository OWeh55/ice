#include <image_nonvis.h>

#define COUNT 100
#define HNUMBER 100

#define X0 5
#define Y0 5

#define D2 2e-7
#define D4 2e-16

void DiTest(Distortion& di)
{
  double x = 0;
  double y = 100;
  double x1;
  double y1;
  double x2;
  double y2;

  cout << "Gegebene Verzeichnung: " << di.toString() << endl;
  cout << "Original: " << x << " " << y << endl;
  di.distort(x, y, x1, y1);
  cout << "Verzeichnet: " << x1 << " " << y1 << endl;
  di.rectify(x1, y1, x2, y2);
  cout << "Entzerrt: " << x2 << " " << y2 << endl;
  cout << "Fehler: " << x - x2 << " " << y - y2 << endl;

  double fsum = 0.0;
  double fmax = 0.0;
  double ta = TimeD(TM_PROCESS);

  vector<Point> messung;
  vector<Point> orig;

  for (int n = 0; n < COUNT; n++)
    for (int i = -HNUMBER; i < HNUMBER; i++)
      for (int j = -HNUMBER; j < HNUMBER; j++)
        {
          x = (50.0 + i * 50.0 / HNUMBER);
          y = (50.0 + j * 50.0 / HNUMBER);
          di.distort(x, y, x1, y1);
          di.rectify(x1, y1, x2, y2);
          double dx = x2 - x;
          double dy = y2 - y;
          double fehler2 = dx * dx + dy * dy;

          if (fehler2 > fmax)
            {
              fmax = fehler2;
              // cout << "(" << x << "," << y << ") (" << x2 << "," << y2 << ") ";
              // cout << fehler2 << endl;
            }

          fsum += fehler2;
          /*
          if (n==0)
            {
              messung.Append(Vector(x1,y1));
              orig.Append(Vector(x,y));
            }
          */
        }

  cout << "Zeit: " << TimeD(TM_PROCESS) - ta << endl;
  cout << "mittlerer Fehler (verzeichnen+entzerren): " << sqrt(fsum / (2 * HNUMBER) / (2 * HNUMBER) / COUNT) << endl ;
  cout << "maximaler Fehler (verzeichnen+entzerren): " << sqrt(fmax) << endl ;

  for (double r = 3; r < 300; r += 3.0)
    for (double fi = 0; fi < 6.28; fi += 0.2)
      {
        double x = r * sin(fi) + X0;
        double y = r * cos(fi) + Y0;
        orig.push_back(Point(x, y));
        double fr = 1.0 + D2 * r * r + D4 * r * r * r * r;
        double xd = r * fr * sin(fi) + X0 /*+ RandomD() - 0.5 */;
        double yd = r * fr * cos(fi) + Y0 /*+ RandomD()*/;
        messung.push_back(Point(xd, yd));
      }

  //  cout << orig << endl;
  //  cout << messung << endl;

  //  cout << "Vorgabe: " << di.toString() << endl;
  ta = TimeD(TM_PROCESS);
  di.calculate(messung, orig);
  cout << "Zeit Calc: " << TimeD(TM_PROCESS) - ta << endl;
  cout << "Calc:" << di.toString() << endl;
  cout << "------------------------------------------------------" << endl;
}

int main(int argc, char** argv)
{
  Distortion0 di0(50, 50, 1e-6);
  Distortion1 di1(50, 50, 1e-6,        1e-14);
  Distortion2 di2(50, 50, 1e-6, 1e-11, 1e-14);
  Distortion3 di3(50, 50, 1e-6,        1e-14, 1e-20);

  DiTest(di0);
  DiTest(di1);
  DiTest(di2);
  DiTest(di3);

  return OK;
}

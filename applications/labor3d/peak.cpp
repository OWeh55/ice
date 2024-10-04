#include <iostream>
#include <vector>
#include <lmdif.h>

#include <queue>

#include "peak.h"

using namespace ice;
using namespace std;

class GaussReg: public ice::LMFunctionObject
{
private:
// Referenzen auf die dem Konstruktor übergebenen Parameter
  const Matrix3d& m;
  ice::Vector& gausspara; // Parameter der Gauss-Funktion
  // inhalt des vektors: my_x,my_y,my_z,1/sigma_x,1/sigma_y,1/sigma_z,faktor
public:
  // Konstruktor
  // übernimmt (als Referenz) die zu optimierenden
  // Parameter der Gauss-Funktion und die Matrix
  GaussReg(ice::Vector& gp, const Matrix3d& mp):
    m(mp), gausspara(gp) { } ;
  // Dimension der Fehlerfunktion
  int funcdim() const
  {
    // std::cout << m.sizeX()*m.sizeY()*m.sizeZ() << std::endl;    // Ein Fehlerwert pro Voxel
    return m.sizeX() * m.sizeY() * m.sizeZ(); // Ein Fehlerwert pro Voxel
  }
  // Fehler-Funktion
  int operator()(ice::Vector& result) const
  {
    int i = 0;
    for (int z = 0; z < m.sizeZ(); z++)
      {
        double rz = (z - gausspara[2]) * gausspara[5];
        double rz2 = rz * rz;
        for (int y = 0; y < m.sizeY(); y++)
          {
            double ry = (y - gausspara[1]) * gausspara[4];
            double ryz2 = ry * ry + rz2;
            for (int x = 0; x < m.sizeX(); x++)
              {
                double rx = (x - gausspara[0]) * gausspara[3];
                double rxyz2 = rx * rx + ryz2;
                double fval = exp(-0.5 * rxyz2) * gausspara[6];
                double val = m(x, y, z);
                result[i] = val - fval;
                i++;
              }
          }
      }
    return 1;
  }
};

double PeakEvaluation(const Matrix3d& m, double xm, double ym, double zm)
{
  int inumber = 10000;

  ice::Vector n(7); // Parameter der Gauss-Funktion

  // Startwerte
  n[0] = xm;
  n[1] = ym;
  n[2] = zm;
  n[3] = 1;
  n[4] = 1;
  n[5] = 1;
  n[6] = m(xm, ym, zm);

  std::vector<double*> op(7);  // Pointer auf zu optimierende Parameter

  for (int i = 0; i < 7; i++)
    op[i] = &n[i];

  GaussReg gf(n, m); // Konstruktion des Funktors

  int info = LMDif(op,      // Liste von Zeigern auf zu optimierende Parameter
                   gf,        // Funktor der Fehlerfunktion
                   1000,      // maximale Zahl der Iterationsschritte
                   inumber);  // Rückgabe der durchgeführten Iterationsschritte

  std::cout << "Nach " << inumber << " Iterationen: " << info << std::endl;
  for (int i = 0; i < 7; i++)
    std::cout << n[i] << std::endl;

  ice::Vector diff(gf.funcdim());
  gf(diff);

  double errorsum = 0.0;
  double funcsum = 0.0;

  for (int z = 0; z < m.sizeZ(); z++)
    {
      double rz = (z - n[2]) * n[5];
      double rz2 = rz * rz;
      for (int y = 0; y < m.sizeY(); y++)
        {
          double ry = (y - n[1]) * n[4];
          double ryz2 = ry * ry + rz2;
          for (int x = 0; x < m.sizeX(); x++)
            {
              double rx = (x - n[0]) * n[3];
              double rxyz2 = rx * rx + ryz2;
              double fval = exp(-0.5 * rxyz2) * n[6];
              double error = m(x, y, z) - fval;
              funcsum += fval;
              errorsum += error * error;
            }
        }

    }
  std::cout << sqrt(errorsum) << " / " << funcsum << std::endl;
  return sqrt(errorsum) / funcsum;
}

struct PointP: public IVector3d
{
  // Point with value and direction
  double val;

  PointP(int x, int y, int z, double v): IVector3d(x, y, z), val(v) {}
  PointP(IVector3d s, double v): IVector3d(s), val(v) {}
  bool operator<(const PointP& second) const
  {
    return val < second.val;
  }
};

void push(priority_queue<PointP>& heap, IVector3d p, const Image3d<Image>& val, Image3d<Image>& mark)
{
  if (mark.inside(p))
    if (mark.getPixel(p) == 200) // unbearbeitet
      {
        heap.push(PointP(p, val.getPixel(p)));
        mark.setPixel(p, 127); // in Queue
      }
}

int getVal(const Image3d<Image>& val, int x, int y, int z, const Image3d<Image>& mark)
{
  //  cout << "getVal: " << x << ", " << y << ", "<< z << endl;
  if (!val.inside(x, y, z)) return 0;
  if (mark.getPixel(x, y, z) == 0)
    return 0;
  return val.getPixel(x, y, z);
}

bool isMax(const Image3d<Image>& val, IVector3d p, const Image3d<Image>& mark)
{
  int x = p.x;
  int y = p.y;
  int z = p.z;
  int v = val.getPixel(p);
  if (getVal(val, x - 1, y, z, mark) <= v &&
      getVal(val, x + 1, y, z, mark) <= v &&
      getVal(val, x, y - 1, z, mark) <= v &&
      getVal(val, x, y + 1, z, mark) <= v &&
      getVal(val, x, y, z - 1, mark) <= v &&
      getVal(val, x, y, z + 1, mark) <= v)

    return true;
  return false;
}

IVector3d X(1, 0, 0);
IVector3d Y(0, 1, 0);
IVector3d Z(0, 0, 1);

int markMax(const Image3d<Image>& val, Image3d<Image>& mark, IVector3d p)
{
  priority_queue<PointP> heap;
  int max = val.getPixel(p);
  int min = max;
  heap.push(PointP(p, max));
  while (!heap.empty())
    {
      PointP ap = heap.top();
      heap.pop();

      if (isMax(val, ap, mark))
        {
          mark.setPixel(ap, 0);
          min = val.getPixel(ap);
          push(heap, ap - X, val, mark);
          push(heap, ap + X, val, mark);
          push(heap, ap - Y, val, mark);
          push(heap, ap + Y, val, mark);
          push(heap, ap - Z, val, mark);
          push(heap, ap + Y, val, mark);
        }
      else
        mark.setPixel(ap, 255);
    }
  return max - min;
}

bool isMax(const Image3d<Image>& val, IVector3d p)
{
  int x = p.x;
  int y = p.y;
  int z = p.z;
  int v = val.getPixel(p);
  if (val.getPixel(x - 1, y, z) <= v &&
      val.getPixel(x + 1, y, z) <= v &&
      val.getPixel(x, y - 1, z) <= v &&
      val.getPixel(x, y + 1, z) <= v &&
      val.getPixel(x, y, z - 1) <= v &&
      val.getPixel(x, y, z + 1) <= v)

    return true;
  return false;
}

#include <image.h>

#define SANZ 256

#define FNAME "hpc"
int hpc(const Contur& c, Matrix& cm, int steps = SANZ)
{
  Matrix cpl, hpl;
  double cxs, cys;
  double xys[2], mom[15];

  if (!c.isValid() || !c.isClosed())
    {
      throw IceException(FNAME, M_CONTUR_NOT_CLOSED);
    }

  MomentRegion(c, mom, xys);
  cxs = xys[0];
  cys = xys[1];

  cpl = ConturPointlist(c);
  cm = Matrix(SANZ, 2);

  for (int i = 0; i < SANZ; i++)
    {
      double phi = M_PI * 2 / SANZ * i;
      double xyh[2];
      double xh, yh;
      ConvPolarCartes(1.0, phi, xyh);
      xh = xyh[0];
      yh = xyh[1];
      hpl = Matrix(0, 2);

      for (int j = 0; j < cpl.rows(); j++)
        {
          double cross = (cpl[j][0] - cxs) * yh + (cpl[j][1] - cys) * xh;

          if (cross > 0)
            {
              hpl.append(cpl[j]);
            }
        }

      MomentPolygon(hpl, mom, xyh);
      cm[i][0] = xyh[0];
      cm[i][1] = xyh[1];
    }

  return OK;
}
#undef FNAME

int main(int argv, char** argc)
{
  Contur c;
  Image mark;
  mark = NewImg(800, 600, 255);
  clearImg(mark);
  Display(ON);
  Show(OVERLAY, mark);
  Matrix cpl;
  Matrix hpl;
  Matrix sxy(SANZ, 2);
  int color = 1;

  while (true)
    {
      c = SelContur(mark, TRUE);
      hpc(c, sxy);
      MarkContur(c, color, mark);
      DrawPolygon(sxy, color, mark);
      color++;
    }

  GetChar();

  return 0;

}




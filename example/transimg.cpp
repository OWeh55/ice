#include <image.h>

using namespace std;

double starttime;

void Start()
{
  Delay(1.0);
  starttime = TimeD();
}

string Stop(Image i)
{
  double tm = TimeD() - starttime;
  double ptm = tm / i->xsize / i->ysize * 1E6;
  return NumberString(tm) + " s = " + NumberString(ptm) + " µs pro Pixel";
}

int main(int argc, char* argv[])
{
  Image i1, i2;
  OpenAlpha("Terminal");
  Alpha(ON);
  ClearAlpha();
  //  Display(ON);

  double t[3][3] = { {1, 0.1, 10},
    { -0.1, 1, 10},
    {0, 0, 1}
  };

  i1 = ReadImg("lorna.jpg");
  i2 = NewImg(i1, true);
//    Show(ON,i1);
//    Show(ON,i2);
  Start();
  TransAffineImg(i1, t, DEFAULT, i2);
  Print("Affine, DEFAULT, ralf : " + Stop(i2) + "\n");
  Start();
  TransImg(i1, t, DEFAULT, i2);
  Print("Projektiv, DEFAULT, ralf : " + Stop(i2) + "\n");
  Start();
  TransAffineImg(i1, t, INTERPOL, i2);
  Print("Affine, INTERPOL, ralf : " + Stop(i2) + "\n");
  Start();
  TransImg(i1, t, INTERPOL, i2);
  Print("Projektiv, INTERPOL, ralf : " + Stop(i2) + "\n");

  i1 = ReadImg("lorna.jpg");
  i2 = NewImg(i1, true);
//    Show(ON,i1);
//    Show(ON,i2);
  Start();
  TransAffineImg(i1, t, DEFAULT, i2);
  Print("Affine, DEFAULT, dcp_2727 : " + Stop(i2) + "\n");
  Start();
  TransImg(i1, t, DEFAULT, i2);
  Print("Projektiv, DEFAULT, dcp_2727 : " + Stop(i2) + "\n");
  Start();
  TransAffineImg(i1, t, INTERPOL, i2);
  Print("Affine, INTERPOL, dcp_2727 : " + Stop(i2) + "\n");
  Start();
  TransImg(i1, t, INTERPOL, i2);
  Print("Projektiv, INTERPOL, dcp_2727 : " + Stop(i2) + "\n");

  i1 = ReadImg("lorna.jpg");
  i2 = NewImg(i1, true);
  Show(ON, i1);
  Show(ON, i2);
  Start();
  TransAffineImg(i1, t, DEFAULT, i2);
  Print("Affine, DEFAULT, s3_600 : " + Stop(i2) + "\n");
  Start();
  TransImg(i1, t, DEFAULT, i2);
  Print("Projektiv, DEFAULT, s3_600 : " + Stop(i2) + "\n");
  Start();
  TransAffineImg(i1, t, INTERPOL, i2);
  Print("Affine, INTERPOL, s3_600 : " + Stop(i2) + "\n");
  Start();
  TransImg(i1, t, INTERPOL, i2);
  Print("Projektiv, INTERPOL, s3_600 : " + Stop(i2) + "\n");

  GetChar();
  return OK;
}







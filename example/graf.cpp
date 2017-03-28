#include <stdio.h>
#include <math.h>
#include <image.h>

int main(int argc, char* argv[])
{
  Image imgv, imgo;
  double par[7];

  imgv = NewImg(256, 256, 256);
  ClearImg(imgv);
  imgo = NewImg(256, 256, 256);
  ClearImg(imgo);
  Show(OVERLAY, imgv, imgo);
  Display(ON);
  par[0] = 127;
  par[1] = 127;
  par[2] = 40;
  drawCircle(par, 1, 0, NOFILL, imgo);
  par[2] = 50;
  par[3] = M_PI / 4;
  par[4] = M_PI * 3 / 4;
  drawCircleSegment(par, 1, 0, NOFILL, imgo);
  par[3] = 25;
  par[4] = M_PI / 4;
  par[5] = M_PI / 2;
  par[6] = M_PI * 3 / 2;
  drawEllipseSegment(par, 2, 0, NOFILL, imgo);
  par[4] = 3 * M_PI / 4;
  drawEllipse(par, 3, 0, NOFILL, imgo);
  GetChar();
  par[0] = 12;
  par[1] = 112;
  par[2] = 40;
  drawCircle(par, 1, 0, NOFILL, imgo);
  GetChar();
  drawCircle(par, 1, 2, DEFAULT, imgo);
  GetChar();
  par[0] = 242;
  par[1] = 112;
  par[2] = 40;
  par[3] = 65;
  par[4] = M_PI / 4;
  par[5] = M_PI / 2;
  par[6] = M_PI * 3 / 2;
  drawEllipseSegment(par, 2, 0, NOFILL, imgo);
  par[4] = 3 * M_PI / 4;
  drawEllipse(par, 3, 0, NOFILL, imgo);
  GetChar();
  par[0] = 242;
  par[1] = 112;
  par[2] = 40;
  par[3] = 65;
  par[4] = M_PI / 4;
  par[5] = M_PI / 2;
  par[6] = M_PI * 3 / 2;
  drawEllipseSegment(par, 2, 1, DEFAULT, imgo);
  par[4] = 3 * M_PI / 4;
  drawEllipse(par, 3, 1, DEFAULT, imgo);
  GetChar();
  Display(OFF);

  return 0;
}

/**************************************************************/
/* Testprogramm zur Testen der Funktionen                     */
/*  Line, Circle, Ellipse                                 */
/* "paint_t.c" - neubauer, 01/93                              */
/**************************************************************/

#include <stdio.h>
#include <math.h>
#include <image.h>
//#include <screen.h>

#define XMEN 15
#define YMEN 10

#define XSIZE 999
#define YSIZE 777

void Mark(int x[2], int val, Image img)
{
  Marker(1, x[0], x[1], val, 5, img);
}

int main(int argc, char* argv[])
{
  Image img;
  int x1[2], x2[2], xm[2], r, a, b;
  int val = 0, val1 = 0, val2 = 0, mode = 0;
  double alpha = 45;
  double dx, dy, dx1, dy1, dx2, dy2;
  int i;
  int mv;
  int abort = FALSE;

  Contur c;

  img = NewImg(XSIZE, YSIZE, 255);
  Display(ON);
  Show(ON, img);

  ClearAlpha();

  SetAlphaCursor(3, 3);
  Printf("Bild : %d x %d - Werte von 0 bis %d\n",
         img->xsize, img->ysize, img->maxval);

  mv = img->maxval;

  while (!abort)
    {
      SetAlphaCursor(10, 3);
      Printf("Test Paintfunktionen\n");
      SetAlphaCursor(XMEN, YMEN);
      Printf("1 -> Line\n");
      SetAlphaCursor(XMEN, YMEN + 1);
      Printf("2 -> Circle\n");
      SetAlphaCursor(XMEN, YMEN + 2);
      Printf("3 -> Ellipse\n");
      SetAlphaCursor(XMEN, YMEN + 3);
      Printf("4 -> Polygon\n");
      SetAlphaCursor(XMEN, YMEN + 10);
      Printf("0 -> exit\n");

      i = GetChar();

      SetAlphaCursor(XMEN, YMEN + 12);

      switch (i)
        {
        case '1':
          val = Input("Zeichenwert ");
          mode = Input("Modus ");
          SelPoint(0, img, x1);
          //    cout << x1[0] << " " << x1[1] << endl;
          Mark(x1, mv, img);
          SelPoint(0, img, x2);
          Mark(x2, mv, img);
          Line(x1[0], x1[1], x2[0], x2[1], val, mode, img);
          break;

        case '2':
          val1 = Input("Zeichenwert ");
          val2 = Input("Füllwert ");
          SelPoint(0, img, xm);
          Mark(xm, mv, img);
          SelPoint(0, img, x1);
          Mark(x1, mv, img);
          {
            double dx = xm[0] - x1[0];
            double dy = xm[1] - x1[1];
            r = RoundInt(sqrt(dx * dx + dy * dy));
          }
          Line(xm[0] - r, xm[1] - r, xm[0] - r, xm[1] + r, img->maxval / 2, 0, img);
          Line(xm[0] + r, xm[1] - r, xm[0] + r, xm[1] + r, img->maxval / 2, 0, img);
          Line(xm[0] - r, xm[1] - r, xm[0] + r, xm[1] - r, img->maxval / 2, 0, img);
          Line(xm[0] - r, xm[1] + r, xm[0] + r, xm[1] + r, img->maxval / 2, 0, img);
          Line(xm[0], xm[1] - r, xm[0], xm[1] + r, img->maxval / 2, 0, img);
          Line(xm[0] - r, xm[1], xm[0] + r, xm[1], img->maxval / 2, 0, img);
          Printf("xm[0]: %d xm[1]: %d r: %d\n", xm[0], xm[1], r);
          double par[3];
          par[0] = xm[0];
          par[1] = xm[1];
          par[2] = r;
          drawCircle(par, val1, val2, DEFAULT, img);

          break;

        case '3':
          val1 = Input("Zeichenwert ");
          val2 = Input("Füllwert ");

          SelPoint(0, img, xm);
          Mark(xm, mv, img);
          SelPoint(0, img, x1);
          Mark(x1, mv, img);
          Line(x1[0], x1[1], xm[0], xm[1], mv, 1, img);

          dx = x1[0] - xm[0];
          dy = x1[1] - xm[1];
          alpha = Degree(atan2(dy, dx));
          a = RoundInt(sqrt(dx * dx + dy * dy));

          SelPoint(0, img, x2);
          Mark(x2, mv, img);
          dx1 = x2[0] - xm[0];
          dy1 = x2[1] - xm[1];

          dx2 = (dx1 * dx + dy1 * dy) / a / a * dx;
          dy2 = (dx1 * dx + dy1 * dy) / a / a * dy;

          dx1 = dx1 - dx2;
          dy1 = dy1 - dy2;

          b = RoundInt(sqrt(dx1 * dx1 + dy1 * dy1));

          Printf("Ellipse (%d,%d) %d, %d  %5.2f\n",
                 xm[0], xm[1], a, b, alpha);
          double ellipse[5];
          ellipse[0] = xm[0];
          ellipse[1] = xm[1];
          ellipse[2] = a;
          ellipse[3] = b;
          ellipse[4] = Arcus(alpha);
          drawEllipse(ellipse, val1, val2, 0, img);
          break;

        case '4':
          break;

        case '0':
          abort = true;
          break;
        }
    }

  Display(OFF);

  return 0;
}









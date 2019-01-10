#include <image.h>
// extended histogram equalization
#define superq 1000

void XHequal(const Image& qimg, Image& iimg)
{
  int xs = qimg->xsize;
  int ys = qimg->ysize;
  int maxvx = (iimg->maxval + 1) * superq - 1 ;

  //  cout << "maxvalx:" << maxvx << endl;
  Histogram h(qimg); // Histogramm ermitteln

  vector<int> dval(qimg->maxval);
  vector<int> llimit(qimg->maxval);
  vector<int> ulimit(qimg->maxval);

  double ddval = 0.0;

  for (int i = 0; i <= qimg->maxval; i++)
    {
      dval[i] = (int)ddval;
      ddval = ddval + h.Rel(i) * maxvx;
      //      cout << dval[i] << endl;
    }

  llimit[0] = 0;

  for (int i = 1; i <= qimg->maxval; i++)
    {
      llimit[i] = (dval[i - 1] + dval[i]) / 2;
      //llimit[i]=dval[i-1];
      //      cout << llimit[i] << endl;
    }

  for (int i = 0; i < qimg->maxval; i++)
    {
      ulimit[i] = (dval[i + 1] + dval[i]) / 2;
      //ulimit[i]=dval[i+1];
      //      cout << ulimit[i] << endl;
    }

  ulimit[qimg->maxval] = maxvx;

  for (int i = 0; i <= qimg->maxval; i++)
    cout << i << ": " << llimit[i] << " .. " << ulimit[i] << " " << dval[i] << endl;


  Image himg = NewImg(xs, ys, maxvx);

  for (int y = 0; y < ys; y++)
    for (int x = 0; x < xs; x++)
      PutVal(himg, x, y, dval[GetVal(qimg, x, y)]);

  Show(ON, himg, "xhq-debug"); // Debug

  // GetChar();

  int change;
  int istep = 0;

  do
    {
      change = 0;
#ifdef OPENMP
      #pragma omp parallel for schedule(dynamic,40)
#endif

      for (int y = 1; y < ys - 1; y++)
        {
          int y1 = y - 1;
          int y2 = y + 1;

          for (int x = 1; x < xs - 1; x++)
            {
              int x1 = x - 1;
              int x2 = x + 1;
              int sval = GetValUnchecked(qimg, x, y);
              int aval = GetValUnchecked(himg, x, y);
              int val = (aval +
                         GetValUnchecked(himg, x1, y) +
                         GetValUnchecked(himg, x2, y) +
                         GetValUnchecked(himg, x, y1) +
                         GetValUnchecked(himg, x, y2) + 2) / 5;

              //        cout << sval << ": " << aval << " " << val << endl;
              if (val > ulimit[sval])
                val = ulimit[sval];

              if (val < llimit[sval])
                val = llimit[sval];

              //        cout << llimit[sval] << " .. " << ulimit[sval] << " " << aval << " " << val << endl;
              if (val != aval)
                {
                  PutVal(himg, x, y, val);
                  //      PutValUnchecked(himg,x,y,val);
                  change++;
                }
            }
        }

      istep++;
      cout << istep << ": " << change << endl;
    }
  while (change > 0);

  for (int y = 0; y < ys; y++)
    for (int x = 0; x < xs; x++)
      PutVal(iimg, x, y, GetVal(himg, x, y) / superq);
}

int main(int argc, char** argv)
{
  Image src;
  Alpha(ON);
  Printf("XHequal");
  src = ReadImg(argv[1]);
  Show(ON, src, "Original");

  Image hequal = NewImg(src);
  Show(ON, hequal, "HEqual");
  HistogramEqual(src, hequal);

  Image xhequal = NewImg(src);
  XHequal(src, xhequal);
  Show(ON, xhequal, "XHequal");

  int rc = 1;

  while (rc >= 0)
    {
      Point p = SelPoint(1, xhequal, rc);
      int g1 = GetVal(src, p.x, p.y);
      int g2 = GetVal(hequal, p.x, p.y);
      int g3 = GetVal(xhequal, p.x, p.y);
      Printf("(%d,%d) %d %d %d\n", (int)p.x, (int)p.y, g1, g2, g3);
    }

  Print("Irgend eine Taste drücken");
  GetChar();
  return 0;
}

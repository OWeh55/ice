#include <image.h>

const int ss = 2;

int main(int argc, char** argv)
{
  Image img;
  img = ReadImg("test_gray.jpg");
  //  img = ReadImg("test.jpg");
  Show(ON, img);
  Image dst = NewImg(img);
  Show(ON, dst);
  IPoint dd(ss, ss);

  IPoint p;

  for (p.y = 0 ; p.y <= img->ysize ; p.y++)
    for (p.x = 0 ; p.x <= img->xsize ; p.x++)
      {
        if (Inside(img, p - dd) && Inside(img, p + dd))
          {
            Moments m;
            int min = 255;
            int sum = 0;

            for (int dy = -ss; dy <= ss; dy++)
              for (int dx = -ss; dx <= ss; dx++)
                {
                  IPoint h = p + IPoint(dx, dy);
                  int g = GetVal(img, h);

                  if (g < min) min = g;

                  sum = sum + g;
                }

            double mean = (double)sum / (ss * ss);

            for (int dy = -ss; dy <= ss; dy++)
              for (int dx = -ss; dx <= ss; dx++)
                {
                  IPoint h = p + IPoint(dx, dy);
                  int g = GetVal(img, h);
                  //    m.Add(Point(dx,dy),g-min);
                  m.Add(Point(dx, dy), g - mean);
                }

#if 0

            for (int i = 0; i < 5; i++)
              for (int j = 0; j < 5; j++)
                if (i + j < 5)
                  cout << i << "," << j << ": " << m(i, j) << endl;

#endif
#if 0

            if (m(0, 0) > 0)
              {
                Ellipse eps = FitEllipse(m);
                double r1 = eps.R();
                double r2 = eps.R2();
                cout << r1 << " / " << r2 << endl;
                int g = int(255 * r2 / r1);
                PutValue(dst, p, g);
              }

#endif
            double z = 2.0 * m(1, 1);
            double n = m(2, 0) - m(0, 2);
            //    cout << z << " " << n << endl;
            //    PutValue(dst,p,limited(z/10+127,img));
            //    PutValue(dst,p,limited(n/10+127,img));
            //    PutValue(dst,p,limited((n*n+z*z)/1000000,img));
            double d2 = m(2, 0) - m(0, 2);
            double s2 = m(2, 0) + m(0, 2);
            double m2norm = d2 * d2 + 4.0 * m(1, 1) * m(1, 1);
            m2norm /= m(0, 0) * m(0, 0);
            //    m2norm = m2norm / (s2 * s2);
            cout << m2norm << endl;
            PutVal(dst, p, limited(m2norm * 100000, img));
          }
      }

  GetChar();
  return OK;
}

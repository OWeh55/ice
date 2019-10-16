#include <math.h>

#include "numbase.h"
#include "defs.h"
#include "IceException.h"

#include "bairstow.h"
namespace ice
{
#define FNAME "Bairstow"
  int Bairstow(const Vector& para, Matrix& result, int inumber)
  {
    int rc = OK;

    // determine real degree of polygon
    int degree = para.size() - 1;
    while (degree >= 0 && para[degree] == 0)
      {
        degree--;
      }

    if (degree < 1)
      {
        rc = NO_SOLUTION;
        throw IceException(FNAME, M_NO_SOLUTION);
      }

    // redegree coefficients
    Vector a;
    for (int i = degree; i >= 0; i--)
      {
        a.Append(para[i]);
      }

    int iterate = degree;
    int n = degree;

    result = Matrix(degree, 2);

    while (n > 2)
      {
        double s = 2.0;
        double r = 2.0;
        double ds = 1.0;
        double dr = 1.0;
        Vector b(n + 1);
        Vector c(n + 1);
        //Search for r and s
        int i1 = 0;

        do
          {
            if (i1 >= inumber)
              {
                rc = NO_SOLUTION;
                throw IceException(FNAME, M_NO_SOLUTION);
              }

            b[0] = a[0];
            b[1] = a[1] + r * b[0];

            for (unsigned int j = 2; j <= n; j++)
              {
                b[j] = a[j] + r * b[j - 1] + s * b[j - 2];
              }

            c[0] = 0.0;
            c[1] = b[0];
            c[2] = b[1] + r * c[1];

            for (unsigned int j = 3; j <= n; j++)
              {
                c[j] = b[j - 1] + r * c[j - 1] + s * c[j - 2];
              }

            double denom = c[n - 1] * c[n - 1] - c[n] * c[n - 2];

            dr = (b[n] * c[n - 2] - c[n - 1] * b[n - 1]) / denom;
            ds = (b[n - 1] * c[n] - c[n - 1] * b[n]) / denom;
            r = r + dr;
            s = s + ds;
            i1++;

          }
        while ((ds * ds + dr * dr) > EPSILON);

        {
          double p2 = -r / 2.0;
          double q = -s;

          double D = p2 * p2 - q;

          if (D >= 0)
            {
              D = sqrt(D);
              result[iterate - 1][0] = -p2 + D;
              result[iterate - 1][1] = 0;
              result[iterate - 2][0] = -p2 + D;
              result[iterate - 2][1] = 0;
            }
          else
            {
              D = sqrt(-D);
              result[iterate - 1][0] = -r;
              result[iterate - 1][1] = D ;
              result[iterate - 2][0] = -r;
              result[iterate - 2][1] = -D;
            }
        }

        for (unsigned int j = 1; j <= n - 1; j++)   // for next stage's iteration
          {
            a[j - 1] = b[j - 1];
          }

        n -= 2;
        iterate -= 2;
      }

    if (n == 2)
      {
        double p2 = -a[1] / a[0] / 2.0;
        double q = -a[2] / a[0];

        double D = p2 * p2 - q;

        if (D >= 0)
          {
            D = sqrt(D);
            result[iterate - 1][0] = -p2 + D;
            result[iterate - 1][1] = 0;
            result[iterate - 2][0] = -p2 + D;
            result[iterate - 2][1] = 0;
          }
        else
          {
            D = sqrt(-D);
            result[iterate - 1][0] = -p2;
            result[iterate - 1][1] = D ;
            result[iterate - 2][0] = -p2;
            result[iterate - 2][1] = -D;
          }
      }
    else
      {
        if (n == 1)
          {
            result[0][0] = -a[1] / a[0];
            result[0][1] = 0;
          }
      }

    return rc;
  }
#undef FNAME
}

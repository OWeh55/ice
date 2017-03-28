/*
 * simple test for lmdif
 * fit of a circle to 4 points
 */

#include <image_nonvis.h>
#include <float.h>
#include <lmdif.h>

// function for c like way (lmdif)
int fcn(int m, int n, double x[], double fvac[])
{
  int i;
  double xx[] = {1.0, 2.0, 1.0, 0.0};
  double yy[] = {0.0, 1.0, 2.0, 1.0};
  double dx, dy;

//  printf("%d %d\n",m,n);
  for (i = 0; i < 4; i++)
    {
      dx = xx[i] - x[0];
      dy = yy[i] - x[1];
      fvac[i] = sqrt(fabs(dx) + fabs(dy)) - x[2];
    }

  return 1;
}
// function for c++ wrapper

int FCircle(const Vector& x, Vector& result)
{
  int i;
  double xx[] = {1.0, 2.0, 1.0, 0.0};
  double yy[] = {0.0, 1.0, 2.0, 1.0};
  double dx, dy;

  //  printf("%f %f  %f\n",x[0],x[1],x[2]);
  for (i = 0; i < 4; i++)
    {
      dx = xx[i] - x[0];
      dy = yy[i] - x[1];
      result[i] = sqrt(fabs(dx) + fabs(dy)) - x[2];
    }

  return 1;
}

#define numvar 3
#define numfunc 4

void infomessage(int info)
{
  return ;
  printf("Info:%d\n", info);

  switch (info)
    {
    case 0:
      printf("improper input parameters");
      break;
    case 1:
      printf("both actual and predicted relative reductions in "
             "the sum of squares are at most ftol.");
      break;
    case 2:
      printf("relative error between two consecutive iterates is at most xtol.");
      break;
    case 3:
      printf("conditions for info = 1 and info = 2 both hold.");
      break;
    case 4:
      printf("the cosine of the angle between fvec and any column of the"
             " jacobian is at most gtol in absolute value.");
      break;
    case 5:
      printf("number of calls to fcn has reached or exceeded maxfev.");
      break;
      break;
    case 6:
      printf("ftol is too small. no further reduction in the sum of squares"
             " is possible.");
      break;
    case 7:
      printf("xtol is too small. no further improvement in the approximate "
             "solution x is possible.");
      break;
    case 8:
      printf("gtol is too small. fvec is orthogonal to the columns of the "
             "jacobian to machine precision.");
      break;
    }

  printf("\n");
}

int testlmdif()
{
  double x[numvar] = {0.0, 0.0, 0.0};
  double v[numfunc];
  double diag[numvar];
  int info;
  int nfev;
  double fjac[numvar * numfunc];
  int ipvt[numvar];
  double qtf[numvar];
  lmdif(fcn, numfunc, numvar, x, v,
        DBL_EPSILON/*ftol*/, DBL_EPSILON/*xtol*/, DBL_EPSILON/*gtol*/, 10000000, DBL_EPSILON/*epsfcn*/,
        diag, 1,
        100./*factor*/, 0/*nprint*/, &info, &nfev, fjac, numfunc/*fjac*/, ipvt, qtf);

  infomessage(info);
  printf("Nach %d Iterationen\n", nfev);
  printf("(%f,%f) r=%f\n", x[0], x[1], x[2]);
  return OK;
}

int testLMDif()
{
  int info;
  int inumber = 1000;
  Vector x(0.0, 0.0, 0.0);
  info = LMDif(x, numvar, FCircle, numfunc, inumber);
  infomessage(info);
  printf("Nach %d Iterationen\n", inumber);
  printf("(%f,%f) r=%f\n", x[0], x[1], x[2]);
  return OK;
}

int testLMDif2(IVector ov)
{
  int info;
  int inumber = 1000;
  Vector x(0.0, 0.0, 0.0);
  info = LMDif(x, ov, FCircle, numfunc, inumber);
  infomessage(info);
  printf("Nach %d Iterationen\n", inumber);
  printf("(%f,%f) r=%f\n", x[0], x[1], x[2]);
  return OK;
}

int main(int argc, char* argv[])
{
  testlmdif();
  testLMDif();
  testLMDif2(IVector(0, 1));
  testLMDif2(IVector(0, 2));
  testLMDif2(IVector(1, 2));
  testLMDif2(IVector(0, 2, 1));
  testLMDif2(IVector(0, 1, 2));
  return OK;
}

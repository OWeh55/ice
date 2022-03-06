/**************************************************************/
/* Test 1D - Fouriertransformation                             */
/**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <image_nonvis.h>
#include <string.h>
#include <fstream>

#define SIZE 8

void print(const vector<double>& v,
           bool index = true, bool centered = true,
           bool sc = false)
{
  int width = 9;
  int dim = v.size();
  int n = dim;
  if (n > 20)
    n = 20;
  int fm = n / 2;
  if (index)
    {
      if (centered)
        {
          for (int i = -fm; i < -fm + n; ++i)
            cout << setw(width) << i << " ";
        }
      else
        {
          for (int i = 0; i < n; ++i)
            {
              int idx = i;
              if (idx >= fm)
                idx = i - n;
              cout << setw(width) << idx << " ";
            }
        }
      cout << endl;
    }
  double sum2 = 0;
  for (int idx = 0; idx < n; ++idx)
    {
      sum2 += v[idx] * v[idx];
      if (sc)
        cout << setw(width) << setprecision(2) << scientific << v[idx] << " ";
      else
        cout << setw(width) << setprecision(4) << fixed << v[idx] << " ";
    }
  cout << "  (" << sqrt(sum2) << ")";
  cout << endl;
}

int main(int argc, char* argv[])
{
  Vector v1, v2, v3;
  Matrix m1;
  int i;
  //  Alpha(ON);
  //  ClearAlpha();
  v1 = Vector(SIZE);
  v2 = Vector(SIZE);

  for (i = 0; i < SIZE; i++)
    {
      v1[i] = sin(i * M_PI * 2 / SIZE);
      v2[i] = 0;
    }

  cout.setf(ios::fixed);
  cout.precision(4);
  cout << "original" << endl;
  print(v1);

  Fourier(v1, v2);

  cout << "spectrum" << endl;
  print(v1);
  print(v2, false);

  Fourier(v1, v2, INVERS);
  cout << "inverse transformed: " << endl;
  print(v1);
  print(v2, false);

  cout << "--- Convolution ---" << endl;

  for (i = 0; i < SIZE; i++)
    {
      v2[i] = v1[i] = 0;
    }

  v2[1] = 1.0;
  v1[0] = 1;
  v1[1] = 1;

  calcConvolution(v1, v2, v3);
  print(v1, true, false);
  print(v2, false);
  print(v3, false);
  cout << "----------------------------" << endl;

  for (i = 0; i < SIZE; i++)
    {
      v1[i] = i;
      v2[(i + 7) % SIZE] = i;
    }

  calcInvConvolution(v1, v2, v3, 0.001);
  print(v1, true, false);
  print(v2, false);
  print(v3, false);
  cout << "----------------------------" << endl;

  return 0;
}


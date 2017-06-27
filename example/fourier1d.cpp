/**************************************************************/
/* Test 1D- Fouriertransformation                             */
/**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <image_nonvis.h>
#include <string.h>
#include <fstream>

#define SIZE 15

int main(int argc, char* argv[])
{
  std::vector<double> v1(SIZE), v2(SIZE), v3;
  Matrix m1;

  for (int i = 0; i < SIZE; i++)
    {
      v1[i] = sin(i * M_PI * 2 / SIZE);
      v2[i] = 0;
    }

  cout.setf(ios::fixed);
  cout.precision(4);
  cout << "Original (real):" << endl;
  cout << v1 << endl << "----------------------------" << endl;
  Fourier(v1, v2);
  cout << "Transformed (complex)" << endl;
  cout << v1 << endl;
  cout << v2 << endl << "----------------------------" << endl;
  Fourier(v1, v2, INVERS);
  cout << "inverse transformed (complex)"  << endl;
  cout << v1 << endl;
  cout << v2 << endl << "----------------------------" << endl;

  for (int i = 0; i < SIZE; i++)
    {
      v2[i] = v1[i] = 0;
    }

  v2[3] = 1.0;
  v1[2] = 1.0;

  Convolution(v1, v2, v3);

  cout << "v1:   " << v1 << endl;
  cout << "v2:   " << v2 << endl;
  cout << "v1*v2:" << v3 << endl << "----------------------------" << endl;

  for (int i = 0; i < SIZE; i++)
    {
      v1[i] = i;
      v2[(i + 7) % SIZE] = i;
    }

  InvConvolution(v1, v2, 0.001, v3);
  cout << "v1:   " << v1 << endl;
  cout << "S(v1):" << v2 << endl;
  cout << "S:    " << v3 << endl << "----------------------------" << endl;

  return 0;
}

/**************************************************************/
/* Test Zeit 1D- Fouriertransformation                        */
/**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <image.h>

#define TIMING 1
#include <visual/timing.h>

#define SIZE 1024*1024*10
#define TIMES 4

int main(int argc, char* argv[])
{
  Vector v1, v2, v3;
  Matrix m1;
  int i;

  Alpha(ON);
  v1 = Vector(SIZE);
  v2 = Vector(SIZE);

  // Feld belegen
  for (i = 0; i < SIZE; i++)
    {
      v1[i] = sin(i * M_PI * 2 / SIZE);
      v2[i] = 0;
    }

  for (int i = 0; i < TIMES; i++)
    {
      BEGIN(1, "Fourier");
      Fourier(v1, v2);
      END(1, "Fourier");
    }

  GetChar();
  return 0;
}

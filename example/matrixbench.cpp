#include <image.h>

#define TIMING 1
#include <visual/timing.h>

#define SIZE 300
#define TIMES 10000

typedef struct
{
  unsigned int x: 9;
  unsigned int y: 9;
  unsigned int p: 2;
} padr;

int main(int argc, char* argv[])
{
  // Laufzeiten für array-zugriffe
  int i;
  int x, y;
  volatile double d;
  //  double d;
  volatile int n;
  //  int n;

  Matrix mat(SIZE, SIZE);

  matrix<double> tmat(SIZE, SIZE);

  double m[SIZE][SIZE];

  padr pa[SIZE][SIZE];

  double* mp[SIZE];

  for (i = 0; i < SIZE; i++)
    mp[i] = (double*)malloc(sizeof(double) * SIZE);

  BEGIN(0, "c-Matrix");

  for (i = 0; i < TIMES; i++)
    for (y = 0; y < SIZE; y++)
      for (x = 0; x < SIZE; x++)
        {
          d = m[y][x];
        }

  END(0, "");
  BEGIN(0, "c-Matrix *mp[]");

  for (i = 0; i < TIMES; i++)
    for (y = 0; y < SIZE; y++)
      for (x = 0; x < SIZE; x++)
        {
          d = mp[y][x];
        }

  END(0, "");
  BEGIN(0, "ICE-Matrix []");

  for (i = 0; i < TIMES; i++)
    for (y = 0; y < SIZE; y++)
      for (x = 0; x < SIZE; x++)
        {
          d = mat[y][x];
        }

  END(0, "");
  BEGIN(0, "ICE-Matrix at()");

  for (i = 0; i < TIMES; i++)
    for (y = 0; y < SIZE; y++)
      for (x = 0; x < SIZE; x++)
        {
          d = mat.at(y).at(x);
        }

  END(0, "");

  BEGIN(0, "matrix<double> []");

  for (i = 0; i < TIMES; i++)
    for (y = 0; y < SIZE; y++)
      for (x = 0; x < SIZE; x++)
        {
          d = tmat[y][x];
        }

  END(0, "");

  Printf("Size of padr: %d\n", sizeof(padr));
  Printf("Size of pa: %d\n", sizeof(pa));

  BEGIN(0, "bitarray x");

  for (i = 0; i < TIMES; i++)
    for (y = 0; y < SIZE; y++)
      for (x = 0; x < SIZE; x++)
        {
          n = pa[y][x].x;
        }

  END(0, "");
  BEGIN(0, "bitarray y");

  for (i = 0; i < TIMES; i++)
    for (y = 0; y < SIZE; y++)
      for (x = 0; x < SIZE; x++)
        {
          n = pa[y][x].y;
        }

  END(0, "");
  BEGIN(0, "bitarray p");

  for (i = 0; i < TIMES; i++)
    for (y = 0; y < SIZE; y++)
      for (x = 0; x < SIZE; x++)
        {
          n = pa[y][x].p;
        }

  END(0, "");
  GetChar();
  return OK;
}

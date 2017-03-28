#include <stdio.h>
#include <stdlib.h>
#include <image_nonvis.h>

/* Test für Sortieren von Matrix und matrix */
#define LNG 10

int main(int argc, char* argv[])
{
  MatrixStruct mat, mats;
  int i, j;
  double time;
  mat = NewMatrix(MAT_DOUBLE, LNG, 2);

  for (i = 0; i < LNG; i++)
    for (j = 0; j < 2; j++)
      mat->data[i][j] = LNG * RandomD();

  time = TimeD();
  mats = SortMatrix(mat, 0, -1);
  printf("Zeit für %d Elemente: %f\n", LNG, TimeD() - time);

  for (i = 0; i < LNG; i++)
    {
      printf("%f  %f    %f  %f\n", mats->data[i][0], mats->data[i][1], mat->data[i][0], mat->data[i][1]);
    }

  Matrix m(LNG, 2);

  for (i = 0; i < LNG; i++)
    for (j = 0; j < 2; j++)
      m[i][j] = LNG * RandomD();

  time = TimeD();
  m.Sort(0);
  printf("Zeit für %d Elemente: %f\n", LNG, TimeD() - time);

  for (i = 0; i < LNG; i++)
    {
      printf("%f  %f\n", m[i][0], m[i][1]);
    }

  return 0;
}

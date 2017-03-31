/*
 * Testprogramm fuer Berechnung der inversen Matrix
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <image_nonvis.h>

void printmat(int, int, double*);

/********************************************************************
Multiplikation von zwei Matrizen
----------------------------------------------------------------
C:=A*B
z1   - Zeilen Matrix A
sz   - Spalten Matrix A, Zeilen Matrix B
s2   - Spalten Matrix B
pa   - double pointer auf Matrix A
pb   - double pointer auf Matrix B
pc   - double pointer auf Matrix C
----------------------------------------------------------------
***/
void mat_mult(int z1, int sz, int s2, double* pa, double* pb, double* pc)
{
  int i, j, k, f;
  f = 0;

  for (i = 0; i < z1; i++)
    {
      for (j = 0; j < s2; j++, f++)
        {
          pc[f] = 0;

          for (k = 0; k < sz; k++) pc[f] += pa[i * z1 + k] * pb[j + s2 * k];
        }
    }
}

/********************************************************************
Ausgabe einer Matrix
----------------------------------------------------------------
z   - Zeilen Matrix A
s   - Spalten Matrix A
pa   - double pointer auf Matrix A
----------------------------------------------------------------
***/
void printmat(int z, int s, double* pa)
{
  int i, j;
  printf("\n");

  for (i = 0; i < z; i++)
    {
      printf("\n|");

      for (j = 0; j < s; j++)
        printf("%9.2e ", *(pa++));
    };
};

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int main(int argc, char* argv[])
{
  double* a, *b, *c;
  int i, j, RANG;
  double maxf, f;
  printf("\n\n### Berechnung der invesen Matrix ###\n \
	      -Bitte Rang der zu invertierenden Matrix eingeben->");
  scanf("%d", &RANG);

  if (RANG < 1 && RANG > 1000) return 1;

  a = (double*)malloc(RANG * RANG * sizeof(double));
  b = (double*)malloc(RANG * RANG * sizeof(double));
  c = (double*)malloc(RANG * RANG * sizeof(double));

  printf("Fuelle Matrix mit Zufallszahlen \n");

  for (i = 0; i < RANG; i++)
    for (j = 0; j < RANG; j++) a[i * RANG + j] = ((double)rand()) / 20 - 700;

  printf("Invertiere Matrix vom Rang %d\n", RANG);

  InvertMatrix(a, RANG, b);

  printf("Multipliziere Inverse Matrix mit Originalmatrix\n");
  mat_mult(RANG, RANG, RANG, a, b, c);
  maxf = 0;

  for (i = 0; i < RANG; i++)
    for (j = 0; j < RANG; j++)
      {
        if (i == j)  
	  f = fabs(1 - c[RANG * i + j]);
        else  
	  f = fabs(c[RANG * i + j]);

        if (f > maxf) maxf = f;
      }

  printf("Fehler bei der Rueckmultiplikation :%9.3e\n", maxf);

  if (RANG < 14)
    {
      printmat(RANG, RANG, a);
      printf("\nOriginalmatrix");
      printmat(RANG, RANG, b);
      printf("\nInvertierte Matrix");
      printmat(RANG, RANG, c);
      printf("\nProdukt beider obiger Matrizen");
    }

  return OK;

};

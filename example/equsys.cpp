/* Algebra mit Matrizen als double array - Veraltet! */

#define RANGMAX 40
#define RANGANZ 10
#define EQUMAX (RANGMAX+RANGANZ)

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <image_nonvis.h>

using namespace ice;

#if !defined(RAND_MAX)
#define RAND_MAX (1<<15)
#endif

double equprobe(int r, double* a, double* x, double* b)
{
  int i, j;
  int maxabw, abw;
  double bi;
  double* aptr;
  aptr = a;
  maxabw = 0;

  for (i = 0; i < r; i++)
    {
      bi = 0;

      for (j = 0; j < r; j++, aptr++) bi += *aptr * x[j];

      abw = (int)fabs(bi - b[i]);

      if (maxabw < abw) maxabw = abw;
    };

  return (maxabw);
};

int main(int argc, char* argv[])
{
  double a[RANGMAX * EQUMAX];
  double b[EQUMAX];
  double x[RANGMAX];
  double na[9] = {1, 2, 7, 3, 6, 3, 3, 6, 10};
  double nb[3] = {32, 13, 28};

  int rang, tnr, lsg, i, j;

  double s;
  double randmaxhalbe = (double)RAND_MAX / 2;

  printf("\n\nTestprogramm Modul equsys.c\n");
  printf("Teste lineare Gleichungssysteme vom Rang 1 bis %d (je %d) \n", RANGMAX, RANGANZ);

  for (rang = 1; rang <= RANGMAX; rang++)
    {
      printf("\nRang %d", rang);

      for (tnr = 0; tnr < RANGANZ; tnr++)
        {
          for (i = 0; i < rang; i++)
            {
              b[i] = 0; /* ((double)rand()-randmaxhalbe); */

              for (j = 0; j < rang; j++) a[i * rang + j] = (double)rand() - randmaxhalbe;
            };

          OffMessage();

          lsg = EquSys(a, b, rang, x);

          SetOk();

          OnMessage();

          if (lsg == OK)
            {
              if (equprobe(rang, a, x, b) == 0) printf("*");
              else printf("!");
            }
          else printf("\nRang %d Test %d Fehlercode %d \n", rang, tnr, lsg);
        };
    };

  printf("\n");

  printf("\n\nBeispiel für numerische Instabilität\n");

  printf("1 2  7 | 32\n3 6  3 | 13\n3 6 10 | 28\n");

  OffMessage();

  lsg = EquSys(na, nb, 3, x);

  SetOk();

  OnMessage();

  if ((lsg == NUM_INSTABILITY) || (lsg == OK))
    {
      printf("Rückkehrcode %d\n", lsg);
      printf("Lösungsvektor:\n %9.2e\n %9.2e\n %9.2e\n", x[0], x[1], x[2]);
      printf("max. Abweichung %9.2e\n", equprobe(3, na, x, nb));
    }
  else printf("\n Fehlercode %d \n", lsg);

  printf("\n\nTeste überbestimmte lineare Gleichungssysteme vom Rang 1 bis %d (je %d) \n", RANGMAX, RANGANZ);

  for (rang = 1; rang <= RANGMAX; rang++)
    {
      for (tnr = 0; tnr < RANGANZ; tnr++)
        {
          for (i = 0; i < rang + tnr; i++)
            {
              b[i] = ((double)rand() - randmaxhalbe);

              for (j = 0; j < rang; j++) a[i * rang + j] = (double)rand() - randmaxhalbe;
            };

          OffMessage();

          lsg = OverEquSys(a, b, rang, rang + tnr, x, &s);

          SetOk();

          OnMessage();

          printf("Rang %d: Anzahl Gl. %d: ", rang, rang + tnr);

          if (lsg == OK) printf("Fehlersumme %9e\n", s);
          else printf("Fehlercode %d \n", lsg);
        };
    };

  return (0);
};

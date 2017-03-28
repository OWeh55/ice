/*****************************************************************/
/*  test11.c               */
/* test der erweiterten I/O-Funktionen         */
/*****************************************************************/

#include <stdio.h>
#include <math.h>
#include <image.h>

int main(int argc, char* arg[])
{
  ClearAlpha();
  SetAlphaCursor(5, 5);
  Printf("Ihre Eingabe: %le \n", InputD("Geben Sie eine double-Zahl ein :"));
  Printf("Ihre Eingabe: %d \n", Input("Geben Sie eine integer-Zahl ein :"));
  return 0;
}

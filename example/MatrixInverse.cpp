/*
 * Testprogramm fuer Berechnung der inversen Matrix (Typ Matrix!)
 */

#include <iomanip>
#include <image_nonvis.h>

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int main(int argc, char* argv[])
{
  srand48(time(NULL));
  int rang;
  cout << "Bitte Rang der zu invertierenden Matrix eingeben: ";
  cin >> rang;
  while (rang<1 || rang>1000)
    {
      cout << "Bitte Rang der zu invertierenden Matrix im Bereich 1.. 100 eingeben: ";
      cin >> rang;
    }

  Matrix a(rang,rang);

  cout << "Fuelle Matrix mit Zufallszahlen " << endl;

  for (int i = 0; i < rang; i++)
    for (int j = 0; j < rang; j++) 
      a[i][j] = drand48();

  cout << "Invertiere Matrix vom Rang" << rang << endl;

  Matrix b=Inverse(a);

  cout << "Multipliziere inverse Matrix mit Originalmatrix" << endl;
  Matrix c = a*b;

  double maxf = 0;

  for (int i = 0; i < rang; i++)
    for (int j = 0; j < rang; j++)
      {
	double f;
        if (i == j)  
	  f = fabs(1 - c[i][j]);
        else  
	  f = fabs(c[i][j]);
	
        if (f > maxf) maxf = f;
      }

  cout  << "maximaler Fehler bei der Rueckmultiplikation: " << maxf << endl;

  if (rang <= 10)
    {
      cout << "Original:" << endl;
      cout << a;
      cout << "Inverse: " << endl;
      cout << b;
      cout << "Produkt: " << endl;
      cout << c;
    }

  return OK;

};

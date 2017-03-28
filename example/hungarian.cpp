#include <image_nonvis.h>

//#define POINTS 500
#define DIM 5

double HungarianTime(int points)
{
  Matrix distanz;
  Matrix pl1;
  Matrix pl2;
  IMatrix ref;
  double dist;
  int i, j;
  double time, ta;

  pl1 = Matrix(points, DIM);
  pl2 = Matrix(points, DIM);

  for (i = 0; i < points; i++)
    {
      for (j = 0; j < DIM; j++)
        {
          pl1[i][j] = GaussRandom(1.0);
          pl2[points - 1 - i][j] = pl1[i][j] + GaussRandom(0.0001);
        }
    }

  distanz = DistanceMatrix(pl1, pl2, D_EUCLID);

  ta = TimeD();
  Hungarian(distanz, ref, dist);
  time = TimeD() - ta;
  cerr << "Zeit für Hungarian (" << points << "): " ;
  cerr << time << " Sekunden" << endl ;

  //  cerr << ref << endl;
  j = 0; // Zähler für Fehler

  for (i = 0; i < points; i++)
    {
      if (ref[i][0] + ref[i][1] != points - 1)
        {
          cerr << "Error: " << ref[i][0] << " - " << ref[i][1] << endl;
          j++;
        }
    }

  cerr << "Zuordnungsfehler: " << j << endl;
  cerr << dist << endl;
  return time;
}

int main(int argc, char** argv)
{
  int maxpoints;
  int points;
  int step;

  if (argc > 1) maxpoints = atol(argv[1]);
  else maxpoints = 500;

  step = maxpoints / 100;

  if (step == 0) step = 1;

  for (points = step; points < maxpoints; points += step)
    {
      cout << points << "," << HungarianTime(points) << endl;
    }

  return OK;
}

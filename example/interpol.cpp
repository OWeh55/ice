#include <image.h>
#include <iomanip>

double CosN(double n, double t)
{
  return cos(n * 2 * M_PI / t);
}

double SinN(double n, double t)
{
  return sin(n * 2 * M_PI / t);
}

void TInterpol(const vector<double>& t, // Stütztstellen
               const vector<double>& f, // Funktionswerte in Stützstellen
               vector<double>& inter,   // interpolierte Funktion
               int maxf = 0)    // explizite Grenzfrequenz
{
  // "Bandbegrenzung"
  if (maxf == 0)
    maxf = (t.size() - 1) / 2 - 1; // maximaler frequenzbetrag = Stützstellen / 2

  // Gleichungssystem
  Matrix A(t.size(), maxf * 2 + 1);

  // f ist Inhomogentität
  Vector fi(f.size());

  for (unsigned int i = 0; i < f.size(); i++)
    fi[i] = f[i];

  // Intervalllänge = Zahl der zu berechnenden Funktionswerte
  double ilen = inter.size();

  // Matrix aufbauen
  // cout << "Matrix aufbauen" << endl;

  for (int i = 0; i < (int)t.size(); i++) // i. Gleichung für i. Stützstelle
    {
      double fi = t[i] * 2 * M_PI / ilen ; // Phase für frequenz 1
      A[i][0] = 1; // Absolutglied cos(0*t)
      int bi = 1; // aktueller index in der Zeile

      for (int k = 1; k <= maxf; k++) // steigende Frequenz
        {
          A[i][bi] = sin(fi * k);
          bi++;
          A[i][bi] = cos(fi * k);
          bi++;
        }
    }

  // cout << "Gleichung lösen" << endl;
  // cout << A << f << endl;

  Vector res = SolveLinearEquation(A, fi);

#if 0

  for (int i = 0; i < res.size(); i++)
    cout << i << " " << res[i] << endl;

  cout << endl << endl;
#endif

  //  cout << res << endl;
  //  cout << "Koeffizienten umlagern" << endl;
  // Fourierkoeffizienten herauslösen

  double a0 = res[0]; // Absolutglied
  vector<double> af(maxf);  // Koeffizienten cos(n*x)
  vector<double> bf(maxf);  // Koeffizienten sin(n*x)
  int si = 1;
  int di = 0;

  for (int i = 1; i <= maxf; i++)
    {
      bf[di] = res[si];
      si++;
      af[di] = res[si];
      si++;
      di++;
    }

  //cout << "a0: "<< a0 << endl << af << bf << endl;
  //  cout << "Interpolationsvektor aufbauen" << endl;
  // Interpolationsvektor aufbauen
  for (int i = 0; i < (int)inter.size(); i++)
    {
      double fi = (double)i * 2 * M_PI / ilen;
      //      cout << fi << " " ;
      double f = a0;

      for (int k = 1; k <= maxf; k++)
        {
          f += cos(fi * k) * af[k - 1] + sin(fi * k) * bf[k - 1];
        }

      inter[i] = f;
    }
}

void extra(Image& img, int vals)
{
  for (int i = 0; i < vals; i++) // "gültige Zeilen"
    {
      // in der Zeile extrapolieren
      vector<double> t;
      vector<double> f;
      vector<double> inter(img->xsize);

      for (int k = 0; k < vals; k++)
        {
          t.push_back(k);
          f.push_back(GetVal(img, k, i));
        }

//       cout << setprecision(3);
//       cout << fixed;
//       cout << t << endl;
//       cout << f << endl;
      TInterpol(t, f, inter);

//     cout << inter << endl;
//      GetChar();
      for (int k = 0; k < img->xsize; k++)
        {
          int val = inter[k];

          if (val < 0) val = 0;

          if (val > img->maxval) val = img->maxval;

          PutVal(img, k, i, val);
        }
    }

  for (int i = 0; i < img->xsize; i++) // alle spalten
    {
      // in der Spalte extrapolieren
      vector<double> t;
      vector<double> f;
      vector<double> inter(img->ysize);

      for (int k = 0; k < vals; k++)
        {
          t.push_back(k);
          f.push_back(GetVal(img, i, k));
        }

      TInterpol(t, f, inter);

      for (int k = 0; k < img->ysize; k++)
        {
          int val = inter[k];

          if (val < 0) val = 0;

          if (val > img->maxval) val = img->maxval;

          PutVal(img, i, k, val);
        }
    }
}

#if 0
void extra(ColorImage& ci, int vals)
{
  extra(ci.R(), vals);
  extra(ci.G(), vals);
  extra(ci.B(), vals);
}
#endif

double testf(double t, double n, int typ, int flimit)
{
  switch (typ)
    {
    case 0:
      return SinN(t, n) + 0.4 * CosN(t * 2, n);
      break;
    case 1:
    {
      double sum = 0.0;

      for (int i = 1; i <= flimit; i += 2)
        sum += CosN(t * i, n) / i / i;

      return sum;
    }
    break;
    case 2:
    {
      double sum = 0.0;

      for (int i = 1; i <= flimit; i += 2)
        sum += SinN(t * i, n) / i;

      return sum;
    }
    break;
    case 3:
    {
      double sum = 0.0;

      for (int i = 1; i <= flimit; i++)
        sum += SinN(t * i, n) / i;

      return sum;
    }
    break;
    case 4:
    {
      double sum = 0.0;

      for (int i = 2; i <= flimit; i += 2)
        sum += CosN(t * i, n) / i;

      return sum;
    }
    break;
    }

  return 0;
}

void usage(const string& pn)
{
  cout << "Aufruf: " << pn << " [<options>] [filename]" << endl;
  cout << "options:" << endl;

  cout << "Testsignalerzeugung:" << endl;
  cout << "-t t   Typ der Testfunktion (t=0..4)" << endl;
  cout << "-f f   Höchste Frequenz in Testfunktion (default: f=11)" << endl;
  cout << "-z n   Zykluslänge (default: n=64)" << endl;

  cout << "Stützstellenvorgabe" << endl;
  cout << "-s s   Zahl der Stützstellen" << endl;
  cout << "-m m   Modus der Stützstellenermittlung" << endl;
  cout << "       m=0 Zufällige Verteilung im Intervall" << endl;
  cout << "       m=1 Zufällige Auswahl diskreter Stützstellen" << endl;
  cout << "       m=2 Diskrete äquidistante Stützstellen von 0..s-1" << endl;

  cout << "-R s   Setzen Zufallsgenerator auf seed s" << endl;

  cout << "Ergebnisausgabe" << endl;
  cout << "-o fn  Ausgabe in Datei fn" << endl;
  cout << "       -o gnuplot Ausgabe mit gnuplot" << endl;
  cout << "-d     Differenz (Ausgabe der Abweichung der interpolierten Funktion von der Testvorgabe" << endl;

  //  cout << "-H f   Hochpass mit Grenzfrequenz f" << endl;
  //  cout << "-T f   Tiefpass mit Grenzfrequenz f" << endl;
  cout << "-h     Diese Hilfe ausgeben" << endl;

  exit(1);
}

void gpsend(FILE* gp, const char* s)
{
  fprintf(gp, "%s\n", s);
  fflush(gp);
}

void gpsend(FILE* gp, const string& s)
{
  gpsend(gp, s.c_str());
}

void gpsend(FILE* gp, double d1, double d2)
{
  char hbuf[300];
  sprintf(hbuf, "%f %f", d1, d2);
  gpsend(gp, hbuf);
}

int main(int argc, char** argv)
{
  int rc;
  int s = 57;
  int z = 64;
  int typ = 1;
  int smode = 0;

  int flimit = 10;

  int maxfreq = 0;

  string ofn = "";

  bool dif = false;

  int rseed = 0;

  while ((rc = getopt(argc, argv, "z:o:m:s:R:dht:f:F:")) >= 0)
    {
      switch (rc)
        {
        // Zykluslänge
        case 'z':
          z = atol(optarg);
          break;

        // Testfunktions-Typ
        case 't':
          typ = atol(optarg);
          break;
        // max. Frequenz in Testfunktion
        case 'f':
          flimit = atol(optarg);
          break;

        // Typ und Zahl der Stützstellen
        case 'm':
          smode = atol(optarg);
          break;
        case 's':
          s = atol(optarg);
          break;

        case 'F':
          maxfreq = atol(optarg);
          break;

        case 'R':
          rseed = atol(optarg);
          break;

        case 'o':
          ofn = optarg;
          break;
        case 'd':
          dif = true;
          break;
        case 'h':
        default:
          usage(argv[0]);
          break;
        }
    }

  vector<double> t;
  vector<double> f;
  vector<double> inter(z);

  // initialisieren des Zufallszahlen-Genenrators
  if (rseed != 0) srand48(rseed);
  else srand48(time(NULL));

  switch (smode)
    {
    case 0:

      for (int i = 0; i < s; i++)
        {
          double s = z * drand48();
          t.push_back(s);
        }

      sort(t.begin(), t.end());
      break;
    case 1:
    {
      int ct = 0;

      if (s > z) s = z;

      vector<bool> ss(z);

      for (int i = 0; i < z; i++)
        ss[z] = false;

      while (ct < s)
        {
          int st;

          do
            {
              st = lrand48() % z;
            }
          while (ss[st]);

          ss[st] = true;
          ct++;
        }

      for (int i = 0; i < z; i++)
        {
          if (ss[i])
            t.push_back(i);
        }
    }
    break;
    case 2:
    {
      if (s > z) s = z;

      for (int i = 0; i < s; i++)
        {
          t.push_back(i);
        }
    }
    }

  for (int i = 0; i < s; i++)
    f.push_back(testf(t[i], z, typ, flimit));

  TInterpol(t, f, inter, maxfreq);

  //  cout << fixed << setprecision(2);
  /*
    for (int i=0;i<inter.size();i+=n)
    cout << i << " " << CosN(i,hp)-0.4*CosN(i*3,hp) << endl;
    cout << endl << endl;
  */

  if (ofn == "gnuplot")
    {
      FILE* gp = popen("gnuplot >/dev/null", "w");
      //      cout << gp << endl;
      gpsend(gp, "plot '-' title 'Stützstellen','-' with lines title 'interpoliert','-' title 'Original' with lines");

      for (unsigned int i = 0; i < t.size(); i++)
        gpsend(gp, t[i], f[i]);

      gpsend(gp, "e");

      for (unsigned int i = 0; i < inter.size(); i++)
        if (dif)
          gpsend(gp, i, inter[i] - testf(i, z, typ, flimit));
        else
          gpsend(gp, i, inter[i]);

      gpsend(gp, "e");

      for (unsigned int i = 0; i < inter.size() * 10; i++)
        gpsend(gp, i * 0.1, testf(i * 0.1, z, typ, flimit));

      gpsend(gp, "e");

      getchar();
      pclose(gp);
    }
  else
    {
      for (unsigned int i = 0; i < t.size(); i++)
        cout << t[i] << " " << f[i] << endl;

      cout << endl << endl;

      for (unsigned int i = 0; i < inter.size(); i++)
        if (dif)
          cout << i << " " << inter[i] - testf(i, z, typ, flimit) << endl;
        else
          cout << i << " " << inter[i] << endl ;

      cout << endl << endl;

      for (unsigned int i = 0; i < inter.size(); i++)
        cout << i << " " << testf(i, z, typ, flimit) << endl;

      cout << endl;
    }

  return 0;
}

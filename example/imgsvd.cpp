#include <image.h>

using namespace ice;

static double st;

void start()
{
  st = TimeD(TM_PROCESS);
}

void stop(const string& s)
{
  Print(s + ": " + NumberString(TimeD(TM_PROCESS) - st) + " s\n");
}

void usage(const string& pn)
{
  cout << "Usage: " << pn << " <options>" << endl;
  cout << "Options:" << endl;
  cout << " -h             this Help" << endl;
  cout << " -f <filename>  name of image File" << endl;
  cout << " -s <nsv>       number of singular values" << endl;
  exit(1);
}

int main(int argc, char* argv[])
{
  int rc;
  string filename = "test_gray.jpg";
  int svlast = 0;

  while ((rc = getopt(argc, argv, "s:hf:")) >= 0)
    {
      switch (rc)
        {
        case 'f':
          filename = optarg;
          break;
        case 's':
          svlast = atol(optarg);
          break;
        case 'h':
        default:
          usage(argv[0]);
          break;
        }
    }

  ClearAlpha();
  Print("Lese " + filename + " ");;

  int xm, ym, mv, nr;
  InfImgFile(filename, xm, ym, mv, nr);
  Printf(" %dx%d \n", xm, ym);

  int dim = xm;
  if (dim < ym)
    dim = ym;

  Image img1;
  img1.create(dim, dim, 255);
  img1.set(0);

  img1.read(filename);

  Image img2 = NewImg(dim, dim, 255);
  ImageD imgd = NewImgD(dim, dim);

  Show(ON, img1);
  Show(ON, img2);

  int i, j, k;
  Matrix a(img1);

  Matrix u, v, d, r;

  Print("Test Singulärwert-Zerlegung\n");
  start();

  SingularValueDcmp(a, u, d, v);
  stop("SVD");

#if 0

  for (int i = 1; i < dim; i++)
    {
      double delta =  d[i][i] - d[i - 1][i - 1];

      if (delta > 0)
        {
          cout << i << ":" << d[i][i];

          if (i > 0) cout << " " << d[i][i] - d[i - 1][i - 1] ;

          cout << endl;
        }
    }

#endif

  Print("Kontrollrechnung über Matrix-Produkt .. \n");
  start();
  r = u * d * (!v);
  stop("Matrix-Multiplikation");

  setImg(img2, r, RAW, UNSIGNED);

  GetChar();

  clearImgD(imgd);
  bool abort = false;
  for (k = 0; !abort && k < dim; k++)
    {
      char c;
      double fak = d[k][k];
      Print("K: " + NumberString(k) + " Faktor: " + NumberString(fak) + "\n");

      for (i = 0; i < dim; i++)
        for (j = 0; j < dim; j++)
          {
            double g = GetValD(imgd, j, i);
            g = g + u[i][k] * fak * v[j][k];
            PutValD(imgd, j, i, g);
          }

      ConvImgDImg(imgd, img2, RAW);

      if (k >= svlast)
        {
          c = GetChar();
          abort = (c == 'X') || (c == 'x');
        }
    }

  //  Eigenwerte
  //  Vector l;

  Printf("Test zu Eigenwerten\n");

  Image img1x = NewImg(dim * 2, dim * 2, 255);
  Image img2x = NewImg(dim * 2, dim * 2, 255);

  Show(OFF, img1);
  Show(OFF, img2);
  Show(ON, img1x);
  Show(ON, img2x);

  img1x.set(127);

  // Symmetrisches Bild erzeugen
  for (i = 0; i < dim; i++)
    for (j = 0; j < dim; j++)
      {
        int g = GetVal(img1, i, j);
        PutVal(img1x, i, j + dim, g);
        PutVal(img1x, j + dim, i, g);
      }

  a = Matrix(img1x);
  d = Matrix(dim * 2, dim * 2);

  start();
  Eigenvalue(a, d, u);
  stop("Eigenvalue");

  //  for (i=0;i<dim*2;i++)
  //    d[i][i]=l[i];

  Print("Kontrollrechnung über Matrix-Produkt .. ");
  r = u * d * (!u);
  Print("fertig\n");

  setImg(img2x, r);

  GetChar();

  return 0;
}

# 1 "fttest.ag"
// test der Klasse FourierTrafo
# 2 "fttest.ag"

# 3 "fttest.ag"
#include <fouriertrafo.h>
# 4 "fttest.ag"

# 5 "fttest.ag"
#include <iostream>
# 6 "fttest.ag"
#include <iomanip>
# 7 "fttest.ag"

# 8 "fttest.ag"
/*AppGen:Global*/
#include <getopt.h>
#include <string>
/*AppGen:GlobalEnd*/
# 9 "fttest.ag"

# 10 "fttest.ag"
using namespace std;
# 11 "fttest.ag"
using namespace ice;
# 12 "fttest.ag"
/*AppGen
# 13 "fttest.ag"
 prog: Test der Fouriertrafo in ICE
# 14 "fttest.ag"
 %% Beschreibung Parameter
# 15 "fttest.ag"
 % symbolischerName, Art, Typ,   Variablenname, Erklärung, Default-Wert
# 16 "fttest.ag"
 para: dir, optional, string, dir, Verzeichnis mit Bildern, "."
# 17 "fttest.ag"
 %
# 18 "fttest.ag"
 opt: n, nvalues, int, size, Zahl der Werte, 15
# 19 "fttest.ag"
 opt: w, usewisdom, void, wisdom, wisdom.txt laden, false
# 20 "fttest.ag"
 AppGen*/
# 21 "fttest.ag"

# 22 "fttest.ag"
/*AppGen:Main*/
string ag_programName;

void usage()
{
  cout << ag_programName << " - Test der Fouriertrafo in ICE" << endl;
  cout << "Usage:" << endl;
  cout << ag_programName << " [<options>] [dir] " << endl;
  cout << "  dir - Verzeichnis mit Bildern (optional, default:\".\")" << endl;
  cout << "Options:" << endl;
  cout << "  -n <i>    --nvalues=<i>" << endl;
  cout << "     Zahl der Werte (default: 15)" << endl;

  cout << "  -w        --usewisdom" << endl;
  cout << "     wisdom.txt laden (default: false)" << endl;

  exit(1);
}

void error(const string& msg)
{
  cout << endl << ag_programName << " - error: " << msg << endl << endl;
  usage();
}

int ptoi(const char* para)
{
  char* end;
  int res = strtol(para, &end, 10);
  if (end == para)
    error(string("no int: ") + para);
  if (*end != 0)
    error(string("garbage in int: ") + para);
  return res;
}

double ptod(const char* para)
{
  char* end;
  double res = strtod(para, &end);
  if (end == para)
    error(string("no double: ") + para);
  if (*end != 0)
    error(string("garbage in double: ") + para);
  return res;
}

int main(int argc, char** argv)
{
  int size = 15;
  bool wisdom = false;
  string dir = ".";
  static struct option ag_long_options[] =
  {
    {"nvalues", required_argument, 0, 'n' },
    {"usewisdom", no_argument, 0, 'w' },
    {0,         0,                 0,  0 }
  };
  ag_programName = argv[0];
  int rc;
  opterr = 0;
  while ((rc = getopt_long(argc, argv, ":n:w", ag_long_options, NULL)) >= 0)
    {
      switch (rc)
        {
        case '?':
          error("Unknown option");
          break;
        case ':':
          error("Expecting option parameter");
          break;
        case 'n':
          size = ptoi(optarg);
          break;

        case 'w':
          wisdom = true;
          break;

        default:
          error("error in options");
        }
    }
  if (optind < argc)
    dir = argv[optind++];

  /*AppGen:MainEnd*/
# 26 "fttest.ag"

# 27 "fttest.ag"
  if (wisdom)
# 28 "fttest.ag"
    fftw_import_wisdom_from_filename("wisdom.txt");
# 29 "fttest.ag"

# 30 "fttest.ag"
  int i0 = size / 2;
# 31 "fttest.ag"
  // the function
# 32 "fttest.ag"
  vector<double> fr(size, 0.0);
# 33 "fttest.ag"
  vector<double> fi(size, 0.0);
# 34 "fttest.ag"

# 35 "fttest.ag"
  // the spectrum
# 36 "fttest.ag"
  vector<double> sr(size);
# 37 "fttest.ag"
  vector<double> si(size);
# 38 "fttest.ag"

# 39 "fttest.ag"
  // the back transformed function
# 40 "fttest.ag"
  vector<double> gr(size);
# 41 "fttest.ag"
  vector<double> gi(size);
# 42 "fttest.ag"

# 43 "fttest.ag"
  /*
  # 44 "fttest.ag"
   * test with e^{i*phi}-function
  # 45 "fttest.ag"
   */
# 46 "fttest.ag"
  for (int i = 0; i < size; i++)
# 47 "fttest.ag"
    {
# 48 "fttest.ag"
      double phi = 2 * M_PI * (i - i0) / size;
# 49 "fttest.ag"
      fr[i] = cos(phi);
# 50 "fttest.ag"
      fi[i] = sin(phi);
# 51 "fttest.ag"
    }
# 52 "fttest.ag"

# 53 "fttest.ag"
  FourierTrafo ft(size, true, true);
# 54 "fttest.ag"
  ft.setInput(fr, fi);
# 55 "fttest.ag"
  ft.getResult(sr, si);
# 56 "fttest.ag"

# 57 "fttest.ag"
  double error = 0;
# 58 "fttest.ag"
  for (int i = 0; i < size; i++)
# 59 "fttest.ag"
    {
# 60 "fttest.ag"
      double real = sr[i];
# 61 "fttest.ag"
      double imag = si[i];
# 62 "fttest.ag"
      // cout << i << " " << real << " " << imag << "   " << sqrt(real*real+imag*imag) << endl;
# 63 "fttest.ag"
      if (i == i0 + 1)
# 64 "fttest.ag"
        {
# 65 "fttest.ag"
          real -= sqrt(size);
# 66 "fttest.ag"
        }
# 67 "fttest.ag"
      error += real * real + imag * imag;
# 68 "fttest.ag"
    }
# 69 "fttest.ag"
  cout << "Error spectrum of e^ix: " << sqrt(error / size) << endl;
# 70 "fttest.ag"

# 71 "fttest.ag"
  /*
  # 72 "fttest.ag"
   * FT + FT^-1, random data
  # 73 "fttest.ag"
   */
# 74 "fttest.ag"

# 75 "fttest.ag"
  for (int i = 0; i < size; i++)
# 76 "fttest.ag"
    {
# 77 "fttest.ag"
      fr[i] = drand48();
# 78 "fttest.ag"
      fi[i] = drand48();
# 79 "fttest.ag"
    }
# 80 "fttest.ag"

# 81 "fttest.ag"
  // we reuse ft here
# 82 "fttest.ag"
  //   FourierTrafo ft(size, true, true);
# 83 "fttest.ag"
  ft.setInput(fr, fi);
# 84 "fttest.ag"
  ft.getResult(sr, si);
# 85 "fttest.ag"

# 86 "fttest.ag"
  FourierTrafo fti(size, false, true);
# 87 "fttest.ag"
  fti.setInput(sr, si);
# 88 "fttest.ag"
  fti.getResult(gr, gi);
# 89 "fttest.ag"

# 90 "fttest.ag"
  error = 0;
# 91 "fttest.ag"
  for (int i = 0; i < size; i++)
# 92 "fttest.ag"
    {
# 93 "fttest.ag"
      //  cout << i << " " << fr[i] << " " << fi[i] << "   ====   " << gr[i] << " " << gi[i] << endl;
# 94 "fttest.ag"
      double er = fr[i] - gr[i];
# 95 "fttest.ag"
      double ei = fi[i] - gi[i];
# 96 "fttest.ag"
      error += er * er + ei * ei;
# 97 "fttest.ag"
    }
# 98 "fttest.ag"

# 99 "fttest.ag"
  cout << "Error of FT + inverse FT (random data): " << sqrt(error / size) << endl;
# 100 "fttest.ag"

# 101 "fttest.ag"
  /*
  # 102 "fttest.ag"
   * FT + FT , random data
  # 103 "fttest.ag"
   * == reflection
  # 104 "fttest.ag"
   */
# 105 "fttest.ag"

# 106 "fttest.ag"
  for (int i = 0; i < size; i++)
# 107 "fttest.ag"
    {
# 108 "fttest.ag"
      fr[i] = drand48();
# 109 "fttest.ag"
      fi[i] = drand48();
# 110 "fttest.ag"
    }
# 111 "fttest.ag"

# 112 "fttest.ag"
  // we reuse ft here
# 113 "fttest.ag"
  //   FourierTrafo ft(size, true, true);
# 114 "fttest.ag"
  ft.setInput(fr, fi);
# 115 "fttest.ag"
  ft.getResult(sr, si);
# 116 "fttest.ag"

# 117 "fttest.ag"
  // "wrong" inverse trafo results in reflection
# 118 "fttest.ag"
  ft.setInput(sr, si);
# 119 "fttest.ag"
  ft.getResult(gr, gi);
# 120 "fttest.ag"

# 121 "fttest.ag"
  error = 0;
# 122 "fttest.ag"
  for (int i = 0; i < size; i++)
# 123 "fttest.ag"
    {
# 124 "fttest.ag"
      int i1 = (i0 - i + i0 + size) % size;
# 125 "fttest.ag"
      // cout << i << " " << fr[i] << " " << fi[i] << " == " << gr[i1] << " " << gi[i1] << endl;
# 126 "fttest.ag"
      double er = fr[i] - gr[i1];
# 127 "fttest.ag"
      double ei = fi[i] - gi[i1];
# 128 "fttest.ag"
      error += er * er + ei * ei;
# 129 "fttest.ag"
    }
# 130 "fttest.ag"

# 131 "fttest.ag"
  cout << "Error of FT + FT == reflection (random data): " << sqrt(error / size) << endl;
# 132 "fttest.ag"
}

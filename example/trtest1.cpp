#include <image.h>

void usage(const string& prog)
{
  cout << "Aufruf: " << endl;
  cout << "    " << prog << " [options] " << endl << endl;
  cout << "Optionen" << endl;
  cout << "-i     Transformationsmodus INTERPOL" << endl;
  cout << "-d     Keine Darstellung" << endl;
  cout << "-x     Exakte Messung mit 1000 Durchläufen" << endl;
  cout << "-w <n> n Sekunden warten vor der Messung" << endl;
  cout << "-h     Hilfestellung" << endl;
  exit(5);
}

int main(int argc, char* argv[])
{
  bool display = true;
  int anz = 100;
  int mode = DEFAULT;
  int rc;
  int wait = 500000; // 5 s warten

  while ((rc = getopt(argc, argv, "idxw:h")) >= 0)
    {
      switch (rc)
        {
        case 'i':
          mode = INTERPOL;
          break;
        case 'd':
          display = false;
          break;
        case 'x':
          anz = 1000;
          break;
        case 'w':
          wait = atol(optarg);
          break;
        case 'h':
        default:
          usage(argv[0]);
          break;
        }
    }

  double time;
  Vector w(100);
  Trafo tr1(2, 2);
  Trafo tr2, tr3;
  // Originalbild
  Image img = NewImg(511, 511, 255);

  // Transformiertes Bild
  Image himg = NewImg(511, 511, 255);
  // Markierungsbild für ungültige Punkte
  Image mark = NewImg(511, 511, 255);

  clearImg(img);
  setImg(img, 111);

  for (int i = 0; i < 300; i += 10)
    Line(5, 95, 477, i, 255, img);

  if (display)
    {
      Display(ON);
      Show(OVERLAY, himg, mark);
    }

  // eine transformation konstruieren

  tr1.shift(-255, -255);
  tr1.rotate(0, 0, 1);
  tr1.shift(255, 255);

  usleep(wait);
  time = TimeD();

  for (int i = 0; i < anz; i++)
    {
      Transform(tr1, img, himg, mode);
    }

  cout << (TimeD() - time) * 1000 / anz << endl;

  time = TimeD();

  for (int i = 0; i < anz; i++)
    {
      Transform(tr1, img, himg, mode, mark, 1);
    }

  cout << (TimeD() - time) * 1000 / anz << endl;

  if (display) GetChar();

  return OK;
}


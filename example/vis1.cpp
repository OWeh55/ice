/*
 *
 *  Test-Programm zur Laufzeit der Pixelzugriffe
 *       mit und ohne Visualisierung
 */

#include <iomanip>

#include <image.h>
#include <dtime.h>
#include <unistd.h>

int sx = 1024;
int sy = 1024;

const int maxvalue1 = 127;
const int maxvalue2 = 127 * 255;
const int maxvalue3 = 32 * 255 * 255 * 255;

int mc = 1000;
int msleep = 1000000;

const double timefac = 1e9; // unit nano second

// simple timer
double timea;

void t_start()
{
  cerr << "." ;
  timea = TimeD(TM_PROCESS);
}

double t_stop()
{
  return TimeD(TM_PROCESS) - timea;
}

void setPixelTest(Image& img, int maxc)
{
  int xm = img->xsize;
  int ym = img->ysize;
  int mv = img->maxval;

  for (int ct = 0; ct < maxc; ct++)
    for (int y = 0; y < ym; y++)
      for (int x = 0; x < xm; x++)
        img.setPixel(x, y, (x + y + ct) % mv);
}

void getPixelTest(Image& img, int maxc)
{
  volatile int v;
  int xm = img->xsize;
  int ym = img->ysize;

  for (int ct = 0; ct < maxc; ct++)
    for (int y = 0; y < ym; y++)
      for (int x = 0; x < xm; x++)
        v = img.getPixel(x, y);
}

void getPixelUncheckedTest(Image& img, int maxc)
{
  volatile int v;
  int xm = img->xsize;
  int ym = img->ysize;

  for (int ct = 0; ct < maxc; ct++)
    for (int y = 0; y < ym; y++)
      for (int x = 0; x < xm; x++)
        v = img.getPixelUnchecked(x, y);
}

void setPixelUncheckedTest(Image& img, int maxc)
{
  int mv = img->maxval;
  int xm = img->xsize;
  int ym = img->ysize;

  for (int ct = 0; ct < maxc; ct++)
    for (int y = 0; y < ym; y++)
      for (int x = 0; x < xm; x++)
        img.setPixelUnchecked(x, y, (x + y + ct) % mv);
}

int main(int argc, char* argv[])
{
  vector<Image> images;
  vector<vector<double>> times;
  int rc;
  vector<int> maxval;

  while ((rc = getopt(argc, argv, "x:y:c:s:m:")) >= 0)
    {
      switch (rc)
        {
        case 'x':
          sx = atol(optarg);
          break;
        case 'y':
          sy = atol(optarg);
          break;
        case 'c':
          mc = atol(optarg);
          break;
        case 'm':
          maxval.push_back(atol(optarg));
          break;
        case 's':
          msleep = atol(optarg);
          break;
        }
    }

  double tfac = 1.0 / mc / sy / sx * timefac;
  if (maxval.empty())
    {
      maxval.push_back(maxvalue1);
      maxval.push_back(maxvalue2);
      maxval.push_back(maxvalue3);
    }

  for (int i = 0; i < maxval.size(); ++i)
    {
      Image img;
      img.create(sx, sy, maxval[i]);
      images.push_back(img);
    }

  /*
   * Lesen / Schreiben mit Visualisierung
   */

  for (int i = 0; i < images.size(); ++i)
    {
      Image img = images[i];
      times.push_back(vector<double>());

      /*
       * Lesen / Schreiben mit Visualisierung
       */

      Show(ON, img);
      img.setPixel(0, 0, 0); // dummy write to force update
      usleep(msleep);

      t_start();
      setPixelTest(img, mc);
      times[i].push_back(t_stop() * tfac);

      t_start();
      getPixelTest(img, mc);
      times[i].push_back(t_stop() * tfac);

      /*
       * Lesen / Schreiben ohne Visualisierung
       */

      Show(OFF, img);
      usleep(msleep);

      t_start();
      setPixelTest(img, mc);
      times[i].push_back(t_stop() * tfac);

      t_start();
      getPixelTest(img, mc);
      times[i].push_back(t_stop() * tfac);

      /*
       * Lesen / Schreiben Unchecked
       */

      t_start();
      setPixelUncheckedTest(img, mc);
      times[i].push_back(t_stop() * tfac);

      t_start();
      getPixelUncheckedTest(img, mc);
      times[i].push_back(t_stop() * tfac);
    }

  cout << endl << "-- results --" << endl;
  cout << "                       ";
  cout << setw(19) << "setPixel";
  cout << setw(19) << "getPixel";
  cout << setw(19) << "setPixel";
  cout << setw(19) << "getPixel";
  cout << setw(19) << "setPixelUnchecked";
  cout << setw(19) << "getPixelUnchecked" << endl;
  cout << fixed;
  for (int i = 0; i < times.size(); ++i)
    {
      cout << "maxvalue = " << setw(10) << maxval[i] << ": ";
      for (int k = 0; k < times[i].size(); ++k)
        cout << setw(19) << setprecision(3) << times[i][k];
      cout << endl;
    }

  return 0;
}


#include <image.h>
// tests zu localer segmentierung mit OTSU ..

#define ANZAHL 1
#define XS 1024
#define YS 1024

void usage(const string& prog)
{
  cout << prog << ":" << endl;
  cout << "Aufruf: " << prog << " [options] <filename>" << endl;
  cout << "options:" << endl;
  cout << "-n    Umgebungsgröße" << endl;
  //  cout << "-a    Anzahl der Durchläufe (default:" << ANZAHL << " für Benchmarking)" << endl;
  cout << "-l    Schwellwert für \"gültige Klassifikation\"" << endl;
  cout << "-L    Schwellwert für \"ungültige Pixel\"" << endl;
  cout << "-1    Deinterlace und 1. Halbbild verwenden" << endl;
  cout << "-2    Deinterlace und 2. Halbbild verwenden" << endl;
  cout << "-m    Mode" << endl;
  cout << "       m = 0 Rot" << endl;
  cout << "       m = 1 Grün" << endl;
  cout << "       m = 2 Blau" << endl;
  cout << "       m = 3 Intensität" << endl;
  cout << "-h    Hilfe" << endl;
  exit(2);
}

int getValue(const ColorImage& img, int x, int y, int mode)
{
  ColorValue c = img.getPixel(x, y);
  int g = 0;
  switch (mode)
    {
    case 0:
      g = c.red;
      break;
    case 1:
      g = c.green;
      break;
    case 2:
      g = c.blue;
      break;
    case 3:
      g = c.getGray();
      break;
    }
  return g;
}

int main(int argc, char* argv[])
{
  int x, y;
  int anz = 0;
  int rc;

  double level = 4;
  int level2 = 1000000;

  int neighbor = 11;

  int mode = 3;
  string outfilename;
  bool colorout = false;
  bool grayout = false;

  int halbbild = 0;

  ColorImage i1;

  while ((rc = getopt(argc, argv, "hl:L:n:a:o:O:12m:")) >= 0)
    {
      switch (rc)
        {
        case 'h':
          usage(argv[0]);
          break;

        case 'l':
          level = atof(optarg);
          break;
        case 'L':
          level2 = atol(optarg);
          break;
        case 'n':
          neighbor = atol(optarg);
          break;
        case 'a':
          anz = atol(optarg);
          break;
        case 'o':
          outfilename = optarg;
          grayout = true;
          break;
        case 'O':
          outfilename = optarg;
          colorout = true;
          break;
        case '1':
          halbbild = 1;
          break;
        case '2':
          halbbild = 2;
          break;
        case 'm':
          mode = atol(optarg);
          break;
        default:
          usage(argv[0]);
        }
    }

  if (argc - optind > 0) // mindestens ein verbleibender Parameter = Dateiname
    {
      string fn = argv[optind];
      i1.read(fn);

      if (anz == 0)
        anz = 1;
    }
  else
    {
      // artificial image for runtime check
      i1.create(XS, YS, 255);

      for (int y = 0; y < i1.ysize; ++y)
        for (int x = 0; x < i1.xsize; ++x)
          {
            double rand = (drand48() - 0.5) * 10 * x / i1.xsize;
            int g = 0;
            if ((x / 20) & 1)
              {
                g = y / 4 + rand;
              }
            else
              {
                g = 255 - (y / 4) + rand;
              }
            i1.setPixelLimited(x, y, ColorValue(g, g, 255 - g));
          }

      if (anz == 0)
        anz = ANZAHL;
    }

  vector<string> modes {"rot", "gruen", "blau", "Y"};
  string modestring = modes[mode];

  Image m1;
  m1.create(i1.xsize, i1.ysize, 4);
  m1.set(0);

  //  Image eval;
  //  eval.create(i1.xsize, i1.ysize, 255);

  if (halbbild == 1)
    {
      for (int y = 1; y < i1.ysize - 1; y += 2)
        for (int x = 0; x < i1.xsize; ++x)
          {
            i1.setPixel(x, y, (i1.getPixel(x, y - 1) + i1.getPixel(x, y + 1)) / 2);
          }
    }
  else if (halbbild == 2)
    {
      for (int y = 2; y < i1.ysize - 1; y += 2)
        for (int x = 0; x < i1.xsize; ++x)
          {
            i1.setPixel(x, y, (i1.getPixel(x, y - 1) + i1.getPixel(x, y + 1)) / 2);
          }
    }

  Show(ON, i1);
  Show(OVERLAY, m1, "Modus: " + modestring);
  //  Show(ON, eval, "Bewertung Modus: " + modestring);

  double ta = TimeD(TM_PROCESS);

  for (int run = 0; run < anz; ++run)
    {
      int nrad = (neighbor - 1) / 2;

      Histogram h(i1.maxval + 1);

      for (int y = nrad; y < i1.ysize - nrad; ++y)
        for (int x = nrad; x < i1.xsize - nrad; ++x)
          {
            int usedPixels = 0;
            h.reset(i1.maxval + 1);
            for (int dy = -nrad; dy <= nrad; ++dy)
              for (int dx = -nrad; dx <= nrad; ++dx)
                {
                  int g = getValue(i1, x + dx, y + dy, mode);
                  if (g < level2)
                    {
                      h.addValue(g);
                      ++usedPixels;
                    }
                }
            double d = 0;
            double threshold = (usedPixels < 3) ? 0 : CalcThreshold(h, d);
            //      eval.setPixelLimited(x,y,d*30);
            if (d > level)
              {
                int g = getValue(i1, x, y, mode);
                if (g < threshold)
                  m1.setPixel(x, y, 1);
                else
                  m1.setPixel(x, y, 2);
              }
          }
    }

  double t = (TimeD(TM_PROCESS) - ta) / anz;
  Printf("Im Bild %d x %d dauert LocalSeg %8.2f Sekunden\n",
         XS, YS, t);
  Printf("Das sind %6.2f us pro Pixel\n", t * 1e6 / XS / YS);

  Image stat;
  stat.create(768, 256, 7);
  Show(OVERLAY, stat, "Statistics Modus: " + modestring);
  for (int y = 0; y < i1.ysize; ++y)
    for (int x = 0; x < i1.xsize; ++x)
      {
        int label = m1.getPixel(x, y);
        ColorValue c = i1.getPixel(x, y);
        int co = 0;
        switch (label)
          {
          case 1:
            co = 1;
            break;
          case 2:
            co = 2;
            break;
          }
        stat.setPixel(c.red, c.green, co);
        stat.setPixel(c.red + 256, c.blue, co);
        stat.setPixel(c.green + 512, c.blue, co);
      }

  if (colorout)
    {
      vector<ColorValue> cv
      {
        ColorValue(127, 127, 127),
        ColorValue(255, 0, 0),
        ColorValue(0, 255, 0),
        ColorValue(0, 0, 255),
        ColorValue(255, 255, 0),
        ColorValue(127, 255, 127)
      };

      ColorImage out;
      out.create(i1.xsize, i1.ysize, 255);

      for (int y = 0; y < i1.ysize; ++y)
        for (int x = 0; x < i1.xsize; ++x)
          {
            int m = m1.getPixel(x, y) % 6;
            out.setPixel(x, y, cv[m]);
          }

      out.write(outfilename);
    }

  if (grayout)
    {
      int gv[] = {127, 0, 255, 192, 64};
      Image gray;
      gray.create(i1.xsize, i1.ysize, 255);
      for (int y = 0; y < i1.ysize; ++y)
        for (int x = 0; x < i1.xsize; ++x)
          {
            int m = m1.getPixel(x, y) % 5;
            gray.setPixel(x, y, gv[m]);
          }

      gray.write(outfilename);
    }


  GetChar();
  return OK;
}

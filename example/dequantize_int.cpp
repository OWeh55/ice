#include <image.h>

#define superq 1000

bool verbose = false;
bool marker = false;

void Dequantize(const Image& qimg, int step, Image& iimg)
{
  int xs = qimg->xsize;
  int ys = qimg->ysize;
  int step2 = step / 2 * superq;
  Image h = NewImg(xs, ys, qimg->maxval * superq);
  Image mark;

  for (int y = 0; y < ys; y++)
    for (int x = 0; x < xs; x++)
      PutVal(h, x, y, GetVal(qimg, x, y)*superq);

  if (verbose)
    {
      if (marker)
        {
          mark = NewImg(qimg->xsize, qimg->ysize, 255);
          clearImg(mark);
          Show(OVERLAY, h, mark);
        }
      else
        Show(ON, h, "debug");
    }

  long int change;
  int istep = 0;

  do
    {
      change = 0;
#ifdef OPENMP
      #pragma omp parallel for schedule(dynamic,40)
#endif

      for (int y = 1; y < ys - 1; y++)
        {
          int y1 = y - 1;
          int y2 = y + 1;

          for (int x = 1; x < xs - 1; x++)
            {
              int x1 = x - 1;
              int x2 = x + 1;
              int sval = GetValUnchecked(qimg, x, y) * superq;
              int aval = GetValUnchecked(h, x, y);
              int val = (aval +
                         GetValUnchecked(h, x1, y) +
                         GetValUnchecked(h, x2, y) +
                         GetValUnchecked(h, x, y1) +
                         GetValUnchecked(h, x, y2) + 2) / 5;

              if (marker)
                PutValUnchecked(mark, x, y, 0);

              if (val > sval + step2)
                {
                  val = sval + step2;

                  if (marker)
                    PutValUnchecked(mark, x, y, 1);
                }

              if (val < sval - step2)
                {
                  val = sval - step2;

                  if (marker)
                    PutValUnchecked(mark, x, y, 2);
                }

              if (val != aval)
                {
                  PutValUnchecked(h, x, y, val);
                  change += abs(val - aval);

                  if (marker)
                    PutValUnchecked(mark, x, y, 3);
                }
            }
        }

      istep++;
      cout << istep << ": " << change << endl;
    }
  while (change > 0);

  iimg = NewImg(qimg, true);

  for (int y = 0; y < ys; y++)
    for (int x = 0; x < xs; x++)
      PutVal(iimg, x, y, GetVal(h, x, y) / superq);
}

int main(int argc, char** argv)
{

  Image src;
  int quants = 1;
  int step = 0;
  bool inter = false;
  string outfile;
  string filename;

  Alpha(ON);
  Printf("Dequantize\n");

  int rc;

  while ((rc = getopt(argc, argv, "q:s:io:mv")) >= 0)
    {
      switch (rc)
        {
        case 'q':
          quants = atoi(optarg);
          break;
        case 's':
          step = atoi(optarg);
          break;
        case 'i':
          inter = true;
          break;
        case 'o':
          outfile = optarg;
          break;
        case 'm':
          marker = true;
          break;
        case 'v':
          verbose = true;
          break;
        }
    }

  if (optind < argc) // verbleibender Parameter ?
    filename = argv[optind]; // muß Filename sein
  else
    {
      cerr << " kein Dateiname angegeben" << endl;
      exit(1);
    }

  if (step == 0)
    step = quants;

  if (step < 2)
    {
      cerr << "Stufenhöhe zu klein" << endl;
      exit(1);
    }

  src = ReadImg(filename);

  // for testing purposes we quantize the image
  if (verbose)
    Show(ON, src, "Original");

  Image quant = NewImg(src);

  if (verbose)
    Show(ON, quant, "Quantisiert");

  for (int y = 0; y < src->ysize; y++)
    for (int x = 0; x < src->xsize; x++)
      {
        int g = GetVal(src, x, y);
        g = g / quants * quants + quants / 2;
        PutVal(quant, x, y, g);
      }

  Image sext;

  Dequantize(quant, step, sext);

  Show(ON, sext, "Smoothed Extended Depth");
#if 0
  Image diff = NewImg(src);
  Show(ON, diff);

  for (int y = 0; y < src->ysize; y++)
    for (int x = 0; x < src->xsize; x++)
      {
        int v1 = GetVal(src, x, y);
        int v2 = GetVal(sext, x, y);
        int diffv = v1 - v2;
        PutVal(diff, x, y, diffv * diff->maxval / step + diff->maxval / 2);
      }

  //  HistogramEqual(diff);
#endif

  //  SubImg(sext,src,diff);
  if (inter)
    {
      rc = 1;

      while (rc >= 0)
        {
          Point p = SelPoint(1, src, rc);
          int g1 = GetVal(src, p.x, p.y);
          //      int g2=GetVal(ext,p.x,p.y);
          int g3 = GetVal(sext, p.x, p.y);
          Printf("(%d,%d) %d %d\n", (int)p.x, (int)p.y, g1, g3);
        }
    }

  if (!outfile.empty())
    WriteImg(sext, outfile);
  else
    {
      Print("Irgend eine Taste drücken");
      GetChar();
    }

  return 0;
}

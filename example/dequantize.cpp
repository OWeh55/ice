#include <image.h>

bool verbose = false;
bool marker = false;
double nada = 0.001;

class vpoint
{
public:
  double x;
  double y;
  double val;
};

const int ds = 1;

void Dequantize(const Image& qimg, int step, Image& iimg)
{
  int xs = qimg->xsize;
  int ys = qimg->ysize;
  double step2 = step / 2;
  ImageD h = NewImgD(xs, ys);
  ImageD h1 = NewImgD(xs, ys);
  Image show, mark;

  for (int y = 0; y < ys; y++)
    for (int x = 0; x < xs; x++)
      {
        PutValD(h, x, y, GetValUnchecked(qimg, x, y));
        PutValD(h1, x, y, GetValUnchecked(qimg, x, y));
      }

  if (verbose)
    {
      if (marker)
        {
          mark = NewImg(qimg->xsize, qimg->ysize, 255);
          ClearImg(mark);
          Show(OVERLAY, mark, "status");
        }

      show = NewImg(qimg);
      Show(ON, show, "aktuelles Filterergebnis");
    }

  double change;
  int istep = 0;

  do
    {
      change = 0;
#ifdef OPENMP
      #pragma omp parallel for schedule(dynamic,40)
#endif

      for (int y = 1; y < ys - 1; y++)
        {
          int y1 = y - ds;
          int y2 = y + ds;

          for (int x = 1; x < xs - 1; x++)
            {
              int x1 = x - ds;
              int x2 = x + ds;
              double sval = GetValUnchecked(qimg, x, y);

              double aval = GetValD(h, x, y);
              double val = (aval +
                            GetValD(h, x1, y) +
                            GetValD(h, x2, y) +
                            GetValD(h, x, y1) +
                            GetValD(h, x, y2)) / 5.0;

              //        cout << sval << " " << aval << " " << val << endl;

              if (marker)
                PutValUnchecked(mark, x, y, 4);

              // val += ((val-aval)/4);

              // Begrenzung auf original +- step
              double svalp = sval + step2;

              //        cout << "ul:" << svalp << endl;
              if (val > svalp)
                {
                  val = svalp;

                  if (marker)
                    PutValUnchecked(mark, x, y, 1);
                }
              else
                {
                  double svalm = sval - step2;

                  //  cout << "ll: " << svalm << endl;
                  if (val < svalm)
                    {
                      val = svalm;

                      if (marker)
                        PutValUnchecked(mark, x, y, 2);
                    }
                }

              PutValD(h1, x, y, val);
              double delta = fabs(val - aval);

              if (delta > change) change = delta;

              if (marker)
                {
                  if (delta > nada)
                    PutValUnchecked(mark, x, y, 3);
                }
            }
        }

      for (int y = 0; y < ys; y++)
        for (int x = 0; x < xs; x++)
          PutValD(h, x, y, GetValD(h1, x, y));

      if (verbose)
        for (int y = 0; y < ys; y++)
          for (int x = 0; x < xs; x++)
            PutVal(show, x, y, limited(GetValD(h1, x, y), show));

      istep++;
      cout << istep << ": " << change << endl;
      //     GetChar();
    }
  while (change > nada);

  //  GetChar();
  iimg = NewImg(qimg, true);

  for (int y = 0; y < ys; y++)
    for (int x = 0; x < xs; x++)
      PutVal(iimg, x, y, limited(RoundInt(GetValD(h, x, y)), iimg));

  FreeImgD(h);
  FreeImgD(h1);
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

  while ((rc = getopt(argc, argv, "q:s:io:mvn:")) >= 0)
    {
      switch (rc)
        {
        case 'n':
          nada = atof(optarg);
          break;
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
        default:
          exit(1);
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

  if (verbose)
    Show(ON, src, "Original");

  Image quant = NewImg(src);

  if (verbose)
    Show(ON, quant, "Quantisiert");

  // for testing purposes we quantize the image
  for (int y = 0; y < src->ysize; y++)
    for (int x = 0; x < src->xsize; x++)
      {
        int g = GetVal(src, x, y);
        g = g / quants * quants; // + quants/2;
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

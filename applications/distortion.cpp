#include <unistd.h>
#include <image.h>
#include <lmdif.h>
#include <Distortion.h>
#include <lseg.h>
#include <fstream>

#define KALIBNAME "camera"
#define OUTFILE "out.jpg"

bool wait = false;
bool verbose = false;
bool display = false;
bool fehler = false;
bool vertical = false;

int localsegneighbour = 5;
int localseglevel = 25;
double formlimit = 1.3;

Distortion1 di;

void Wait(const Image & img)
{
  int xc, yc;
  if (wait)
    {
      GetAlphaCursor(&xc, &yc);
      Print("<ENTER>");
      //    if (display) SelPoint(DEFAULT,img);
      //    else
      GetChar();
      SetAlphaCursor(xc, yc);
      PutChar(0x10);
    }
}

void MarkerSearch(const Image & oimg, const Image & mark,
                  Matrix &markers, Matrix &smarkers)
{
  int ps[2] = {0, 0};
  double length, area, form, conv;
  bool special, has_hole;

  ClearImg(mark);
  markers = Matrix(0, 2);
  smarkers = Matrix(0, 2);

  while (SearchStart(oimg, mark, LocalSegObj, 5, 10, ps) == OK)
    {
      Contur c = CalcContur(oimg, mark, LocalSegObj, 5,
                            ps, 0, isunknown, isunknown);

      PutVal(mark, ps[0], ps[1], 3);
      if ((c.isValid()) && (!c.isHole()) && (c.isClosed()))
        {
          MarkContur(c, 1, mark); // markieren
          Contur cc = ConvexContur(c);
          FeatureContur(cc, length, area, form, conv);
          if (area > 100 && area < 20000 && form < formlimit)
            {
              /* marker found */
              /* is this a special marker ? */
              special = false;
              has_hole = false;

              int x1, y1, x2, y2;
              int psi[2];

              c.getRect(x1, y1, x2, y2);

              Window markerwindow(x1, y1, x2, y2);

              psi[0] = x1 + 1;
              psi[1] = y1 + 1;
              while (SearchStart(oimg(markerwindow), mark(markerwindow), LocalSegObj, 5, 1, psi) == OK)
                {
                  double areai, lengthi, formi, convi;
                  Contur ci = CalcContur(oimg(markerwindow), mark(markerwindow), LocalSegObj, 5,
                                         psi, 0, isunknown, isunknown);
                  PutVal(mark(markerwindow), psi[0], psi[1], 3);
                  if ((ci.isValid()) && (ci.isHole()) && (ci.isClosed()))
                    {
                      FeatureContur(ci, lengthi, areai, formi, convi);
                      areai = -areai;
                      formi = -formi;
                      if (areai > 0.1 * area) has_hole = true;
                      if (areai < area && areai > 0.3 * area && formi < formlimit)
                        special = true;
                    }
                }
              // GetChar();
              FillRegion(c, has_hole ? (special ? 2 : 1) : 3, mark);
              double m[15], s[2];
              MomentRegion(c, m, s);

              if (special)
                {
                  smarkers = smarkers && Vector(s[0], s[1]);
                }
              else
                {
                  if (has_hole)
                    markers = markers && Vector(s[0], s[1]);
                }
            }
        }
    }
  if (smarkers.rows() == 4)
    {
      /* wenn alle Spezialmarker gefunden, dann sortieren */
      int i, j;
      for (i = 0; i < 3; i++)
        for (j = i + 1; j < 4; j++)
          if (smarkers[i][0] > smarkers[j][0]) smarkers.ExchangeRow(i, j);
      if (smarkers[0][1] > smarkers[1][1]) smarkers.ExchangeRow(0, 1);
      if (smarkers[2][1] > smarkers[3][1]) smarkers.ExchangeRow(2, 3);
    }
}

int SMarkerSearch(const Image & oimg, const Image & mark, double markersize, Matrix &smarkers)
{

  int ps[2] = {0, 0};
  double length, area, form, conv;
  bool found[4];
  int anzahl = 0;
  int nr;

  for (nr = 0; nr < 4; nr++) found[nr] = FALSE;

  smarkers = Matrix(4, 2);
  while (SearchStart(oimg, mark, LocalSegObj, 5, 5, ps) == OK)
    {
      Contur c = CalcContur(oimg, mark, LocalSegObj, 5,
                            ps, 0, isunknown, isunknown);

      PutVal(mark, ps[0], ps[1], 3);
      if ((c.isValid()) && (!c.isHole()) && (c.isClosed()))
        {
          MarkContur(c, 1, mark);              // markieren
          Contur cc = ConvexContur(c);
          FeatureContur(cc, length, area, form, conv);
          if (area > markersize / 200 && area < 0.4 * markersize && form < formlimit)
            {
              FillRegion(c, 2, mark);
              double m[15], s[2];
              MomentRegion(c, m, s);

//	Printf("%g\n",sqrt(m[0]*85/markersize));
              nr = RoundInt(sqrt(m[0] * 85 / markersize)) - 1;
              if ((nr > 0) && (nr < 5))
                {
                  if (verbose) Printf("%d ", nr);
                  nr--;
                  if (!found[nr])
                    {
                      smarkers[nr] = Vector(s[0], s[1]);
                      found[nr] = TRUE;
                      anzahl++;
                    }
                }
            }
        }
    }
  return anzahl;
}

void Fehler(const Matrix &originallist, const Matrix &markerlist,
            const Trafo &tr_o_b, const Distortion &di, const Image & mark,
            bool last,
            double &diff2)
{
  int i;
  diff2 = 0.0;
  ClearImg(mark);
  Matrix rschnitt;
  if ((fehler) && (last))
    {
      cout << "#FIG 3.2" << endl;
      cout << "Landscape" << endl;
      cout << "Center" << endl;
      cout << "Metric" << endl;
      cout << "A4" << endl;
      cout << "100.00" << endl;
      cout << "Single" << endl;
      cout << "-2" << endl;
      cout << "1200 2" << endl;
    }

  TransformList(tr_o_b, originallist, rschnitt);

  for (i = 0; i < rschnitt.rows(); i++)
    {
      double xr = rschnitt[i][0];
      double yr = rschnitt[i][1];
      double xm = markerlist[i][0];
      double ym = markerlist[i][1];
      di.Distort(xr, yr);
      double dx = xr - xm;
      double dy = yr - ym;
      diff2 += dx * dx + dy * dy;
      Line(RoundInt(xm), RoundInt(ym),
           RoundInt(xm + 20 * dx), RoundInt(ym + 20 * dy),
           mark->maxval, DEFAULT, mark);
      double circle[3];
      circle[0] = xm;
      circle[1] = ym;
      circle[2] = 5;
      DrawCircle(circle, mark->maxval, mark->maxval, DEFAULT, mark);
      if ((fehler) && (last))
        {
#define FAKTOR 3
          cout << "1 3 0 1 0 7 40 0 -1 0.000 1 0.0000 ";
          cout << RoundInt(xm)*FAKTOR << " " << RoundInt(ym)*FAKTOR << " ";
          cout << RoundInt(25)*FAKTOR << " " << RoundInt(25)*FAKTOR << " ";
          cout << RoundInt(xm)*FAKTOR << " " << RoundInt(ym)*FAKTOR << " ";
          cout << RoundInt(xm + 20)*FAKTOR << " " << RoundInt(ym)*FAKTOR << endl;

          cout << "2 1 0 1 0 7 50 0 -1 0.000 0 0 -1 1 0 2 " << endl;
          cout << " 0 0 1.00 10.00 20.00" << endl;

          cout << " " << RoundInt(xm)*FAKTOR << " " << RoundInt(ym)*FAKTOR << " ";
          cout << RoundInt(xm + dx * 50)*FAKTOR << " " << RoundInt(ym + dy * 50)*FAKTOR << endl;
        }
    }
  diff2 /= rschnitt.rows();
}

void usage(string pname)
{
  cout << "Verzeichnungsberechnung" << endl << endl;
  cout << "Aufruf" << "  " << pname << " [options]" << endl << endl;
  cout << "Optionen:" << endl;

  cout << "-c infile  Kalibrierung anhand von \"infile\"" << endl;
  cout << "-e infile  Entzerrung des Bildes \"infile\"" << endl;
  cout << "-o outfile Ausgabe des entzerrten Bildes als \"outfile\"" << endl;
  cout << "           Der Standardname ist \"" << OUTFILE << "\"" << endl;

  cout << "-k kamera  Name des Verzeichnungsdaten" << endl;
  cout << "           Der Standardname ist \"" << KALIBNAME << "\"" << endl;
  cout << "-s         Senkrechte Ausrichtung der Vorlage" << endl;
  cout << "-d         Anzeige der Bilder bei der Berechnung" << endl;
  cout << "-f         Zeige Fehlerbild" << endl;
  cout << "-v         Textausgaben bei Berechnung" << endl;

  cout << "-l n       Nachbarschaftsgröße n für LocalSeg" << endl;
  cout << "-L n       Schwellwert n für LocalSeg" << endl;

  cout << "-w         Warten auf Tastendruck" << endl;

  cout << "-h         Ausgabe dieser Hilfe" << endl;
  exit(0);
}

void CopyRGB(const Image & r1, const Image & g1, const Image & b1, Image & r2, Image & g2, Image & b2)
{
  int x, y;
  wloop(r2, x, y)
  {
    PutVal(r2, x, y, GetVal(r1, x, y));
    PutVal(g2, x, y, GetVal(g1, x, y));
    PutVal(b2, x, y, GetVal(b1, x, y));
  }
}

int CalcDist(const string &filename, const string &kalibname)
{
  //  Distortion1 di;
  int xs, ys, gm;
  double diff2;

  if (verbose) Print("Analysiere " + filename + "\n");

  InfImgFile(filename, xs, ys, gm);

  Image original = NewImg(xs, ys, gm);
  Image segmented = NewImg(xs, ys, gm);
  Image mark = NewImg(xs, ys, gm);

  ClearImg(segmented);
  ClearImg(mark);

  if (verbose)
    Print("Datei " + filename + " laden");

  ReadImg(filename, original);

  if (verbose)
    Print(" beendet\n");

  if (display)
    {
      Display(ON);
      Show(OVERLAY, original, segmented);
      Zoom(original, 0, 0, 0);
    }

  if (verbose)
    Print("Segmentierung ");

  LocalSeg(original, segmented, localsegneighbour, localseglevel);

  if (verbose)
    Print("beendet\n");

  if (display)
    {
      Wait(original);
      Show(OFF, original);
      Show(OVERLAY, original, mark);
      Zoom(original, 0, 0, 0);
    }

  Matrix markerlist(0, 2);
  Matrix smarkerlist(0, 2);
  Matrix originallist(0, 2);

  if (verbose)
    Print("Suche Markierungen\n");

  MarkerSearch(segmented, mark, markerlist, smarkerlist);

  if (verbose)
    Printf("%d Markierungen gefunden\n", markerlist.rows());

  if (smarkerlist.rows() != 4)
    {
      Printf("%d Marker gefunden (statt 4)!", smarkerlist.rows());
      Wait(original);
      return ERROR;
    }

  if (display)
    {
      Wait(original);
      Show(OFF, original);
    }

  if (verbose)
    Print("\nBerechne Trafo (Startwert)\n");

  Matrix soriginallist;

  if (! vertical)
    {
      soriginallist =
        Vector(9.0, 4.0) &&
        Vector(9.0, 14.0) &&
        Vector(18.0, 4.0) &&
        Vector(18.0, 14.0);
      //    Printf("horizontal\n");
    }
  else
    {
      soriginallist =
        Vector(4.0, 9.0) &&
        Vector(4.0, 18.0) &&
        Vector(14.0, 9.0) &&
        Vector(14.0, 18.0);
      //    Printf("vertikal\n");
    }

  // Startwert projektive Trafo
  // Trafo Bild -> Original
  Trafo tr_b_o = MatchPointlistsLinOpt(smarkerlist, soriginallist, TRM_PROJECTIVE);

  if (display)
    {
      // prepare 3 images (for RGB)
      if (fehler)
        {
          Show(OVERLAY, original, mark);
        }
      else
        {
          CopyImg(original, segmented);
          SetImg(mark, mark->maxval);
          // Show(OVERLAY,original,segmented);
          Show(_RGB, original, segmented, mark);
        }
      //      Zoom(original);
    }

  for (int run = 1; run < 4; run++)
    {
      // mehrere Läufe, damit nach Bestimmung der ersten Verzeichnungswerte
      // Zuordnung korrigiert werden kann
      if (verbose)
        Printf("************** %d.Lauf *****************\n", run);

      for (int in = 0; in < 2; in++)
        {
          // mehrfach anwenden um Ausreißer noch in Griff zu bekommen

          originallist = markerlist;

          if (run > 1) // Verzeichnung ist schon mal bestimmt worden
            {
              //  cout << di.D2() << " " << di.D4() << " " << endl;
              for (int i = 0; i < originallist.rows(); i++)
                {
                  di.Rect(markerlist[i][0], markerlist[i][1],
                          originallist[i][0], originallist[i][1]);
                  //    cout << markerlist[i][0] << "," << markerlist[i][1] << " " ;
                  //		    cout << originallist[i][0] << "," << originallist[i][1] << endl;
                }
            }

          TransformList(tr_b_o, originallist);

          for (int i = 0; i < originallist.rows(); i++)
            {
              double xh = Round(originallist[i][0]);
              double yh = Round(originallist[i][1]);
              originallist[i][1] = yh;
              originallist[i][0] = xh;
              //     cout << originallist[i][0] << "," << originallist[i][1] << endl;
            }
          Trafo tr_b_o = MatchPointlists(markerlist, originallist, TRM_PROJECTIVE);
        }

      // Trafo von Original in Bild
      Trafo tr_o_b = tr_b_o.Inverse();

      // di=Distortion2(markerlist,originallist,tr_o_b);
      di.Calc(markerlist, originallist, tr_o_b);
      //di=Distortion(markerlist,originallist);

      if (verbose)
        {
          Print("Verzeichnung: " + di.toString() + "\n");
        }

      Fehler(originallist, markerlist, tr_o_b, di, mark, run == 3, diff2);

      if (verbose)
        {
          Printf("Fehler:%6.2f\n", sqrt(diff2));
        }

      if (display)
        {
          di.RectImg(original, segmented); // Darstellung der entzerrten Bildes
          Wait(original);
        }
    } /* drei durchlaeufe */

  if (display)
    Show(OFF, original);

  if ((!display) && (wait))
    GetChar();

  ofstream os(kalibname.c_str());
  os << di.toString();
  return OK;
}

int Rectify(string source, string dest, string dname)
{
  // Verzeichnung einlesen
  if (verbose)
    {
      Print("Lese Verzeichnungsdaten " + dname + "\n");
    }

  ifstream ci(dname.c_str());
  string ps;
  getline(ci, ps);
  di.Set(ps);

  // Bilder einlesen
  int xm, ym, gm;

  if (verbose)
    Print("Ermittle Bilddaten " + source + "\n");

  Image ro, go, bo;
  Image re, ge, be;

  InfImgFile(source, xm, ym, gm);

  if (verbose)
    Printf("Lege Bilder an\n");

  ro = NewImg(xm, ym, gm);
  go = NewImg(xm, ym, gm);
  bo = NewImg(xm, ym, gm);
  re = NewImg(xm, ym, gm);
  ge = NewImg(xm, ym, gm);
  be = NewImg(xm, ym, gm);

  if (verbose)
    Print("Lade " + source + "\n");

  ReadImg(source, ro, go, bo);

  if (verbose) Print("Entzerre..    ");

  if (display)
    {
      Display(ON);
      Show(_RGB, ro, go, bo, "original");
      Show(_RGB, re, ge, be, "entzerrt");
      //    Zoom(re);
    }

  di.RectImg(ro, re, INTERPOL);
  di.RectImg(go, ge, INTERPOL);
  di.RectImg(bo, be, INTERPOL);

  if (verbose) Print("Fertig\n");
  if (display && wait) GetChar();

  if (verbose)
    Print("Schreibe " + dest + "\n");
  WriteImg(re, ge, be, dest);

  if (verbose)
    Print("Done\n");
  return OK;
}

int main(int argc, char *argv[])
{
  string markerfile;
  string infile;
  string outfile = OUTFILE;
  string kalibdata = KALIBNAME ".dat";
  int c = 0;

  bool calib = false;
  bool correct = false;

  while ((c = getopt(argc, argv, "c:e:o:k:dhwvfF:sl:L:")) >= 0)
    {
      switch (c)
        {
        case 'c'://kalibrieren
          calib = true;
          markerfile = optarg;
          break;
        case 'e'://entzerren
          correct = true;
          infile = optarg;
          break;

        case 'o':// output image filename
          outfile = optarg;
          break;

        case 'k':// verzeichnungsdaten
          kalibdata = string(optarg) + ".dat";
          break;

        case 's':
          vertical = true;
          break;

        case 'd':
          display = true;
          break;

        case 'F':
          formlimit = atof(optarg);
          break;

        case 'w':
          wait = true;
          break;
        case 'v':
          verbose = true;
          break;
        case 'f':
          fehler = true;
          break;

        case 'l':
          localsegneighbour = atol(optarg);
          break;

        case 'L':
          localseglevel = atol(optarg);
          break;

        case 'h':
          usage(argv[0]);
          break;

        default:
          usage(argv[0]);
          break;
        }
    }

  if ((!calib) && (!correct))
    usage(argv[0]);

  OpenAlpha("Distortion");
  ClearAlpha();

  if (calib)
    {
      if (CalcDist(markerfile, kalibdata) != OK)
        {
          Printf("Fehler bei der Kalibrierung");
          exit(1);
        }
    }

  if (correct)
    {
      if (Rectify(infile, outfile, kalibdata) != OK)
        {
          Printf("Fehler bei der Korrektur");
          exit(1);
        }
    }

  return 0;
}

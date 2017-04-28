#include <stdio.h>
#include <math.h>
#include <image.h>

#include <fstream>

#define MW 36
#define MH 17

#define XM1 4
#define YM1 2
#define XM2 (XM1+MW)
#define YM2 (YM1+MH)

#define XS1 5
#define YS1 3
#define XS2 (XS1+MW)
#define YS2 (YS1+MH)

#define YP (YS2+2)
#define XP 0

#define XSTAT 45

Matrix polygon(0, 2);
Matrix mpolygon(0, 2);

int polygoncolor = 3; // color of current polygon
int mpolygoncolor = 1; // color of stored polygon
int oldpolygoncolor = 2; // color of old polygon

Image pic;
Image mark;

string imagefn;

Contur contur;

int sMarker = 2;

int corners = 5;
double prec = 2.0;
bool markcorners = true;
bool printpolygon = false;

void PrintStatus()
{
  SetAlphaCursor(XSTAT, 5);
  Printf("Selected Corners   %6d", corners);
  SetAlphaCursor(XSTAT, 7);
  Printf("Selected Precision %6.2f", prec);

  if (polygon.rows() > 0)
    {
      SetAlphaCursor(XSTAT, 10);
      Printf("Corners of polygon %6d", polygon.rows());
      SetAlphaCursor(XSTAT, 12);
      Printf("Error (max,max) %6.2f",
             DistanceConturPolygon(contur, polygon, DPP_MAX, DPP_MAX));
    }
}

void DrawPoly(const Matrix &poly, int color)
{
  if (poly.rows() > 100 || !markcorners)
    drawPolygon(poly, color, mark);
  else
    drawPolygon(poly, color, mark,
                sMarker, 5, color);
}

void DrawPolygons()
{
  if (mpolygon.rows() > 0)
    DrawPoly(mpolygon, mpolygoncolor);
  DrawPoly(polygon, polygoncolor);
}

void NewPolygon(const Matrix &poly)
{
  DrawPoly(polygon, oldpolygoncolor);
  polygon = poly;
  DrawPolygons();
  if (printpolygon)
    cout << polygon << endl;
}

void StorePolygon()
{
  mpolygon = polygon;
  DrawPolygons();
}

void SMMenu()
{
  // Menu mit Split & Merge verwandten Funktionen
  vector<string> men;
  vector<int> mid;
  men.push_back("~ReducePolygon(Merge)");
  mid.push_back(1);
  //  men.push_back("ReducePolygon(~SplitAndMerge)"); mid.push_back(2);
  men.push_back("ReducePolygon~Precision(Merge)");
  mid.push_back(3);
  //  men.push_back("ReducePolygonPrecision(SplitAndMerge)"); mid.push_back(4);
  men.push_back("-----------------------------");
  mid.push_back(0);
  men.push_back("~FitPolygonContur");
  mid.push_back(10);
  men.push_back("-----------------------------");
  mid.push_back(0);
  men.push_back("~Number of Corners");
  mid.push_back(50);
  men.push_back("Pr~ecision");
  mid.push_back(51);
  men.push_back("~Clear old markers");
  mid.push_back(90);
  men.push_back("-----------------------------");
  mid.push_back(0);
  men.push_back("~Store polygon");
  mid.push_back(70);
  men.push_back("E~xit");
  mid.push_back(99);

  PrintStatus();

  PushAlpha();
  int selection = Menu(men, mid, XS1, YS1, XS2, YS2, false, "Edge-based Fitting");
  while (selection != 99 && selection != -255)
    {
      SetAlphaCursor(XP, YP);

      /*************************************************/

      if (selection == 1)
        NewPolygon(ReducePolygon(contur, corners, 1));

      if (selection == 2)
        NewPolygon(ReducePolygon(contur, corners, 2));

      if (selection == 3)
        NewPolygon(ReducePolygonPrecision(contur, prec, 1));

      if (selection == 4)
        NewPolygon(ReducePolygonPrecision(contur, prec, 2));

      if (selection == 10)
        {
          Matrix p1;
	  try {
	    p1 = FitPolygonContur(polygon, contur, 0, 1000);
              NewPolygon(p1);
	  }
	  catch (IceException ex)
	    {
	      Print("Fitting impossible");
	    }
        }

      if (selection == 50)
        {
          do
            {
              SetAlphaCursor(XP, YP);
              corners = Input("Number of corners (>=3): ");
            }
          while (corners < 3);

          SetAlphaCursor(XP, YP);
          Print("\020");
        }

      if (selection == 51)
        {
          do
            {
              SetAlphaCursor(XP, YP);
              prec = InputD("Precision: ");
            }
          while (prec < 0.0);

          SetAlphaCursor(XP, YP);
          Print("\020");
        }

      if (selection == 70)
        StorePolygon();

      if (selection == 90)
        {
          clearImg(mark);
          DrawPolygons();
        }

      PrintStatus();
      selection = Menu(men, mid, XS1, YS1, XS2, YS2, false, "Edge-based Fitting");
    }
  PopAlpha();
}

void MMenu()
{
  //
  //double moment[15],s[2];
  //MomentRegion(contur,moment,s);
  Moments moment(contur);
  //  for (int i=0;i<15;i++)
  //    cout << moment[i] << endl;

  // Moment menu
  vector<string> men; // list of items
  vector<int> mid; // list of id

  men.push_back("~Triangle");
  mid.push_back(1);
  men.push_back("~Equilateral triangle");
  mid.push_back(2);
  men.push_back("~Isosceles triangle");
  mid.push_back(3);
  men.push_back("-----------------------------");
  mid.push_back(0);
  men.push_back("P~arallelogram");
  mid.push_back(4);
  men.push_back("~Rectangle");
  mid.push_back(6);
  men.push_back("S~quare");
  mid.push_back(7);
  men.push_back("-----------------------------");
  mid.push_back(0);
  men.push_back("General ~Polygon (3<=n<=7)");
  mid.push_back(5);
  men.push_back("~Number of Corners");
  mid.push_back(50);
  men.push_back("-----------------------------");
  mid.push_back(0);
  men.push_back("~Clear old markers");
  mid.push_back(90);
  men.push_back("-----------------------------");
  mid.push_back(0);
  men.push_back("~Store polygon");
  mid.push_back(70);
  men.push_back("E~xit");
  mid.push_back(99);

  PrintStatus();

  PushAlpha();
  int selection = Menu(men, mid, XS1, YS1, XS2, YS2, false, "Moments based fitting");
  while (selection != 99 && selection != -255)
    {

      if (selection == 1)
        {
          Matrix p = FitTriangle(moment);
          NewPolygon(p);
        }

      if (selection == 2)
        {
          Matrix p = FitEquilateralTriangle(moment);
          NewPolygon(p);
        }

      if (selection == 3)
        {
          Matrix p = FitIsoscelesTriangle(moment);
          NewPolygon(p);
        }

      if (selection == 4)
        {
          Matrix p = FitParallelogram(moment);
          NewPolygon(p);
        }

      if (selection == 6)
        {
          Matrix p = FitRectangle(moment);
          NewPolygon(p);
        }

      if (selection == 7)
        {
          Matrix p = FitSquare(moment);
          NewPolygon(p);
        }

      if (selection == 5)
        {
          // Momentenbasiertes Fitting fuer Polygone von n=3 bis n=7
          if (corners > 7)
            {
              SetAlphaCursor(XP, YP);
              Printf("Wrong number of corners: %d, must be 3<=n<=7", corners);
            }
          else
            {
              Matrix corners_r = ReducePolygon(contur, corners);

              // DrawPolygon(corners_r,1,mark1);

              Matrix corners_a;
	      try {
		corners_a = FitPolygonContur(corners_r, contur);
	      }
	      catch (IceException ex)
              {
                corners_a = corners_r; // try without optimization
              }

              NewPolygon(corners_a);

              Matrix p = FitPolygon(moment, corners_a);
              NewPolygon(p);
            }
        }

      if (selection == 50)
        {
          do
            {
              SetAlphaCursor(XP, YP);
              corners = Input("Number of corners (>=3): ");
            }
          while (corners < 3);

          SetAlphaCursor(XP, YP);
          Print("\020");
        }

      if (selection == 70)
        StorePolygon();


      if (selection == 90)
        {
          clearImg(mark);
          DrawPolygons();
        }
      PrintStatus();
      selection = Menu(men, mid, XS1, YS1, XS2, YS2, false, "Moments based fitting");

    }
  PopAlpha();
}


void usage(const string &pn)
{
  cout << "Aufruf: " << pn << " [<options>]" << endl;
  cout << "options:" << endl;
  cout << "-x        Width of image " << endl;
  cout << "-y        Height of image" << endl;
  cout << "-c <name> load Contur from file <name>" << endl;
  cout << "-i <name> load Image  from file <name>" << endl;
  cout << "-n <nr>   Number of corners" << endl;
  cout << "-p <prec> Precision" << endl;

  cout << "-m <nr>   Marker value" << endl;
  cout << "-M        Switch off markers" << endl;
  cout << "-C <nr>   color if Current polygon" << endl;
  cout << "-O <nr>   color if Old polygon" << endl;
  cout << "-S <nr>   color if Stored polygon" << endl;
  cout << "-P        Print polygon to standard output" << endl;

  cout << "-w        Wait" << endl;
  cout << "-l        Level for local segmentation" << endl;
  cout << "-s        Neighborhood for local segmentation" << endl;
  cout << "-h        show this Help" << endl;
  exit(1);
}

void MkMainMenu(vector<string> &men, vector<int> &mid)
{
  men.clear();
  mid.clear();
  if (contur.isValid())
    {
      men.push_back("~Edge based fitting");
      mid.push_back(10);
      men.push_back("-----------------------------");
      mid.push_back(0);
      men.push_back("~Moment based fitting");
      mid.push_back(11);
      men.push_back("-----------------------------");
      mid.push_back(0);
      if (polygon.rows() > 0)
        {
          men.push_back("~Store polygon");
          mid.push_back(70);
        }
      men.push_back("-----------------------------");
      mid.push_back(0);
      men.push_back("~Clear old markers");
      mid.push_back(90);
      men.push_back("~Write image (color)");
      mid.push_back(91);
      men.push_back("Write image (~grey)");
      mid.push_back(92);
      men.push_back("-----------------------------");
      mid.push_back(0);
    }
  men.push_back("New contur");
  mid.push_back(80);
  if (contur.isValid())
    {
      men.push_back("Save contur");
      mid.push_back(81);
    }
  men.push_back("Load contur");
  mid.push_back(82);

  men.push_back("-----------------------------");
  mid.push_back(0);
  if (!imagefn.empty())
    {
      men.push_back("Find Contur (in image)");
      mid.push_back(83);
    }

  men.push_back("E~xit");
  mid.push_back(99);
}

int main(int argc, char *argv[])
{
  bool wait = false;
  int dimx = 512;
  int dimy = 512;
  //  int farbe;

  //  int conturcolor=3;

  string cfilename;

  int ls_level = 40;
  int ls_neighbour = 7;

  int rc;
  while ((rc = getopt(argc, argv, "x:y:c:n:m:p:i:wl:s:hMP")) >= 0)
    {
      switch (rc)
        {
        case 'x':
          dimx = atol(optarg);
          break;
        case 'y':
          dimy = atol(optarg);
          break;
        case 'c':
          cfilename = optarg;
          break;
        case 'i':
          imagefn = optarg;
          break;
        case 'n':
          corners = atol(optarg);
          break;
        case 'p':
          prec = atof(optarg);
          break;
        case 'm':
          sMarker = atol(optarg);
          break;
        case 'w':
          wait = true;
          break;
        case 'P':
          printpolygon = true;
          break;
        case 'C':
          polygoncolor = atol(optarg);
          break;
        case 'S':
          mpolygoncolor = atol(optarg);
          break;
        case 'O':
          oldpolygoncolor = atol(optarg);
          break;
        case 'l':
          ls_level = atol(optarg);
          break;
        case 's':
          ls_neighbour = atol(optarg);
          break;
        case 'M':
          markcorners = false;
          break;
        case 'h':
        default:
          usage(argv[0]);
        }
    }

  int selection;


  if (!imagefn.empty())
    {
      int maxv, chnr;
      InfImgFile(imagefn, dimx, dimy, maxv, chnr);
    }

  /************************************************************/

  pic  = NewImg(dimx, dimy, 255);
  mark = NewImg(dimx, dimy, 255);
  clearImg(mark);
  Show(OVERLAY, pic, mark);
  for (int i = 0; i < 256; i++)
    {
      int v = 255 - i / 2;
      SetGreyColor(i, v, v, v);
    }
  /***********************************************************/

  OpenAlpha("Fitting-Tests");
  Alpha(ON);

  if (!imagefn.empty())
    {
      ReadImg(imagefn, pic);
    }
  else
    {
      if (!cfilename.empty())
        {
          ifstream is(cfilename.c_str());
          Matrix m;
          is >> m;
          contur = PolygonContur(m);
          FillRegion(contur, 222, pic);
        }
    }
  /**********************************************/

  SetAttribute(7, 1, 0, 0);
  ClearAlpha();

  vector<string> men;
  vector<int> mid;

  MkMainMenu(men, mid);

  PrintStatus();

  selection = Menu(men, mid, XM1, YM1, XM2, YM2, false, "Polygon Fitting");
  while (selection != 99)
    {
      //      ClearAlpha();
      SetAlphaCursor(XP, YP);

      /*************************************************/

      if (selection == 10)
        SMMenu();

      if (selection == 11)
        MMenu();

      if (selection == 90)
        {
          clearImg(mark);
          DrawPolygons();
        }

      if (selection == 91)
        {
          string fn;
          do
            {
              SetAlphaCursor(XP, YP);
              fn = InputS("Filename: ");
            }
          while (fn.empty());
          ColorImage rgb;
	  rgb.create(pic);

	  WindowWalker w(pic);
	  for (w.init();!w.ready();w.next())
          {
            int markv = mark.getPixel(w);
            int grw = pic.getPixel(w) / 2;
            switch (markv)
              {
              case 1:
                rgb.setPixel(w, ColorValue(0, 255, 255));
                break;
              case 2:
                rgb.setPixel(w, ColorValue(255, 0, 255));
                break;
              case 3:
                rgb.setPixel(w, ColorValue(255, 255, 0));
                break;
              default:
                rgb.setPixel(w, ColorValue(grw, grw, grw));
                break;
              }
          }
          rgb.write(fn);
          ClearAlpha();
        }

      if (selection == 92)
        {
          string fn;
          do
            {
              SetAlphaCursor(XP, YP);
              fn = InputS("Filename: ");
            }
          while (fn.empty());
          Image out = NewImg(pic);

	  WindowWalker w(pic);
	  for (w.init();!w.ready();w.next())
          {
            int markv = mark.getPixel(w);
            int grw = pic.getPixel(w) / 2;
            switch (markv)
              {
              case 1:
                out.setPixel(w, 150);
                break;
              case 2:
                out.setPixel(w, 200);
                break;
              case 3:
                out.setPixel(w, 255);
                break;
              default:
                out.setPixel(w, grw);
                break;
              }
          }
          WriteImg(out, fn);
          ClearAlpha();
        }

      if (selection == 70)
        StorePolygon();

      if (selection == 80)
        {
          // new contour
          clearImg(pic);
          clearImg(mark);

          contur = SelContur(pic, true);

          FillRegion(contur, 222, pic);
          polygon = Matrix(0, 2);
          DrawPolygons();
        }


      if (selection == 81)
        {
          SetAlphaCursor(XP, YP);
          string fn = InputS("Name of file for contur: ") + ".contur";

          SetAlphaCursor(XP, YP);
          Print("\020");

          IMatrix points = ConturPointlist(contur);
          ofstream os(fn.c_str());
          os << Matrix(points);
        }


      if (selection == 82)
        {
          // read contur
          string fn = FileSel("*.contur");

          if (!fn.empty())
            {
              clearImg(pic);
              clearImg(mark);
              ifstream is(fn.c_str());
              Matrix m;
              is >> m;
              contur = PolygonContur(m);
              FillRegion(contur, 222, pic);
              polygon = Matrix(0, 2);
            }
        }

      if (selection == 83)
        {
          // select contur from image
          ClearAlpha();

          Image lseg = NewImg(pic, true);
          //    ReadImg(imagefn,pic);

          LocalSeg(pic, lseg, ls_neighbour, ls_level);

          LocalSeg(pic, mark, ls_neighbour, ls_level);

          IPoint sp = SelPoint(mark);

          clearImg(mark);
          bool ok = false;
          while (!ok)
            {
              SearchStart(lseg, mark, LocalSegObj, 0, 1, sp);

              contur = CalcContur(lseg, mark, LocalSegObj, 0, sp);

              ok = contur.isClosed();
              PutVal(mark, sp.x, sp.y, 1);
            }
          polygon = ConturPointlist(contur);
          mpolygon = Matrix(0, 2);
          DrawPolygons();
        }

      PrintStatus();
      MkMainMenu(men, mid);
      selection = Menu(men, mid, XM1, YM1, XM2, YM2, false, "Polygon Fitting");
    }

  Show(OFF, pic);
  return OK;
}

#include <stdio.h>
#include <math.h>
#include <image.h>

#include <textlist.h>
#include <fstream>

Contur cont1;
Image pic1;
Image mark1;

int cmtype = 0; // Type of Conturmarker

void MkMenu(TextList &men, vector<int> &mid)
{
  men.clear();
  mid.clear();
  if (cont1.isValid())
    {
      men.push_back("Fitting of a ~triangle");
      mid.push_back(1);
      men.push_back("Fitting of a Equilateral triangle");
      mid.push_back(2);
      men.push_back("Fitting of a ~Isosceles triangle");
      mid.push_back(3);
      men.push_back("Fitting of a ~parallelogram");
      mid.push_back(4);
      men.push_back("Fitting of a polygon (3 <= n <= 7)");
      mid.push_back(5);
      men.push_back("Fitting of a ~rectangle");
      mid.push_back(6);
      men.push_back("Fitting of a ~Square");
      mid.push_back(7);
      men.push_back("Fitting of a ~circle");
      mid.push_back(8);
      men.push_back("Fitting of a circular ~segment");
      mid.push_back(9);
      men.push_back("Fitting of a ~ellipse");
      mid.push_back(10);
      men.push_back("Fitting of a elliptical segment");
      mid.push_back(11);
      men.push_back("Fitting of a super ~quadric");
      mid.push_back(12);

      men.push_back("Segmentation of the contour (by Paul Rosin)");
      mid.push_back(13);
      men.push_back("Segmentation of the contour (by Klaus ~Voss)");
      mid.push_back(14);

      men.push_back("Fitting of Polygon and ~Optimization");
      mid.push_back(15);
    }
  men.push_back("~New Contur");
  mid.push_back(16);
  if (cont1.isValid())
    {
      men.push_back("Save Contur");
      mid.push_back(17);
    }
  men.push_back("Load Contur");
  mid.push_back(18);
  men.push_back("~Find Contur in image");
  mid.push_back(83);
  men.push_back("E~xit");
  mid.push_back(99);
}

void CMark()
{
  clearImg(mark1);
  if (cmtype == 0)
    {
      clearImg(pic1);
      FillRegion(cont1, 80, pic1);
    }
  if (cmtype == 1)
    MarkContur(cont1, 1, mark1);
}

void usage(const string &pn)
{
  cout << "Aufruf: " << pn << " [<options>]" << endl;
  cout << "options:" << endl;
  cout << "-x        Width of image " << endl;
  cout << "-y        Height of image" << endl;
  cout << "-c <nr>   set Color of marker" << endl;
  cout << "-i <name> load Image from file <name>" << endl;
  cout << "-l        Level for local segmentation" << endl;
  cout << "-s        Neighborhood for local segmentation" << endl;
  cout << "-h        show this Help" << endl;
  exit(1);
}

int main(int argc, char *argv[])
{
  int dimx = 512;
  int dimy = 512;
  //  int farbe;
  int color = 3;
  int p_in[3][2];
  double circle_par[3];
  double c_seg_par1[2], c_seg_par2[2];
  double moment[21], s[2];
  double t_p[4][2];
  //  double sh;
  double x0, y0, radius, kreis[3], ell_par[5];
  double seg_par1[2], seg_par2[2];
  double sup_c1, sup_c2, sup_tr1[3][3], sup_tr2[3][3];
  double sup_f1, sup_f2;
  int x1, y1, x2, y2, x3, y3, x4, y4;
  string imagefn;

  int ls_level = 20;
  int ls_neighbour = 15;


  int rc;
  while ((rc = getopt(argc, argv, "x:y:c:i:l:s:h")) >= 0)
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
          color = atol(optarg);
          break;
        case 'i':
          imagefn = optarg;
          cmtype = 1;
          break;
        case 'l':
          ls_level = atol(optarg);
          break;
        case 's':
          ls_neighbour = atol(optarg);
          break;
        case 'h':
        default:
          usage(argv[0]);
        }
    }

  Contur cont2, cont1_sh;
  int selection;
  TextList men;
  /*
    Segment Seg1;
    PointList pl;
  */
  /************************************************************/

  if (!imagefn.empty())
    {
      int maxv, chnr;
      InfImgFile(imagefn, dimx, dimy, maxv, chnr);
    }

  pic1 = NewImg(dimx, dimy, 255);
  mark1 = NewImg(dimx, dimy, 255);
  Show(OVERLAY, pic1, mark1);

  for (int i = 0; i < 256; i++)
    {
      int v = 255 - i / 2;
      SetGrayColor(i, v, v, v);
    }

  if (!imagefn.empty())
    {
      ReadImg(imagefn, pic1); //HistogramEqual(pic1,pic1);
      int min, max;
      GetGrayLimits(pic1, min, max);
      GrayTransformLimits(pic1, min, max);

    }
  /***********************************************************/

  Alpha(ON);

  /**********************************************/
  OpenAlpha("Fitting");
  SetAttribute(7, 1, 0, 0);
  ClearAlpha();
  SetAlphaCursor(11, 0);
  Printf("Fitting\n");

  vector<int> mid;

  MkMenu(men, mid);

  while ((selection = Menu(men, mid, 5, 2, 60, 19, false, "Fitting with moments")) != 99)
    {

      ClearAlpha();
      SetAlphaCursor(0, 20);

      /*************************************************/

      if (selection == 4)
        {
          FitParallelogramMoments(moment, t_p);
          x1 = (int)t_p[0][0];
          y1 = (int)t_p[0][1];
          x2 = (int)t_p[1][0];
          y2 = (int)t_p[1][1];
          x3 = (int)t_p[2][0];
          y3 = (int)t_p[2][1];
          x4 = (int)t_p[3][0];
          y4 = (int)t_p[3][1];

          CMark();
          Line(x1, y1, x2, y2, color, DEFAULT, mark1);
          Line(x2, y2, x3, y3, color, DEFAULT, mark1);
          Line(x3, y3, x4, y4, color, DEFAULT, mark1);
          Line(x4, y4, x1, y1, color, DEFAULT, mark1);
        }

      if (selection == 5)
        {
          // Momentenbasiertes Fitting fuer Polygone von n=3 bis n=7
          double pstart[7][2] = {{0.0, 0.0}};
          int anzahl_corners = 3;
          do
            {
              SetAlphaCursor(0, 20);
              anzahl_corners = Input(" Number of corners: ");
            }
          while ((anzahl_corners < 3) || (anzahl_corners > 7));

          Matrix corners_r = ReducePolygon(cont1, anzahl_corners);

          drawPolygon(corners_r, 1, mark1);

          Matrix corners_a;

          try
            {
              corners_a = FitPolygonContur(corners_r, cont1);
            }
          catch (IceException &ex)
            {
              corners_a = corners_r;// try without optimization
              Print("FitPolygonContur failed !!\n");
              Print("continue without optimization !!\n");
              Print("Press <ENTER>");
              GetChar();
            }

          drawPolygon(corners_a, 2, mark1);

          for (int i = 0; i < anzahl_corners; i++)
            {
              pstart[i][0] = corners_a[i][0];
              pstart[i][1] = corners_a[i][1];
            }

          try
            {
              FitPolygonMoments(anzahl_corners, moment, pstart, t_p);
            }
          catch (IceException &ex)
            {
              Print("FitPolygonMoments failed !!\n");
              Print("continue without optimization !!\n");
              Print("Press <ENTER>");
              GetChar();
              for (int i = 0; i < anzahl_corners; i++)
                {
                  t_p[i][0] = corners_a[i][0];
                  t_p[i][1] = corners_a[i][1];
                }
            }

          CMark();
          for (int i = 0; i < anzahl_corners - 1; i++)
            {
              Line((int)t_p[i][0], (int)t_p[i][1], (int)t_p[i + 1][0], (int)t_p[i + 1][1], color, DEFAULT, mark1);
            }
          Line((int)t_p[anzahl_corners - 1][0], (int)t_p[anzahl_corners - 1][1], (int)t_p[0][0], (int)t_p[0][1], color, DEFAULT, mark1);
        }

      if (selection == 1)
        {
          FitTriangleMoments(moment, t_p);
          x1 = p_in[0][0] = (int)t_p[0][0];
          y1 = p_in[0][1] = (int)t_p[0][1];
          x2 = p_in[1][0] = (int)t_p[1][0];
          y2 = p_in[1][1] = (int)t_p[1][1];
          x3 = p_in[2][0] = (int)t_p[2][0];
          y3 = p_in[2][1] = (int)t_p[2][1];
          CMark();
          Line(x1, y1, x2, y2, color, DEFAULT, mark1);
          Line(x2, y2, x3, y3, color, DEFAULT, mark1);
          Line(x3, y3, x1, y1, color, DEFAULT, mark1);
        }

      if (selection == 2)
        {

          FitEquilateraltriangleMoments(moment, t_p);
          x1 = p_in[0][0] = (int)t_p[0][0];
          y1 = p_in[0][1] = (int)t_p[0][1];
          x2 = p_in[1][0] = (int)t_p[1][0];
          y2 = p_in[1][1] = (int)t_p[1][1];
          x3 = p_in[2][0] = (int)t_p[2][0];
          y3 = p_in[2][1] = (int)t_p[2][1];
          CMark();
          Line(x1, y1, x2, y2, color, DEFAULT, mark1);
          Line(x2, y2, x3, y3, color, DEFAULT, mark1);
          Line(x3, y3, x1, y1, color, DEFAULT, mark1);
        }

      if (selection == 3)
        {

          FitIsoscelestriangleMoments(moment, t_p);
          x1 = p_in[0][0] = (int)t_p[0][0];
          y1 = p_in[0][1] = (int)t_p[0][1];
          x2 = p_in[1][0] = (int)t_p[1][0];
          y2 = p_in[1][1] = (int)t_p[1][1];
          x3 = p_in[2][0] = (int)t_p[2][0];
          y3 = p_in[2][1] = (int)t_p[2][1];
          CMark();
          Line(x1, y1, x2, y2, color, DEFAULT, mark1);
          Line(x2, y2, x3, y3, color, DEFAULT, mark1);
          Line(x3, y3, x1, y1, color, DEFAULT, mark1);
        }

      if (selection == 6)
        {
          FitRectangleMoments(moment, t_p);
          x1 = (int)t_p[0][0];
          y1 = (int)t_p[0][1];
          x2 = (int)t_p[1][0];
          y2 = (int)t_p[1][1];
          x3 = (int)t_p[2][0];
          y3 = (int)t_p[2][1];
          x4 = (int)t_p[3][0];
          y4 = (int)t_p[3][1];
          CMark();
          Line(x1, y1, x2, y2, color, DEFAULT, mark1);
          Line(x2, y2, x3, y3, color, DEFAULT, mark1);
          Line(x3, y3, x4, y4, color, DEFAULT, mark1);
          Line(x4, y4, x1, y1, color, DEFAULT, mark1);
        }

      if (selection == 7)
        {
          FitSquareMoments(moment, t_p);
          x1 = (int)t_p[0][0];
          y1 = (int)t_p[0][1];
          x2 = (int)t_p[1][0];
          y2 = (int)t_p[1][1];
          x3 = (int)t_p[2][0];
          y3 = (int)t_p[2][1];
          x4 = (int)t_p[3][0];
          y4 = (int)t_p[3][1];
          CMark();
          Line(x1, y1, x2, y2, color, DEFAULT, mark1);
          Line(x2, y2, x3, y3, color, DEFAULT, mark1);
          Line(x3, y3, x4, y4, color, DEFAULT, mark1);
          Line(x4, y4, x1, y1, color, DEFAULT, mark1);
        }

      if (selection == 8)
        {

          double guete;
          FitCircleMoments(moment, x0, y0, radius, guete);
          //Printf("Güte %lf\n",guete);
          kreis[0] = x0;
          kreis[1] = y0;
          kreis[2] = radius;
          CMark();
          drawCircle(kreis, color, color, NOFILL, mark1);
        }

      if (selection == 9)
        {
          FitCircularSegmentMoments(moment, circle_par, c_seg_par1, c_seg_par2);
          CMark();
          drawCircle(circle_par, color, color, NOFILL, mark1);

          //Printf("%d %d %d %d\n",(int)c_seg_par1[0],(int)c_seg_par1[1],(int)c_seg_par2[0],(int)c_seg_par2[1]);
          //Printf("%lf %lf %lf\n",circle_par[0],circle_par[1],circle_par[2]);

          Line((int)c_seg_par1[0], (int)c_seg_par1[1], (int)c_seg_par2[0], (int)c_seg_par2[1], color, DEFAULT, mark1);
        }

      if (selection == 10)
        {
          FitEllipseMoments(moment, ell_par);
          CMark();
          drawEllipse(ell_par, color, color, NOFILL, mark1);
        }

      if (selection == 11)
        {
          //    sh=
          FitEllipticalSegmentMoments(moment, ell_par, seg_par1, seg_par2);
          CMark();
          drawEllipse(ell_par, color, color, NOFILL, mark1);
          Line((int)seg_par1[0], (int)seg_par1[1], (int)seg_par2[0],
               (int)seg_par2[1], color, DEFAULT, mark1);

        }

      if (selection == 12)
        {
          FitSuperEllipseMoments(moment, sup_c1, sup_f1, sup_tr1, sup_c2, sup_f2, sup_tr2);
          //Printf("Superquadrik c1 f1 %f %f \n",sup_c1,sup_f1);
          //Printf("Superquadrik c2 f2 %f %f \n",sup_c2,sup_f2);
          CMark();
          drawSuperEllipse(sup_c1, sup_tr1, color, mark1);
          //DrawSuperEllipse(sup_c2,sup_tr2,color,mark1);
        }

      if (selection == 13)
        {
          Printf("Segmentation by the algorithm of Paul Rosin\n");

        }

      if (selection == 14)
        {
          Printf("Segmentation by the algorithm of Klaus Voss\n");

        }

      if (selection == 15)
        {
          int anz;
          do
            {
              SetAlphaCursor(0, 20);
              anz = Input(" Number of corners: ");
            }
          while ((anz < 3) || (anz > cont1.Number() / 3));

          SetAlphaCursor(0, 20);
          Printf("\020");

          Matrix p;
          p = ReducePolygon(cont1, anz);
          CMark();
          drawPolygon(p, color, mark1);
          /*
            for (int i=4;i<=anz;i++)
            {
            Matrix h=p;
            p=ReducePolygon(cont1,i,2);
            //  cout << p << endl;
            DrawPolygon(h,1,mark1);
            DrawPolygon(p,color,mark1);
            GetChar();
            }
          */
          Matrix p1;
          try
            {
              p1 = FitPolygonContur(p, cont1);
            }
          catch (IceException ex)
            {
              Print("Optimierung nicht möglich");
            }
          drawPolygon(p1, color + 1, mark1);
        }

      if (selection == 16)
        {
          // new contour

          clearImg(mark1);

          cont1 = SelContur(mark1, TRUE);

          MomentRegion(cont1, moment, s);
          CMark();
        }


      if (selection == 17)
        {
          SetAlphaCursor(0, 20);
          string fn = InputS("Name of file for contur: ") + ".contur";

          SetAlphaCursor(0, 20);
          Printf("\020");

          IMatrix points = ConturPointlist(cont1);
          ofstream os(fn.c_str());
          os << Matrix(points);
        }


      if (selection == 18)
        {
          // read contur

          string fn = FileSel("*.contur");

          if (!fn.empty())
            {
              ifstream is(fn.c_str());
              Matrix m;
              is >> m;
              cont1 = PolygonContur(m);
            }
          MomentRegion(cont1, moment, s);
          if (cmtype == 0)

            CMark();
        }

      if (selection == 83)
        {
          // select contur from image
          ClearAlpha();

          Image lseg = NewImg(pic1, true);
          //      ReadImg(imagefn,pic);

          LocalSeg(pic1, lseg, ls_neighbour, ls_level);

          LocalSeg(pic1, mark1, ls_neighbour, ls_level);

          IPoint sp = SelPoint(mark1);

          clearImg(mark1);
          bool ok = false;
          while (!ok)
            {
              SearchStart(lseg, mark1, LocalSegObj, 0, 1, sp);

              cont1 = CalcContur(lseg, mark1, LocalSegObj, 0, sp);

              ok = cont1.isClosed();
              PutVal(mark1, sp.x, sp.y, 1);
            }
          MomentRegion(cont1, moment, s);
          CMark();
        }

      MkMenu(men, mid);
    }

  Show(OFF, pic1);
  Show(OFF, mark1);

  //  mark1.destroy();
  //  pic1.destroy();

  return OK;
}

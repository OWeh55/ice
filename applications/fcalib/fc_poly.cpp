#include <string>
#include <vector>
#include <fstream>

#include <lseg.h>
#include <conturfunctions.h>
#include <contfeat.h>
#include <geo.h>
#include <drawline.h>

#include "fcalib.h"
#include "pattern.inc"
#include "fc_poly.h"
#include "visual/screen.h"
#include "draw.h"
#include "visual/xio.h"

using namespace std;
using namespace ice;

void generatePointlistWithIndexShift(const vector<Point> &src,
                                     int shift, vector<Point> &dst)
{
  int npoint = src.size();
  dst.resize(npoint);
  for (int i = 0; i < npoint; i++)
    {
      dst[i] = src[(i + shift) % npoint];
    }
}

double PointlistError(const vector<Point> &pl1, const vector<Point> &pl2, double minerror)
{
  int npoints = pl1.size();
  double errorsum = 0.0;
  for (int i = 0; i < npoints && errorsum < minerror; i++)
    {
      errorsum += (pl1[i] - pl2[i]).Length2();
    }
  return errorsum;
}

bool findPoly(const Image &oimg, const Image &mark,
              Trafo &tr)
{
  bool polyfound = false;
  IPoint ps(0, 0);

  // gegebene Polygonkoordinaten aufbereiten
  vector<Point> pattern_pl(pattern_corners);

  for (int i = 0; i < pattern_corners; i++)
    pattern_pl[i] = Point(pattern_x[i], pattern_y[i]);

  Contur c;

  while (SearchStart(oimg, mark, LocalSegObj, 20, 1, ps) == OK && !polyfound)
    {
//         cout << "start: " << ps.x << ", " << ps.y << endl;
      c = CalcContur(oimg, mark, LocalSegObj, 20,
                     ps, 0, isunknown, isunknown);

      PutVal(mark, ps, 3);
      if (c.isValid())
        {
          MarkContur(c, 1, mark); // markieren
          if (c.isClosed() && !c.isHole())
            {
              MarkContur(c, 2, mark); // markieren

              double length, area, form, conv;
              FeatureContur(c, length, area, form, conv);

              //    cout << oimg.xsize*oimg.ysize << " ? " << area << " == "<< form << " " << conv << endl;

              if (area * 10 > oimg.xsize * oimg.ysize && form > 1.70) // Mindestens 10 % der Fläche, "unrund",
                {
                  MarkContur(c, 3, mark); // markieren
                  Polygon p;
                  LimitingPolygon(c, p);

                  Polygon pr = p.Reduced(pattern_corners, 2);

                  draw(pr, mark, 2, 3);

                  //  Printf("Polygon reduziert\n");
                  //  GetChar();

#if 1
                  Polygon fitted(FitPolygonPointlist(pr.PointList(), p.PointList(), 3, 3));
#else
                  Polygon fitted = pr; // shortcut fit
#endif

                  vector<Point> pl_img(pattern_corners);

                  double minerror = (50 * 50) * pattern_corners; // mittlerer Fehler muss kleiner 50 sein
                  // minerror = 1e99;
                  int minoffset = -1;
                  Trafo minTrafo;
                  for (int offset = 0; offset < pattern_corners; offset++)
                    {
                      if (Verbose & v_assign)
                        {
                          cout << " offset: " << offset;
                          cout.flush();
                        }

                      generatePointlistWithIndexShift(fitted.PointList(), offset, pl_img);

                      Trafo ret = MatchPointlists(pattern_pl, pl_img, TRM_PROJECTIVE);

                      vector<Point> pl_corr = pattern_pl;
                      Transform(ret, pl_corr);

                      double error = PointlistError(pl_corr, pl_img, minerror);

                      if (error < minerror)
                        {
                          minerror = error;
                          minoffset = offset;
                        }
                      if (Verbose & v_assign)
                        {
                          cout << ": " << error << " " << minerror << endl;
                        }
                    }
                  if (minoffset >= 0)
                    {
                      polyfound = true;
#if 0
                      ClearImg(mark);
                      generatePointlistWithIndexShift(fitted.PointList(), minoffset, pl_img);
                      Trafo otttt = MatchPointlists(pattern_pl, pl_img, TRM_PROJECTIVE);
                      Point p1a = pl_img[pattern_corners - 1];
                      Point p2a = pattern_pl[pattern_corners - 1];
                      Transform(otttt, p2a);
                      for (int i = 0; i < pattern_corners; i++)
                        {
                          Point p1 = pl_img[i];
                          Point p2 = pattern_pl[i];
                          Transform(otttt, p2);
                          Line(p1, p2, 1, mark);
                          Line(p1a, p1, 2, mark);
                          Line(p2a, p2, 2, mark);
                          p1a = p1;
                          p2a = p2;
                        }
                      GetChar();
#endif
                      if (Verbose & v_step)
                        cout << "Polygon found" << endl;
                      generatePointlistWithIndexShift(fitted.PointList(), minoffset, pl_img);
                      Trafo otr = MatchPointlists(pattern_pl, pl_img, TRM_PROJECTIVE);
                      if (Verbose & v_trafo)
                        {
                          cout << otr.Tmatrix() << endl;
                        }
                      tr = otr;
                    }
                }
            }
        }
    }
  clearImg(mark); // freie bahn für markersuche
  if (polyfound)
    MarkContur(c, 3, mark);
  //  ice::GetChar();
  return polyfound;
}

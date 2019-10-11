#include <string>
#include <vector>
#include <fstream>

#include <lseg.h>
#include <conturfunctions.h>
#include <contfeat.h>
#include <draw.h>
#include <moments.h>
#include <fitmoments.h>
#include <Distortion.h>
#include <paint.h>
#include <visual/screen.h>

#include "fcalib.h"
#include "fc_marker.h"

using namespace std;
using namespace ice;

bool FindMarker(const Image& lseg_image, const Image& marker_image, Point& p)
{
  //  cout << p.x << "," << p.y << endl;
  Point start(p);
  IPoint ps(p);

  if (!lseg_image.inside(p))
    return false;

  if (SearchStart(lseg_image, marker_image, LocalSegObj, 20, 1, ps) != OK)
    {
      PutVal(marker_image, ps, 1);
      return false;
    }

  Contur c = CalcContur(lseg_image, marker_image, LocalSegObj, 20,
                        ps, 0, isunknown, isunknown);

  if (!c.isValid() || !c.isClosed())
    {
      PutVal(marker_image, ps, 1);
      return false;
    }

  MarkContur(c, 1, marker_image);

  if (!c.isHole())
    {
      PutVal(marker_image, ps, 1);
      return false;
    }

  MarkContur(c, 2, marker_image);

  Polygon limiting_polygon;
  LimitingPolygon(c, limiting_polygon);

  Polygon reduced_polygon = limiting_polygon.Reduced(4);

  Moments mom(c);
  Polygon fitted_polygon = FitPolygon(mom, reduced_polygon);

  draw(fitted_polygon, marker_image, 3);

  Point p1(fitted_polygon[0]);
  Point p2(fitted_polygon[1]);
  Point p3(fitted_polygon[2]);
  Point p4(fitted_polygon[3]);

  // Längen der Seiten
  double l1 = (p1 - p2).Length();
  double l2 = (p2 - p3).Length();
  double l3 = (p3 - p4).Length();
  double l4 = (p4 - p1).Length();

  // Längen der Diagonalen
  double l5 = (p1 - p3).Length();
  double l6 = (p2 - p4).Length();

  // wenn Objektgroesse oder Form nicht stimmt, wird das Objekt nicht benutzt

  if (fabs(l1 - l3) > ((l1 + l3) * 0.2)) return false;
  if (fabs(l2 - l4) > ((l2 + l4) * 0.2)) return false;
  if (fabs(l5 - l6) > ((l5 + l6) * 0.7)) return false;

  LineSeg diag1(p1, p3, LineSeg::segment);
  LineSeg diag2(p2, p4, LineSeg::segment);
  if (!diag1.Intersection(diag2, p))
    return false;
  return (p - start).Length() < 10.;
}

bool MarkerSearch(const Image& oimg, const Image& mark,
                  Trafo& tr, Distortion& dist,
                  vector<Point>& markers, vector<Point>& reference)
{
  int nmarkers = 0;
  int good_markers = 0;

  markers.clear();
  reference.clear();

  double distance2 = 0;

  for (int y = 0; y < pattern_rows; y++)
    for (int x = 0; x < pattern_cols; x++)
      {
        nmarkers++;
        Point reference_point(pattern_x0 + x * pattern_dx, pattern_y0 + y * pattern_dy);
        Point estimated_point(reference_point);
        transform(tr, estimated_point);
        estimated_point = dist.distort(estimated_point);
        if (Inside(oimg, IPoint(RoundInt(estimated_point.x), RoundInt(estimated_point.y))))
          {
            Point found_point(estimated_point);
            bool found = FindMarker(oimg, mark, found_point);
            //      cout << estimated_point.x << ", "<<estimated_point.y << endl;
            //      cout << found_point.x << ", "<<found_point.y << endl;
            Marker(3, estimated_point, 2, 7, mark); // erst nach suche markieren!
            if (!found)
              {
                if (Verbose & v_marker)
                  {
                    cout << "-";
                    cout.flush();
                  }
              }
            else
              {
                distance2 += (estimated_point - found_point).r2();
                if (Verbose & v_marker)
                  {
                    cout << ".";
                    cout.flush();
                  }
                Marker(1, found_point, 1, 11, mark);

                good_markers++;
                reference.push_back(reference_point);
                markers.push_back(found_point);
              }
          }
      }

  if (Verbose & v_marker)
    cout << endl;
  if (Verbose & v_step)
    {
      cout << good_markers << " Markers of " << nmarkers << " found" << endl;
      cout << "average distance " << sqrt(distance2 / good_markers) << endl;
    }

  if (wait > 1)
    ice::GetChar();

  return good_markers >= nmarkers * 2 / 10 ;
}

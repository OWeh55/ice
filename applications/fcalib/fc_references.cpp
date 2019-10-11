#include <ImageBase.h>
#include <draw.h>
#include <vector>

#include "visual/screen.h"
#include "fcalib.h"

//#include "pattern.inc"

#include "fc_poly.h"
#include "fc_marker.h"

#include "fc_references.h"

using namespace std;
using namespace ice;

bool GetReferencePoints(const Image& img, const Image& mrk,
                        const Image& segmented_img,
                        vector<Point>& pointList, vector<Point>& referenceList,
                        Trafo& homography, Distortion& dist)
{
  clearImg(mrk);

  // grobe Lageschätzung
  if (!findPoly(segmented_img, mrk, homography))
    {
      cout << "findPoly failed" << endl;
      return false;
    }

#if 1
  // debug: Anzeige des transformierten Polygons
  {
    vector<Point> ppointList;
    for (int i = 0; i < pattern_corners; i++)
      ppointList.push_back(Point(pattern_x[i], pattern_y[i]));
    transform(homography, ppointList);
    Polygon tpoly(ppointList);
    draw(tpoly, mrk, 1);
  }
#endif

  bool found = MarkerSearch(segmented_img, mrk, homography, dist, pointList, referenceList);
  if (found)
    {
      Point distortionCenter(segmented_img->xsize / 2, segmented_img->ysize / 2);
      for (int run = 0; run < 3 && found; ++run)
        {
          // Verfeinerung
          dist.Calc(pointList, referenceList, homography, distortionCenter);
          if (Verbose & v_trafo)
            {
              cout  << dist.toString() << endl;
              //!!!!              cout << homography.getMatrix() << endl;
            }
          clearImg(mrk);

          found = MarkerSearch(segmented_img, mrk, homography, dist, pointList, referenceList);
        }
    }

#if 0
  // debug: Anzeige des transformierten und verzeichneten Polygons
  if (display && found)
    {
      vector<Point> ppointList;
      for (int i = 0; i < pattern_corners; i++)
        ppointList.push_back(Point(pattern_x[i], pattern_y[i]));
      Transform(homography, ppointList);
      Polygon tpoly(ppointList);
      Draw(tpoly, mrk, 1);
      if (wait)
        GetChar();
      vector<Point> dpointList(ppointList);
      for (int i = 0; i < pattern_corners; i++)
        dpointList[i] = dist.Distort(ppointList[i]);
      Polygon dpoly(dpointList);
      Draw(dpoly, mrk, 3);
      if (wait)
        GetChar();
    }
#endif
  return found;
}

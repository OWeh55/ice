#include <vector>
#include <image.h>
// interpolate image from a limited number of known pixels

using namespace std;

void interpolate(Image& img, vector<IPointValue>& pl)
{
  int xsize = img.xsize;
  int ysize = img.ysize;
  ImageD freal;
  freal.create(xsize, ysize);
  ImageD fimag;
  fimag.create(xsize, ysize);
  freal.set(0.0);
  fimag.set(0.0);
  for (int i = 0; i < pl.size(); ++i)
    {

    }
}

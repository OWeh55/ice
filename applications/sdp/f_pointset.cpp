#include "gpointset.h"

#include "f_pointset.h"

void Image2PointSet::get_data()
{
  //  cout << "2pointset" << endl;
  const GImage* src = getInputImage(0);
  int threshold = getInputInt(1, 1);

  GPoint* tresult = new GPoint;
  result[0] = tresult;

  WindowWalker w((*src)[0]);
  for (w.init(); !w.ready(); w.next())
    {
      if ((*src)[0].getPixel(w) >= threshold)
        tresult->push_back(w);
    }
  //  cout << "2pointset done: " << tresult->getSize() << " Punkte" << endl;
}

void PointSet2Image::get_data()
{
  const GPoint* src = dynamic_cast<const GPoint*>(getInputData(0));
  xsize = 1;
  ysize = 1;
  for (int i = 0; i < src->getSize(); ++i)
    {
      const Point& p = (*src)[i];
      xsize = std::max(xsize, RoundInt(p.x));
      ysize = std::max(ysize, RoundInt(p.y));
    }

  xsize = getInputInt(1, xsize);
  ysize = getInputInt(2, ysize);
  vmax = getInputInt(3, 1);
  nImg = 1;

  GImage* tresult = createResult();

  (*tresult)[0].set(0);

  for (int i = 0; i < src->getSize(); ++i)
    {
      const Point& p = (*src)[i];
      (*tresult)[0].setPixelClipped(RoundInt(p.x), RoundInt(p.y), vmax);
    }
}

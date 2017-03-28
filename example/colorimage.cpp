#include <list>
#include <image.h>

const int xsize = 2111;
const int ysize = 1999;

template<class Te, class Ti>
Ti findNearest(const Te& p, Ti first, Ti last)
{
  if (first == last)
    return last;
  Ti nearest = first;
  double minimalDistance = (*first - p).abs2();
  first++;
  for (Ti iter = first; iter != last; iter++)
    {
      double currentDistance = (*iter - p).abs2();
      if (currentDistance < minimalDistance)
        {
          nearest = iter;
          minimalDistance = currentDistance;
        }
    }
  return nearest;
}

int main(int argc, char** argv)
{
  ColorImage ci = ColorImage::createColorImage(xsize, ysize, 255, "testbild");
  ColorImage co = ColorImage::createColorImage(ci, "medianbild");
  //  Show(_RGB,ci.redImage(),ci.greenImage(),ci.blueImage());
  Show(ON, ci);
  Show(ON, co);
  // Show(ON,ci.redImage());
  // Show(ON,ci.greenImage());
  // Show(ON,ci.blueImage());

  //  vector<ColorValue> cvs;
  //list<IPoint> points;
  vector<IPoint> points;

  double tstart = TimeD();
  for (int r = 0; r < 256; r += 16)
    for (int g = 0; g < 256; g += 16)
      for (int b = 0; b < 256; b += 16)
        {
          ColorValue cv(r, g, b);
          //    cvs.push_back(cv);
          int x = Random(xsize - 1);
          int y = Random(ysize - 1);
          points.push_back(IPoint(x, y));
          ci.setPixel(x, y, cv);
        }
  #pragma omp parallel for schedule(dynamic,30)
  for (int y = 0; y < ci.ysize; y++)
    for (int x = 0; x < ci.xsize; x++)
      {
        ColorValue cv = ci.getPixel(*findNearest(IPoint(x, y), points.begin(), points.end()));
        ci.setPixel(x, y, cv);
      }

  cout << "Dauer Erzeugung: " << TimeD() - tstart << endl;

  // 10 + 10 % Rauschen
  for (int n = 0; n < xsize * ysize / 10; n++)
    {
      int x = Random(xsize - 1);
      int y = Random(ysize - 1);
      ci.setPixel(x, y, ColorValue(0, 0, 0));
      x = Random(xsize - 1);
      y = Random(ysize - 1);
      ci.setPixel(x, y, ColorValue(255, 255, 255));
    }

  const int medianSize = 11;
  tstart = TimeD();
  MedianImg(ci, co, medianSize);
  cout << "Median-Dauer: " << TimeD() - tstart << endl;
  co.write("colorimage-median-test.jpg");

  ColorImage cm = newColorImage(ci, "Falscher Median");
  Show(ON, cm);
  MedianImg(ci.redImage(), medianSize, cm.redImage());
  MedianImg(ci.greenImage(), medianSize, cm.greenImage());
  MedianImg(ci.blueImage(), medianSize, cm.blueImage());


  ColorImage diff = newColorImage(ci);
  Show(ON, diff);
  SubImg(co, cm, diff, SMD_SHIFT);
  GetChar();
  return 0;
}

#include <image.h>

//const int nColors = 8;
const int nColors = 16;

void quantizeWithStatistics(const ColorImage& imageIn,
                            Image& quantized,
                            const vector<ColorValue>& colorTable,
                            vector<ColorValue>& mid,
                            vector<int>& diff,
                            vector<int>& count)
{
  // apply a given colortable to an image plus return
  // the statistics of color usage
  // reset statistics
  for (unsigned int i = 0; i < mid.size(); ++i)
    {
      mid[i] = ColorValue(0);
    }
  for (unsigned int i = 0; i < count.size(); ++i)
    {
      count[i] = 0;
    }

  vector<ColorValue> sum(nColors, ColorValue(0));
  vector<int> difsum(nColors, 0);
  // visit all pixels
  WindowWalker w(imageIn);
  for (w.init(); !w.ready(); w.next())
    {
      ColorValue cv = imageIn.getPixelUnchecked(w);
      // find nearest index
      int idx = 0;
      double mindiff = (cv - colorTable[0]).abs2();
      for (int i = 1; i < nColors; ++i)
        {
          double adiff = (cv - colorTable[i]).abs2();
          if (adiff < mindiff)
            {
              idx = i;
              mindiff = adiff;
            }
        }
      quantized.setPixelUnchecked(w, idx);
      difsum[idx] += mindiff;
      sum[idx] += cv;
      ++count[idx];
    }

  // sum -> average
  for (int i = 0; i < nColors; ++i)
    {
      if (count[i] > 0)
        {
          mid[i] = sum[i] / count[i];
          diff[i] = difsum[i] / count[i];
        }
      else
        {
          mid[i] = ColorValue(0);
          diff[i] = 0;
        }
    }
}

void initTable(vector<ColorValue>& colorTable)
{
  // initialize colortable with regular values
  colorTable.resize(nColors);
  int n = 1;
  while (n * n * n < nColors)
    ++n;
  --n;
  int idx = 0;
  for (int r = 0; r < n; ++r)
    for (int g = 0; g < n; ++g)
      for (int b = 0; b < n; ++b)
        colorTable[idx++] = ColorValue(r * 255 / n, g * 255 / n, b * 255 / n);

  while (idx < nColors)
    {
      colorTable[idx++] = ColorValue(Random(255), Random(255), Random(255));
    }
}

void refineColorTableFromStatistics1(vector<ColorValue>& colorTable,
                                     const vector<ColorValue>& mid,
                                     const vector<int>& diff,
                                     const vector<int>& count)
{
  int usedColors = 0;
  for (int i = 0; i < nColors; ++i)
    {
      if (count[i] > 10)
        {
          colorTable[i] = mid[i];
          ++usedColors;
        }
    }

  cout << "used: " << usedColors << endl;

  for (int i = 0; i < nColors; ++i)
    {
      if (count[i] <= 10)
        colorTable[i] = ColorValue(Random(255), Random(255), Random(255));
    }
}

void refineColorTableFromStatistics2(vector<ColorValue>& colorTable,
                                     const vector<ColorValue>& mid,
                                     const vector<int>& diff,
                                     const vector<int>& count)
{
  refineColorTableFromStatistics1(colorTable, mid, diff, count);
  int maxidx = 0;
  int maxd = diff[0];
  for (int i = 1; i < nColors; ++i)
    {
      if (diff[i] > maxd)
        {
          maxd = diff[i];
          maxidx = i;
        }
    }
  int minp1 = 0;
  int minp2 = 1;
  int mind = (mid[minp1] - mid[minp2]).abs2();
  for (int i1 = 0; i1 < nColors - 1; ++i1)
    for (int i2 = i1 + 1; i2 < nColors; ++i2)
      {
        if (i1 != maxidx && i2 != maxidx)
          {
            int adiff = (mid[i1] - mid[i2]).abs2();
            if (adiff < mind)
              {
                mind = adiff;
                minp1 = i1;
                minp2 = i2;
              }
          }
      }
  cout << "maxd: " << maxd << " ??  mind: " << mind << endl;
  cout << "i: " << maxidx << " // " << minp1 << " + " << minp2 << endl;
  if (maxd > mind)
    {
      cout << "split!!!!" << endl;

      colorTable[minp1] = (colorTable[minp1] + colorTable[minp2]) / 2;
      colorTable[minp2] = colorTable[maxidx] + ColorValue(Random(3) - 1, Random(3) - 1, Random(3) - 1);
    }
}

void setColorTable(const vector<ColorValue>& colorTable, Image& quantized)
{
  for (int i = 0; i < nColors; ++i)
    SetGrayColor(quantized, i,
                 colorTable[i].red, colorTable[i].green, colorTable[i].blue);
}

int main(int argc, char** argv)
{
  int xs, ys;
  int mv;
  int fps;
  VideoFile in("Maus.mp4");
  in.getPara(xs, ys, mv, fps);
  cout << xs << "," << ys << " " << mv << " " << fps << endl;
  ColorImage imageIn;
  Image quantized;
  imageIn.create(xs, ys, mv);
  quantized.create(xs, ys, nColors);
  vector<ColorValue> colorTable(nColors);
  initTable(colorTable);

  Show(ON, imageIn);
  Show(ON, quantized);
  int imageIdx = 0;
  while (in.read(imageIn) && imageIdx < 3000)
    cout << imageIdx++ << endl;
  while (in.read(imageIn))
    {
      cout << imageIdx++ << endl;
      for (int i = 0; i < 3; ++i)
        {
          // adapt and apply colortable
          vector<ColorValue> mid(nColors);
          vector<int> diff(nColors);
          vector<int> count(nColors);
          quantizeWithStatistics(imageIn, quantized, colorTable,
                                 mid, diff, count);
          refineColorTableFromStatistics1(colorTable, mid, diff, count);
          setColorTable(colorTable, quantized);
          for (int i = 0; i < nColors; ++i)
            {
              cout << i << ": " << colorTable[i] << " " << mid[i] <<  " " << diff[i] << " " << count[i] << endl;
            }

          GetChar();

          refineColorTableFromStatistics2(colorTable, mid, diff, count);
          setColorTable(colorTable, quantized);
        }
      // GetChar();
    }
  GetChar();
}

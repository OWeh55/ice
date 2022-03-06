#include <image.h>
#include "xdisplay.h"

// helper class for display of n images
// allow for interactive settings

XDisplay::XDisplay(int xSize, int ySize, int nImages):
  windows(nImages), pImages(nImages), sImages(nImages), zooms(nImages, 1),
  shiftxs(nImages, 0), shiftys(nImages, 0),
  factors(nImages, 1.0), offsets(nImages, 0.0), modes(nImages, mSIGNED)
{
#if 0
  int maxx = InfVis(MAXX);
  int maxy = InfVis(MAXY);
  cout << maxx << "x" << maxy << endl;

  int fac = 1;
  while (xSize / fac > maxx || ySize / fac > maxy)
    fac++;
  xSize /= fac;
  ySize /= fac;
#endif

  int nX = 0;
  while (nX * nX < nImages)
    nX++;
  int nY = 0;
  while (nY * nX < nImages)
    nY++;

  xSize += 2 * nX;
  ySize += 2 * nY;
  // cout << xSize << "x" << ySize << endl;

  dImage.create(xSize, ySize, 255);
  dImage.set(255);
  mImage.create(xSize, ySize);
  mImage.set(0);


  iXSize = xSize / nX;
  iYSize = ySize / nY;

  for (int i = 0; i < nImages; i++)
    {
      int ix = i % nX;
      int iy = i / nX;
      int x0 = ix * iXSize;
      int y0 = iy * iYSize;
      windows[i] = Window(x0, y0, x0 + iXSize - 1, y0 + iYSize - 1);
      pImages[i] = dImage(windows[i]);
      Line(x0, y0, x0, y0 + iYSize - 1, 1, mImage);
      Line(x0, y0, x0 + iXSize - 1, y0, 1, mImage);
      Line(x0, y0 + iYSize - 1, x0 + iXSize - 1, y0 + iYSize - 1, 1, mImage);
      Line(x0 + iXSize - 1, y0, x0 + iXSize - 1, y0 + iYSize - 1, 1, mImage);
    }
}

void XDisplay::show()
{
  ice::Show(OVERLAY, dImage, mImage);
}

void XDisplay::setTitle(int i, const string& title)
{
  clearMarker(i);
  Text(title, 5, 5, 2, 1, mImage(windows[i]));
}

void XDisplay::drawText(int i, const string& text, int x, int y)
{
  Text(text, x, y, 3, 1, mImage(windows[i]));
}

void XDisplay::clearMarker(int i)
{
  mImage(windows[i]).set(0);
}

void XDisplay::copyShow(int i)
{
  double zoom = zooms[i];
  double shiftx = shiftxs[i];
  double shifty = shiftys[i];
  double factor = factors[i];
  double offset = offsets[i];
  for (int y = 0; y < iYSize; y++)
    {
      int oy = y * zoom + shifty;
      if (oy < sImages[i].ysize)
        for (int x = 0; x < iXSize; x++)
          {
            int ox = x * zoom + shiftx;
            if (ox < sImages[i].xsize)
              {
                // scale value for display
                double fval = (sImages[i].getPixel(ox, oy) + offset) * factor;
                // round to int
                int val = RoundInt(fval);
                // clip to image value
                pImages[i].setPixelLimited(x, y, val);
              }
            else
              pImages[i].setPixel(x, y, pImages[i].maxval);
          }
      else
        for (int x = 0; x < iXSize; x++)
          pImages[i].setPixel(x, y, pImages[i].maxval);
    }
}

double XDisplay::getZoom(int xs, int ys) const
{
  double zoomX = (double)xs / iXSize;
  double zoomY = (double)ys / iYSize;
  double zoom = zoomX > zoomY ? zoomX : zoomY;

  if (zoom == 0)
    zoom = 1;
  return zoom;
}

void XDisplay::show(int i, const Image& img)
{
  int xs = img.xsize;
  int ys = img.ysize;

  sImages[i].create(xs, ys);
  ConvImgImgD(img, sImages[i]);
  sImages[i].adaptLimits();

  // initial parameter settings
  zooms[i] = getZoom(xs, ys);
  shiftxs[i] = 0.0;
  shiftys[i] = 0.0;
  modes[i] = mUNSIGNED;
  factors[i] = (double)dImage.maxval / (double)img.maxval;
  offsets[i] = 0.0;
  //
  copyShow(i);
}

static double dlog(double x)
{
  if (x > 0)
    return log(x * 0.1 + 1);
  else
    return -log(-x * 0.1 + 1);
}

void XDisplay::estimateScaling(int i)
{
  // limits must be valid
  double sMin = sImages[i].minValue();
  double sMax = sImages[i].maxValue();

  switch (modes[i])
    {
    case mFREE:
      offsets[i] = -sMin;
      factors[i] = dImage.maxval / (sMax - sMin);
      break;
    case mUNSIGNED:
      offsets[i] = 0;
      factors[i] = dImage.maxval / sMax;
      break;
    case mSIGNED:
      factors[i] = dImage.maxval * 0.5 / std::max(fabs(sMax), fabs(sMin));
      offsets[i] = dImage.maxval * 0.5 / factors[i] ;
      break;
    case mUNKNOWN: // should not happen
      throw "internal failure -  wrong mode";
    }
}

void XDisplay::setFactor(int i, double f)
{
  if (f == 0)
    estimateScaling(i);
  else
    factors[i] = f;
  copyShow(i);
}

void XDisplay::setZoom(int i, int f)
{
  int xs = sImages[i].xsize;
  int ys = sImages[i].ysize;
  if (f <= 1)
    {
      zooms[i] = getZoom(xs, ys);
      shiftxs[i] = 0.0;
      shiftys[i] = 0.0;
    }
  else
    {
      zooms[i] = getZoom(xs, ys) / f;
      double fac = 1.0 - 1.0 / f;
      shiftxs[i] = xs / 2 * fac;
      shiftys[i] = ys / 2 * fac;
    }
  copyShow(i);
}

void XDisplay::show(int i, const ImageD& img, bool log, Mode mode)
{
  int xs = img.xsize;
  int ys = img.ysize;
  sImages[i].create(img, true);
  if (log)
    {
      WindowWalker w(img);
      for (w.init(); !w.ready(); w.next())
        {
          sImages[i].setPixel(w, dlog(sImages[i].getPixel(w)));
        }
    }
  sImages[i].adaptLimits();
  double sMin = sImages[i].minValue();
  double sMax = sImages[i].maxValue();
  // guess right mode
  if (mode == mUNKNOWN)
    {
      if (sMax < 0)
        mode = mFREE;
      else if (fabs(sMin) < 0.01 * sMax)
        mode = mUNSIGNED;
      else if (sMin < 0 && sMax > 0)
        mode = mSIGNED;
      else
        mode = mFREE;
    }
  else
    modes[i] = mode;

  // initial scaling of values
  estimateScaling(i);

  // initial zoom settings
  zooms[i] = getZoom(xs, ys);
  shiftxs[i] = 0.0;
  shiftys[i] = 0.0;

  //
  copyShow(i);
}
